#pragma once
#include "Ogre.h"
#include "OgreApplicationContext.h"

class Enemy : public Ogre::FrameListener
{
public:
	Enemy(Ogre::SceneManager* scnMgr, Ogre::SceneNode* playerNode, const Ogre::Vector3& initOffset);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

	void setFlashlight(Ogre::Light* flashLight);
	void respawn();
	bool checkCollisionWithPlayer();  // New method to check collision
private:
	Ogre::SceneManager* mScnMgr;
	Ogre::Entity* mEnemy;
	Ogre::SceneNode* mEnemyNode;
	Ogre::SceneNode* mPlayerNode;
	Ogre::Light* mFlashLight;
	float mFollowSpeed;
	float mConstantY;
	float mTurnSpeed;
	float mHealth;

	bool isInFlashLight();
};

