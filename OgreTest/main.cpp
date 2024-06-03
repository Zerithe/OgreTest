// This file is part of the OGRE project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at https://www.ogre3d.org/licensing.
// SPDX-License-Identifier: MIT

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "SDL2/SDL.h"  // Include SDL2 headers for keycodes
#include "Enemy.h"
#include <iostream>
using namespace std;


void printNodeInfo(const Ogre::SceneNode* node, const std::string& nodeName)
{
    Ogre::Vector3 position = node->getPosition();
    Ogre::Quaternion orientation = node->getOrientation();

    std::cout << nodeName << " Position: "
        << position.x << ", " << position.y << ", " << position.z << std::endl;

    Ogre::Vector3 axis;
    Ogre::Radian angle;
    orientation.ToAngleAxis(angle, axis);

    std::cout << nodeName << " Orientation: Angle: " << angle.valueDegrees()
        << " Axis: " << axis.x << ", " << axis.y << ", " << axis.z << std::endl;
}

void printLightInfo(const Ogre::Light* light, const std::string& lightName)
{
    Ogre::Vector3 position = light->getParentSceneNode()->_getDerivedPosition();
    Ogre::Quaternion orientation = light->getParentSceneNode()->_getDerivedOrientation();
    
    std::cout << lightName << " Position: "
        << position.x << ", " << position.y << ", " << position.z << std::endl;

    Ogre::Vector3 axis;
    Ogre::Radian angle;
    orientation.ToAngleAxis(angle, axis);

    std::cout << lightName << " Orientation: Angle: " << angle.valueDegrees()
        << " Axis: " << axis.x << ", " << axis.y << ", " << axis.z << std::endl;
}

Ogre::Vector3 initOffsetGen() {
    float x = static_cast<float>(std::rand() % 10000 - 2500);
    float z = static_cast<float>(std::rand() % 10000 - 2500);
    float constantY = 0;
    return Ogre::Vector3(x, constantY, z);
}

/*
class KeyHandler : public OgreBites::InputListener, public Ogre::FrameListener
{
public:
    KeyHandler(Ogre::SceneNode* node) : mNode(node), mMoveSpeed(100), mDirection(Ogre::Vector3::ZERO) {}

    bool keyPressed(const OgreBites::KeyboardEvent& evt) override
    {
        switch (evt.keysym.sym)
        {
        case OgreBites::SDLK_ESCAPE:
            Ogre::Root::getSingleton().queueEndRendering();
            break;
        case OgreBites::SDLK_UP:
            mDirection.y = 1;
            break;
        case OgreBites::SDLK_DOWN:
            mDirection.y = -1;
            break;
        case OgreBites::SDLK_LEFT:
            mDirection.x = -1;
            break;
        case OgreBites::SDLK_RIGHT:
            mDirection.x = 1;
            break;
        }
        return true;
    }

    bool keyReleased(const OgreBites::KeyboardEvent& evt) override
    {
        switch (evt.keysym.sym)
        {
        case OgreBites::SDLK_UP:
        case OgreBites::SDLK_DOWN:
            mDirection.y = 0;
            break;
        case OgreBites::SDLK_LEFT:
        case OgreBites::SDLK_RIGHT:
            mDirection.x = 0;
            break;
        }
        return true;
    }

    bool frameRenderingQueued(const Ogre::FrameEvent& evt) override
    {
        Ogre::Vector3 translate = mDirection * mMoveSpeed * evt.timeSinceLastFrame;
        mNode->translate(translate, Ogre::Node::TS_LOCAL);
        return true;
    }

private:
    Ogre::SceneNode* mNode;
    Ogre::Vector3 mDirection;
    float mMoveSpeed;
};
*/

