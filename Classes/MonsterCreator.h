#ifndef __MONSTER_CREATOR_H__
#define __MONSTER_CREATOR_H__
#pragma once
#include "CharactorCreator.h"

enum _moveState;

class CharactorCreator;

struct Monster_Property {
	b2Body* body;
	b2BodyDef body_def;
	b2FixtureDef fixture_def;
};

class MonsterCreator : public CharactorCreator {
private:

protected:
	b2World* m_world;

	cocos2d::Texture2D* m_texture;
	Sprite* m_MonsterSprite;
	Sprite* m_RecognizeSprite;
	Monster_Property* m_Monster;
	Monster_Property* m_Recognize;

public:
	MonsterCreator();
	~MonsterCreator();

	virtual b2Body* CreateMonster(b2World* world, Vec2 position, Layer* layer);
	virtual Sprite* AnimationState(_moveState state, Layer* layer);
	void RecognizeMonster(b2World* world, Layer* layer);

	virtual void MonsterPatrol(b2World* world, std::list<Monster> *Monsters, b2Body* Charactor, float movetime, Layer* layer, int foot);
	virtual void SpriteChange(std::list<Monster>::iterator mob, Layer* layer);

	b2Body* getMonsterBody() { return m_Monster->body; }
	b2Body* getRecognizeBody() { return m_Recognize->body; }

	

};
#endif __MONSTER_CREATOR_H__