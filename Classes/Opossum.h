#ifndef __Opossum_H__
#define __Opossum_H__
#pragma once
#include "MonsterCreator.h"

enum _moveState;

class CharactorCreator;

class Opossum : public MonsterCreator {
private:
	bool m_bJump;
	float jump_time = 0;

	b2WheelJoint* m_spring1, *m_spring2;

public:
	Opossum();
	~Opossum();

	virtual b2Body* CreateMonster(b2World* world, Vec2 position, Layer* layer);
	virtual Sprite* AnimationState(_moveState state, Layer* layer);
	virtual void RecognizeMonster(b2World* world, Layer* layer);
	b2Body* CreateBody(b2Vec2 point, Size size, b2BodyType bodytype, const char* spriteName, int type, Layer* layer);


	virtual void MonsterPatrol(b2World* world, std::list<Monster> *Monsters, b2Body* Charactor, float movetime, Layer* layer, int foot);
	virtual void SpriteChange(std::list<Monster>::iterator mob, Layer* layer);

	b2Body* getMonsterBody() { return m_Monster->body; }
	b2Body* getRecognizeBody() { return m_Recognize->body; }
};
#endif __Opossum_H__