class KeyHandler : public OgreBites::InputListener, public Ogre::FrameListener
{
public:
    KeyHandler(Ogre::SceneNode* camNode, Ogre::Light* flashLight)
        : mCamNode(camNode), mFlashLight(flashLight), mMoveSpeed(400), mBackwardSpeed(100), mDirection(Ogre::Vector3::ZERO), mPitch(0), mYaw(0)
    {
        //capture and hide mouse
        SDL_ShowCursor(SDL_DISABLE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
    
    }

    bool keyPressed(const OgreBites::KeyboardEvent& evt) override
    {
        switch (evt.keysym.sym)
        {
        case SDLK_ESCAPE:
            Ogre::Root::getSingleton().queueEndRendering();
            break;
        case SDLK_w:
            mDirection.z = -1;
            break;
        case SDLK_s:
            mDirection.z = 1;
            break;
        case SDLK_a:
            mDirection.x = -1;
            break;
        case SDLK_d:
            mDirection.x = 1;
            break;
        }
        return true;
    }

    bool keyReleased(const OgreBites::KeyboardEvent& evt) override
    {
        switch (evt.keysym.sym)
        {
        case SDLK_w:
        case SDLK_s:
            mDirection.z = 0;
            break;
        case SDLK_a:
        case SDLK_d:
            mDirection.x = 0;
            break;
        }
        return true;
    }

    bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override
    {
        float sensitivity = 0.1f;
        mYaw -= evt.xrel * sensitivity;
        mPitch -= evt.yrel * sensitivity;

        if (mPitch > 89.0f) mPitch = 89.0f;
        if (mPitch < -89.0f) mPitch = -89.0f;

        mCamNode->setOrientation(Ogre::Quaternion::IDENTITY);
        mCamNode->yaw(Ogre::Degree(mYaw));
        mCamNode->pitch(Ogre::Degree(mPitch));


        return true;
    }


    bool frameRenderingQueued(const Ogre::FrameEvent& evt) override
    {
        float currentSpeed = (mDirection.z > 0) ? mBackwardSpeed : mMoveSpeed;

        Ogre::Vector3 translate = mDirection * currentSpeed * evt.timeSinceLastFrame;
        Ogre::Vector3 position = mCamNode->getPosition();
        Ogre::Vector3 newPosition = mCamNode->getPosition() + mCamNode->getOrientation() * translate;
        newPosition.y = position.y;
       
        mCamNode->setPosition(newPosition);
        //mFlashLightNode->setOrientation(mCamNode->getOrientation());

        //printNodeInfo(mCamNode, "Camera Node");
        //printLightInfo(mFlashLight, "Flashlight Light");
        return true;
    }

private:
    Ogre::SceneNode* mCamNode;
    Ogre::Vector3 mDirection;
    Ogre::Light* mFlashLight;
    float mMoveSpeed;
    float mBackwardSpeed;
    float mPitch;
    float mYaw;
};


int main(int argc, char* argv[])
{
    //! [constructor]
    OgreBites::ApplicationContext ctx("FlashlightGame");
    ctx.initApp();
    //! [constructor]

    //! [setup]
        // get a pointer to the already created root
    Ogre::Root* root = ctx.getRoot();
    Ogre::SceneManager* scnMgr = root->createSceneManager();

    // register our scene with the RTSS
    Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    //floor plane thing
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane(
        "ground", Ogre::RGN_DEFAULT,
        plane,
        10000, 10000, 40, 40,
        true,
        1, 30, 30,
        Ogre::Vector3::UNIT_Z);
    Ogre::Entity* groundEntity = scnMgr->createEntity("ground");
    scnMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
    groundEntity->setCastShadows(false);
    groundEntity->setMaterialName("Examples/Rockwall");

    // without light we would just get a black screen    
    /*
    Ogre::Light* light = scnMgr->createLight("MainLight");
    Ogre::SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    lightNode->attachObject(light);
    lightNode->setPosition(20, 80, 50);
    */

    ////global light
    //Ogre::Light* directionaLight = scnMgr->createLight("DirectionalLight");
    //directionaLight->setType(Ogre::Light::LT_DIRECTIONAL);
    //directionaLight->setDiffuseColour(Ogre::ColourValue(0.1, 0.1, 0.1));
    //directionaLight->setSpecularColour(Ogre::ColourValue(0.1, 0.1, 0.1));
    //Ogre::SceneNode* directionaLightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    //directionaLightNode->attachObject(directionaLight);
    //directionaLightNode->setDirection(Ogre::Vector3(0, -1, 1).normalisedCopy());

    // also need to tell where we are
    Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    camNode->setPosition(0, 188, 0);

    // create the camera
    Ogre::Camera* cam = scnMgr->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);

    // and tell it to render into the main window
    ctx.getRenderWindow()->addViewport(cam);

    ////flashlight obj not part of char yet
    //Ogre::Entity* flashobj = scnMgr->createEntity("Mesh_4.mesh");
    //Ogre::SceneNode* flashnode = scnMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, 160, 0));
    //flashnode->setScale(30, 30, 30);
    //flashnode->attachObject(flashobj);

    //flashlight light
    Ogre::Light* flashLight = scnMgr->createLight("DirectionalLight");
    flashLight->setType(Ogre::Light::LT_SPOTLIGHT);
    flashLight->setDiffuseColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f)); // white light
    flashLight->setSpecularColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
    //flashLight->setAttenuation(800, 0.0, 0.007, 0.0002);
    //flashLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));
    //Ogre::SceneNode* flashLightNode = camNode->createChildSceneNode(Ogre::Vector3(-10, -26, -10));
    //flashLightNode->attachObject(flashLight);
    camNode->attachObject(flashLight);
    

    //// finally something to render
    //Ogre::Entity* ent = scnMgr->createEntity("ninja.mesh");
    //Ogre::SceneNode* node = scnMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-188,0,0));
    //node->attachObject(ent);
    ////! [setup]

    //// head 2
    //Ogre::Entity* ent2 = scnMgr->createEntity("ogrehead.mesh");
    //Ogre::SceneNode* node2 = scnMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(180,10,0));
    //node2->setScale(1, 1, 1);
    //node2->attachObject(ent2);
    
    //fog
    Ogre::ColourValue fadeColour(0.2, 0.2, 0.2);
    scnMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0, 500, 800);
    ctx.getRenderWindow()->getViewport(0)->setBackgroundColour(fadeColour);

    //! [main]
        // register for input events
    KeyHandler keyHandler(camNode, flashLight);
    ctx.addInputListener(&keyHandler);
    root->addFrameListener(&keyHandler);

    
    Enemy* enemy1 = new Enemy(scnMgr, camNode, initOffsetGen());
    Enemy* enemy2 = new Enemy(scnMgr, camNode, initOffsetGen());
    Enemy* enemy3 = new Enemy(scnMgr, camNode, initOffsetGen());
    root->addFrameListener(enemy1);
    root->addFrameListener(enemy2);
    root->addFrameListener(enemy3);
    enemy1->setFlashlight(flashLight);
    enemy2->setFlashlight(flashLight);
    enemy3->setFlashlight(flashLight);




    ctx.getRoot()->startRendering();
    ctx.closeApp();
    //! [main]
    return 0;
}
