#ifndef __TILED_BODY_CREATOR_H__
#define __TILED_BODY_CREATOR_H__

#include "cocos2d.h"
#include <Box2D/Box2D.h>

USING_NS_CC;

class HelloWorldScene;

class FixtureDef {
public:
    FixtureDef()
    : next(nullptr) {}
    
    ~FixtureDef() {
        delete next;
        delete fixture.shape;
    }
    
    FixtureDef *next;
    b2FixtureDef fixture;
    int callbackData;
};

class BodyDef {
public:
    BodyDef()
    : fixtures(nullptr) {}
    
    ~BodyDef() {
        if (fixtures)
            delete fixtures;
    }
    
    FixtureDef *fixtures;
    Point anchorPoint;
};

class TiledBodyCreator : public cocos2d::Scene
{
private :
	int m_nTileGid;
	b2Body* collisionBody;
	b2Body* itemBody;
	b2Body* bulletBody;
	b2Body* bombBody;
	b2Body* LadderBody;

	b2World* m_world;

	Texture2D* texture;
	Sprite* m_brokenblock;
	Sprite* m_brokenblock2;

	TMXLayer* collisionGround;
	TMXLayer* collisionItem;

public:
	
	std::list<b2Body*> lb_Heartlist; // 생명리스트
	std::list<b2Body*> lb_Obstaclelist; // 장애물리스트
	std::list<b2Body*> lb_Pickaxelist;
	std::list<b2Body*> lb_Swordlist;
	std::list<b2Body*> lb_Gunlist;
	std::list<b2Body*> lb_Bulletlist;
	std::list<b2Body*> lb_Bomblist;
	std::list<b2Body*> lb_Ladderlist;

	TMXLayer* getLayer() { return collisionGround; };
	TMXLayer* getItemLayer() { return collisionItem; };

	b2Body* getItemBody() { return itemBody; };
	b2Body* getBulletBody() { return bulletBody; };
	b2Body* getBombBody() { return bombBody; };
	b2Body* getLadderBody() { return LadderBody; };

	Sprite* getBrokenBlock() { return m_brokenblock; };
	Sprite* getBrokenBlock2() { return m_brokenblock2; };

	void initCollisionMap(TMXTiledMap* map, b2World* world);
	Sprite* CreatePickaxe(b2Vec2 position, b2World* world, Layer* layer);
	Sprite* CreateSword(b2Vec2 position, b2World* world, Layer* layer);
	Sprite* CreateGun(b2Vec2 position, b2World* world, Layer* layer);
	Sprite* CreateBullet(b2Vec2 position, b2World* world, Layer* layer);
	b2Body* CreateBomb(b2Vec2 position, b2World* world, Layer* layer);
	void CreateObj(std::string obj_name, b2Vec2 position, b2World* world, Layer* layer);
	b2Body* WhatObj(std::string sPath, std::list<b2Body*>* lb_list, b2Vec2 position, b2World* world, Layer* layer, Size _spsize, Size _objsize, INT16 _category, INT16 _maskbits, int setdata);

	static FixtureDef* createFixture(ValueMap object);

	static FixtureDef* createPolygon(ValueMap object);
	static FixtureDef* createPolyline(ValueMap object);
	static FixtureDef* createCircle(ValueMap object);
	static FixtureDef* createRect(ValueMap object);
	
};

#endif // __TILED_BODY_CREATOR_H__
