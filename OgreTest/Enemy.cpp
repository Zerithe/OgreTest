#include "Enemy.h"
#include <iostream>
using namespace std;

Enemy::Enemy(Ogre::SceneManager* scnMgr, Ogre::SceneNode* playerNode, const Ogre::Vector3& initOffset)
	: mScnMgr(scnMgr), mPlayerNode(playerNode), mFlashLight(nullptr), mFollowSpeed(300.0f), mTurnSpeed(5.0f), mConstantY(0), mHealth(100.f)
{
	mEnemy = scnMgr->createEntity("ninja.mesh");
	mEnemyNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	mEnemyNode->attachObject(mEnemy);
	mEnemyNode->setPosition(initOffset);
}

void Enemy::setFlashlight(Ogre::Light* flashLight) {
	mFlashLight = flashLight;
}

bool Enemy::isInFlashLight()
{
	if (!mFlashLight) return false;
	Ogre::Vector3 lightDir = mFlashLight->getDerivedDirection();
	Ogre::Vector3 toEnemy = mEnemyNode->getPosition() - mFlashLight->getParentSceneNode()->getPosition();
	toEnemy.normalise();

	float angle = lightDir.angleBetween(toEnemy).valueDegrees();
	return angle < mFlashLight->getSpotlightOuterAngle().valueDegrees() * 0.5f;
}

void Enemy::respawn() 
{
	float x = static_cast<float>(std::rand() % 10000 - 2500);
	float z = static_cast<float>(std::rand() % 10000 - 2500);
	mEnemyNode->setPosition(Ogre::Vector3(x, mConstantY, z));
	mHealth = 100.0f;
}

bool Enemy::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	//teleport when they hit 0.
	if (mHealth <= 0)
	{
		respawn();
	}

	//player position
	Ogre::Vector3 direction = mPlayerNode->getPosition() - mEnemyNode->getPosition();
	float distance = direction.normalise();

	//move towards player
	if (distance > 1.0f)
	{
		Ogre::Vector3 newPosition = mEnemyNode->getPosition() + direction * mFollowSpeed * evt.timeSinceLastFrame;
		newPosition.y = mConstantY; // Keep the Y position constant
		mEnemyNode->setPosition(newPosition);

		// Calculate the yaw angle
		Ogre::Vector3 srcDirection = mEnemyNode->getOrientation() * Ogre::Vector3::UNIT_Z;
		Ogre::Vector3 flatDirection(direction.x, 0, direction.z);
		flatDirection.normalise();

		// Calculate the target orientation
		Ogre::Quaternion targetOrientation = Ogre::Vector3::UNIT_Z.getRotationTo(flatDirection);

		// Interpolate the current orientation with the target orientation
		Ogre::Quaternion currentOrientation = mEnemyNode->getOrientation();
		Ogre::Quaternion newOrientation = Ogre::Quaternion::Slerp(mTurnSpeed * evt.timeSinceLastFrame, currentOrientation, targetOrientation, true);

		mEnemyNode->setOrientation(newOrientation);

		Ogre::Radian yaw = srcDirection.angleBetween(flatDirection);
		yaw += Ogre::Degree(180);

		mEnemyNode->yaw(yaw);
	}

	bool inFlashLight = isInFlashLight();

	if (inFlashLight)
	{
		mFollowSpeed = -100.0f;
		mHealth -= 10.0f * evt.timeSinceLastFrame;
		cout << mHealth << "\n";

	}
	else
	{
		mFollowSpeed = 300.f;
	}

	return true;
}
