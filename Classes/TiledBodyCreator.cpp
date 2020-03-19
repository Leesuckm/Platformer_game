#include "TiledBodyCreator.h"
#include "HelloWorldScene.h"

USING_NS_CC;

#define PTMRATIO 32

typedef enum {
	POLYGON_FIXTURE,
	POLYLINE_FIXTURE,
    RECT_FIXTURE,
	CIRCLE_FIXTURE,
	UNKNOWN_FIXTURE
} fixtureTypes;



void TiledBodyCreator::initCollisionMap(TMXTiledMap* map, b2World* world)
{
	// tile map에 있는 Collision 오브젝트를 가져옴
	//auto collisionGroup = map->getObjectGroup("Collision");
	collisionGround = map->getLayer("Ground");
	collisionItem = map->getLayer("Item");
	// ValueVector 는 typedef std::vector<Value> Value라는 클래스의 vector 구조이다.
	//cocos2d::ValueVector collisionObjects = collisionGroup->getObjects();
	m_world = world;
	texture = Director::getInstance()->getTextureCache()->addImage("Images/brokenblock.png");
	m_brokenblock = Sprite::createWithTexture(texture, Rect(0, 0, 32, 32));
	m_brokenblock2 = Sprite::createWithTexture(texture, Rect(32, 0, 64, 32));

	b2BodyDef bd[32][60];
	int k = 31;
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 60; j++) {
			bd[i][j].userData = collisionGround->getTileAt(Vec2(j, k));
			if (collisionItem->getTileAt(Vec2(j, k)) != nullptr) {			// 아이템 위치 Layer가 Background와 Item이 서로 다르므로 Sprite가 서로 겹치지 않도록 주의해야한다.
				bd[i][j].userData = collisionItem->getTileAt(Vec2(j, k));	// Item Layer에 최소 하나의 Object가 있어야 한다.
			}
			if (bd[i][j].userData != nullptr) {
				bd[i][j].position = b2Vec2(j, i);
			}
		}
		k--;
	}
	/*
	k = 31;
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 60; j++) {
			collisionBody = world->CreateBody(&bd[i][j]); // 타일을 넣는 시점.

			cocos2d::Value objectValue;
			//objectValue = collisionObjects.at(i);
			//objectValue.asValueMap();
			//cocos2d::ValueMap object = objectValue.asValueMap();

			auto position = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
			float width = 32 / PTMRATIO;
			float height = 32 / PTMRATIO;

			b2PolygonShape *rectshape = new b2PolygonShape();
			b2Vec2 vertices[4];
			int vindex = 4;

			vertices[0].x = position.x + 0.0f;
			vertices[0].y = position.y + 0.0f;

			vertices[1].x = position.x + 0.0f;
			vertices[1].y = position.y + height;

			vertices[2].x = position.x + width;
			vertices[2].y = position.y + height;

			vertices[3].x = position.x + width;
			vertices[3].y = position.y + 0.0f;

			auto fix = new FixtureDef();
			rectshape->Set(vertices, vindex);
			fix->fixture.shape = rectshape;

			m_nTileGid = this->collisionGround->getTileGIDAt(Vec2(j, k));
			if (m_nTileGid == 6) {
				b2Fixture* footSensorFixture = collisionBody->CreateFixture(&fix->fixture);
				footSensorFixture->SetUserData((void*)5);
				continue;
			}

			if (m_nTileGid == 2) {
				
			}

			collisionBody->CreateFixture(&fix->fixture);
		}
		k--;
	}*/

	/*
	각 Objects의 충돌 Filter Bits
	
	Charactor
		Kategorie_Bits = 0x0001
		Mask_Bits	   = 0xFFFF
	Item
		Kategorie_Bits = 0x1000
		Mask_Bits	   = 0x000F
	Obstacle
		Kategorie_Bits = 0x0100
		Mask_Bits	   = 0x000F
	Monster
		Kategorie_Bits = 0x0010
		Mask_Bits	   = 0x000F
	Block
		Kategorie_Bits = 0x0002
		Mask_Bits	   = 0xFFFF
	Pickaxe
		Kategorie_Bits = 0x2000
		Mask_Bits	   = 0x000E
	Bomb
		Kategorie_Bits = 0x3000
		Mask_Bits	   = 0x000E
	Bomb_bullet
		Kategorie_Bits = 0x0003
		Mask_Bits	   = 0x000F

	*/

	k = 31;
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 60; j++) {
			//collisionBody = world->CreateBody(&bd[i][j]); // 타일을 넣는 시점.
			collisionBody = world->CreateBody(&bd[i][j]);
			collisionBody->SetIsCh(false);
			cocos2d::Value objectValue;

			collisionBody->setNewobj(false);
			auto position = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
			float width = 32 / PTMRATIO;
			float height = 32 / PTMRATIO;

			b2PolygonShape *rectshape = new b2PolygonShape();
			b2Vec2 vertices[4];
			int vindex = 4;

			auto fix = new FixtureDef();
			b2Fixture* SensorFixture;

			m_nTileGid = this->collisionGround->getTileGIDAt(Vec2(j, k));
			if (m_nTileGid != 0 && m_nTileGid != 2 && m_nTileGid != 6 && m_nTileGid != 9 && m_nTileGid != 14) {
				log("da");
			}

			switch (m_nTileGid) {
			case 2 : // 물약
				vertices[0].x = position.x + 0.2f;
				vertices[0].y = position.y + 0.0f;

				vertices[1].x = position.x + 0.2f;
				vertices[1].y = position.y + (height - 0.3);

				vertices[2].x = position.x + (width - 0.3);
				vertices[2].y = position.y + (height - 0.3);

				vertices[3].x = position.x + (width - 0.3);
				vertices[3].y = position.y + 0.0f;
				rectshape->Set(vertices, vindex);
				fix->fixture.shape = rectshape;
				fix->fixture.filter.categoryBits = 0x1000;
				fix->fixture.filter.maskBits = 0x000F;
				//fix->fixture.filter.groupIndex = -1;

				collisionBody->SetHp(1);			// 초기 물약의 Hp (캐릭터로부터 공격받을시에 어느정도의 공격을 버틸 수 있는지)

				SensorFixture = collisionBody->CreateFixture(&fix->fixture);
				SensorFixture->SetUserData((void*)6);

				lb_Heartlist.push_back(collisionBody);
		
				break;

			case 6 : // 장애물
				vertices[0].x = position.x + 0.2f;
				vertices[0].y = position.y + 0.0f;

				vertices[1].x = position.x + 0.2f;
				vertices[1].y = position.y + height / 2;

				vertices[2].x = position.x + (width - 0.3);
				vertices[2].y = position.y + height / 2;

				vertices[3].x = position.x + (width - 0.3);
				vertices[3].y = position.y + 0.0f;
				rectshape->Set(vertices, vindex);
				fix->fixture.shape = rectshape;
				fix->fixture.filter.categoryBits = 0x0100;
				fix->fixture.filter.maskBits = 0x000F;

				collisionBody->SetHp(100);			// 초기 장애물의 Hp (캐릭터로부터 공격받을시에 어느정도의 공격을 버틸 수 있는지)

				SensorFixture = collisionBody->CreateFixture(&fix->fixture);
				SensorFixture->SetUserData((void*)5);

				lb_Obstaclelist.push_back(collisionBody);

				break;

			case 9:
			case 14: // 기본 블럭
				vertices[0].x = position.x + 0.0f;
				vertices[0].y = position.y + 0.0f;

				vertices[1].x = position.x + 0.0f;
				vertices[1].y = position.y + height;

				vertices[2].x = position.x + width;
				vertices[2].y = position.y + height;

				vertices[3].x = position.x + width;
				vertices[3].y = position.y + 0.0f;

				collisionBody->SetHp(3);			// 초기 블럭의 Hp (캐릭터로부터 공격받을시에 어느정도의 공격을 버틸 수 있는지)
				rectshape->Set(vertices, vindex);
				fix->fixture.shape = rectshape;
				fix->fixture.filter.categoryBits = 0x0002;
				fix->fixture.filter.maskBits = 0xFFFF;

				collisionBody->CreateFixture(&fix->fixture);
				break;
			}


			// ItemLayer
			m_nTileGid = this->collisionItem->getTileGIDAt(Vec2(j, k));
			if (m_nTileGid != 0) {
				log("gid = %f", m_nTileGid);
			}
		
			switch (m_nTileGid) {
			case 21:    // 폭탄
				vertices[0].x = position.x + 0.0f;
				vertices[0].y = position.y + 0.0f;

				vertices[1].x = position.x + 0.0f;
				vertices[1].y = position.y + height;

				vertices[2].x = position.x + width;
				vertices[2].y = position.y + height;

				vertices[3].x = position.x + width;
				vertices[3].y = position.y + 0.0f;

				collisionBody->SetHp(10);
				collisionBody->SetType(b2_dynamicBody);

				rectshape->Set(vertices, vindex);
				fix->fixture.shape = rectshape;
				fix->fixture.isSensor = false;
				fix->fixture.filter.categoryBits = 0x3000;
				fix->fixture.filter.maskBits = 0x000E;

				SensorFixture = collisionBody->CreateFixture(&fix->fixture);
				SensorFixture->SetUserData((void*)9);

				lb_Bomblist.push_back(collisionBody);
				break;


			case 22:	// 곡괭이
				vertices[0].x = position.x + 0.2f;
				vertices[0].y = position.y + 0.0f;

				vertices[1].x = position.x + 0.2f;
				vertices[1].y = position.y + (height - 0.3);

				vertices[2].x = position.x + (width - 0.3);
				vertices[2].y = position.y + (height - 0.3);

				vertices[3].x = position.x + (width - 0.3);
				vertices[3].y = position.y + 0.0f;

				collisionBody->SetHp(10);
				collisionBody->SetType(b2_dynamicBody);

				rectshape->Set(vertices, vindex);
				fix->fixture.shape = rectshape;
				fix->fixture.isSensor = false;
				fix->fixture.filter.categoryBits = 0x2000;
				fix->fixture.filter.maskBits = 0x000E;

				SensorFixture = collisionBody->CreateFixture(&fix->fixture);
				SensorFixture->SetUserData((void*)7);

				lb_Pickaxelist.push_back(collisionBody);
				break;

			case 23:  // 검
				vertices[0].x = position.x + 0.2f;
				vertices[0].y = position.y + 0.0f;

				vertices[1].x = position.x + 0.2f;
				vertices[1].y = position.y + (height - 0.3);

				vertices[2].x = position.x + (width - 0.3);
				vertices[2].y = position.y + (height - 0.3);

				vertices[3].x = position.x + (width - 0.3);
				vertices[3].y = position.y + 0.0f;

				collisionBody->SetHp(10);
				collisionBody->SetType(b2_dynamicBody);

				rectshape->Set(vertices, vindex);
				fix->fixture.shape = rectshape;
				fix->fixture.isSensor = false;
				fix->fixture.filter.categoryBits = 0x4000;
				fix->fixture.filter.maskBits = 0x000E;

				SensorFixture = collisionBody->CreateFixture(&fix->fixture);
				SensorFixture->SetUserData((void*)11);

				lb_Swordlist.push_back(collisionBody);
				break;
			case 24:  // 권총
				vertices[0].x = position.x + 0.2f;
				vertices[0].y = position.y + 0.0f;

				vertices[1].x = position.x + 0.2f;
				vertices[1].y = position.y + (height - 0.3);

				vertices[2].x = position.x + (width - 0.3);
				vertices[2].y = position.y + (height - 0.3);

				vertices[3].x = position.x + (width - 0.3);
				vertices[3].y = position.y + 0.0f;

				collisionBody->SetHp(10);
				collisionBody->SetType(b2_dynamicBody);

				rectshape->Set(vertices, vindex);
				fix->fixture.shape = rectshape;
				fix->fixture.isSensor = false;
				fix->fixture.filter.categoryBits = 0x5000;
				fix->fixture.filter.maskBits = 0x000E;

				SensorFixture = collisionBody->CreateFixture(&fix->fixture);
				SensorFixture->SetUserData((void*)12);

				lb_Gunlist.push_back(collisionBody);
				break;
			case 25: // 사다리
				vertices[0].x = position.x + 0.0f;
				vertices[0].y = position.y + 0.0f;

				vertices[1].x = position.x + 0.0f;
				vertices[1].y = position.y + height;

				vertices[2].x = position.x + width;
				vertices[2].y = position.y + height;

				vertices[3].x = position.x + width;
				vertices[3].y = position.y + 0.0f;

				collisionBody->SetHp(10);
				collisionBody->SetType(b2_dynamicBody);

				rectshape->Set(vertices, vindex);
				fix->fixture.shape = rectshape;
				fix->fixture.isSensor = false;
				fix->fixture.filter.categoryBits = 0x7000;
				fix->fixture.filter.maskBits = 0x000E;

				SensorFixture = collisionBody->CreateFixture(&fix->fixture);
				SensorFixture->SetUserData((void*)13);

				lb_Ladderlist.push_back(collisionBody);
				break;

			}


			//collisionBody->CreateFixture(&fix->fixture);
		}
		k--;
	}



	//int i = 0;
	//int j = 0;
	// oop 에서의 A : B는 B를 A에 계속 넣겠다는 의미이다. B에 꺼낼 객체가 없을때까지
	/*
	for(cocos2d::Value objectValue : collisionObjects)
	{
		if (j > 60) {
			j = 0;
			i++;
		}
		

		collisionBody = world->CreateBody(&bd[i][j]);
		auto fixtureShape = createFixture(objectValue.asValueMap());
		if(fixtureShape != NULL) {
			collisionBody->CreateFixture(&fixtureShape->fixture);
		}
		j++;
	}*/
}


Sprite* TiledBodyCreator::CreatePickaxe(b2Vec2 position, b2World* world, Layer* layer) {
	b2BodyDef bd;
	//Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/pickaxe.png");
	//Sprite* Pickaxe_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 40, 32));
	Sprite* Pickaxe_Sprite = Sprite::create("Images/pickaxe.png");
	Pickaxe_Sprite->setAnchorPoint(Vec2(0, 0));
	bd.userData = Pickaxe_Sprite;

	layer->addChild(Pickaxe_Sprite, 3);
	bd.position = position;
	
	itemBody = world->CreateBody(&bd);
	itemBody->SetIsCh(false);
	
	itemBody->setNewobj(true);
	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = posi.x + 0.2f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.2f;
	vertices[1].y = posi.y + (height - 0.3);

	vertices[2].x = posi.x + (width - 0.3);
	vertices[2].y = posi.y + (height - 0.3);

	vertices[3].x = posi.x + (width - 0.3);
	vertices[3].y = posi.y + 0.0f;

	itemBody->SetHp(10);
	itemBody->SetType(b2_dynamicBody);

	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;
	fix->fixture.isSensor = false;
	fix->fixture.filter.categoryBits = 0x2000;
	fix->fixture.filter.maskBits = 0x000E;

	SensorFixture = itemBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)7);

	lb_Pickaxelist.push_back(itemBody);

	return Pickaxe_Sprite;
}

Sprite* TiledBodyCreator::CreateSword(b2Vec2 position, b2World* world, Layer* layer) {
	b2BodyDef bd;
	//Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/pickaxe.png");
	//Sprite* Pickaxe_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 40, 32));
	Sprite* Sword_Sprite = Sprite::create("Images/Sword.png");
	Sword_Sprite->setAnchorPoint(Vec2(0, 0));
	bd.userData = Sword_Sprite;

	layer->addChild(Sword_Sprite, 3);
	bd.position = position;

	itemBody = world->CreateBody(&bd);
	itemBody->SetIsCh(false);

	itemBody->setNewobj(true);
	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = posi.x + 0.2f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.2f;
	vertices[1].y = posi.y + (height - 0.3);

	vertices[2].x = posi.x + (width - 0.3);
	vertices[2].y = posi.y + (height - 0.3);

	vertices[3].x = posi.x + (width - 0.3);
	vertices[3].y = posi.y + 0.0f;

	itemBody->SetHp(10);
	itemBody->SetType(b2_dynamicBody);

	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;
	fix->fixture.isSensor = false;
	fix->fixture.filter.categoryBits = 0x4000;
	fix->fixture.filter.maskBits = 0x000E;

	SensorFixture = itemBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)11);

	lb_Swordlist.push_back(itemBody);

	return Sword_Sprite;
}

Sprite* TiledBodyCreator::CreateGun(b2Vec2 position, b2World* world, Layer* layer) {
	b2BodyDef bd;
	//Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/pickaxe.png");
	//Sprite* Pickaxe_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 40, 32));
	Sprite* Gun_Sprite = Sprite::create("Images/Items/Gun.png");
	Gun_Sprite->setAnchorPoint(Vec2(0, 0));
	bd.userData = Gun_Sprite;

	layer->addChild(Gun_Sprite, 3);
	bd.position = position;

	itemBody = world->CreateBody(&bd);
	itemBody->SetIsCh(false);

	itemBody->setNewobj(true);
	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = posi.x + 0.2f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.2f;
	vertices[1].y = posi.y + (height - 0.3);

	vertices[2].x = posi.x + (width - 0.3);
	vertices[2].y = posi.y + (height - 0.3);

	vertices[3].x = posi.x + (width - 0.3);
	vertices[3].y = posi.y + 0.0f;

	itemBody->SetHp(10);
	itemBody->SetType(b2_dynamicBody);

	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;
	fix->fixture.isSensor = false;
	fix->fixture.filter.categoryBits = 0x5000;
	fix->fixture.filter.maskBits = 0x000E;

	SensorFixture = itemBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)12);

	lb_Gunlist.push_back(itemBody);

	return Gun_Sprite;
}

Sprite* TiledBodyCreator::CreateBullet(b2Vec2 position, b2World* world, Layer* layer) {
	b2BodyDef bd;
	//Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/pickaxe.png");
	//Sprite* Pickaxe_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 40, 32));
	Sprite* Bullet_Sprite = Sprite::create("Images/bullet.png");
	Bullet_Sprite->setAnchorPoint(Vec2(0, 0));
	bd.userData = Bullet_Sprite;

	layer->addChild(Bullet_Sprite, 3);
	bd.position = position;

	bulletBody = world->CreateBody(&bd);
	bulletBody->SetIsCh(false);

	bulletBody->setNewobj(true);
	b2CircleShape CircleShape;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	bulletBody->SetHp(10);
	bulletBody->SetGravityScale(0.1);
	bulletBody->SetType(b2_dynamicBody);

	CircleShape.m_radius = 0.3f;
	fix->fixture.shape = &CircleShape;
	fix->fixture.isSensor = true;
	fix->fixture.filter.categoryBits = 0x6000;
	fix->fixture.filter.maskBits = 0x000E;

	SensorFixture = bulletBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)13);

	lb_Bulletlist.push_back(bulletBody);

	return Bullet_Sprite;
}


b2Body* TiledBodyCreator::CreateBomb(b2Vec2 position, b2World* world, Layer* layer) {
	b2BodyDef bd;
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/Items/bomb.png");

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(1.0);

	for (int i = 0; i < 3; i++) {
		animation->addSpriteFrameWithTexture(texture,
			Rect(i * 32, 0, 32, 34));
	}

	Sprite* Bomb_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 32, 34));
	Bomb_Sprite->setAnchorPoint(Vec2(0, 0));

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);

	Bomb_Sprite->runAction(rep);

	layer->addChild(Bomb_Sprite, 3);
	bd.position = position;
	bd.userData = Bomb_Sprite;

	bombBody = world->CreateBody(&bd);
	bombBody->SetHp(100);
	bombBody->SetIsCh(false);

	bombBody->setNewobj(true);
	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = posi.x + 0.0f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.0f;
	vertices[1].y = posi.y + height;

	vertices[2].x = posi.x + width;
	vertices[2].y = posi.y + height;

	vertices[3].x = posi.x + width;
	vertices[3].y = posi.y + 0.0f;

	bombBody->SetHp(10);
	bombBody->SetType(b2_dynamicBody);

	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;
	fix->fixture.isSensor = false;
	fix->fixture.filter.categoryBits = 0x2000;
	fix->fixture.filter.maskBits = 0x000F;

	SensorFixture = bombBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)9);

	return bombBody;
}

void TiledBodyCreator::CreateObj(std::string obj_name, b2Vec2 position, b2World* world, Layer* layer) {
	if (obj_name == "Ladder") {
		b2Body* body = WhatObj("Images/Items/LongLadder.png", &lb_Ladderlist, position, world, layer, Size(32, 32), Size(0, 0), 0x7007, 0x000E, 13);
		b2Body* Roofbody;
		b2BodyDef bodydef;
		bodydef.type = b2_kinematicBody;
		bodydef.position = body->GetPosition() + b2Vec2(1.0f, 6);

		b2PolygonShape polygonshape;
		polygonshape.SetAsBox(0.5, 0.1);

		b2FixtureDef Fixture;
		Fixture.isSensor = false;
		Fixture.shape = &polygonshape;
		Fixture.density = 1.0f;
		Fixture.friction = 0.0f;
		Fixture.restitution = 0;

		Roofbody = m_world->CreateBody(&bodydef);
		Roofbody->CreateFixture(&Fixture);

		b2WeldJointDef weldJointDef;
		weldJointDef.Initialize(body, Roofbody, body->GetPosition());
		m_world->CreateJoint(&weldJointDef);
	}
}

b2Body* TiledBodyCreator::WhatObj(std::string sPath, std::list<b2Body*>* lb_list, b2Vec2 position, b2World* world, Layer* layer, Size _spsize, Size _objsize, INT16 _category, INT16 _maskbits, int setdata) {
	b2BodyDef bd;
	//Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/pickaxe.png");
	//Sprite* Pickaxe_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 40, 32));
	Sprite* Obj_Sprite = Sprite::create(sPath);
	Obj_Sprite->setAnchorPoint(Vec2(0, 0));
	bd.userData = Obj_Sprite;

	layer->addChild(Obj_Sprite, 3);
	bd.position = position;

	itemBody = world->CreateBody(&bd);
	itemBody->SetIsCh(false);

	itemBody->setNewobj(true);
	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = posi.x + _objsize.width;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + _objsize.width;
	vertices[1].y = posi.y + (height - _objsize.height);

	vertices[2].x = posi.x + (width - _objsize.height);
	vertices[2].y = posi.y + (height - _objsize.height);

	vertices[3].x = posi.x + (width - _objsize.height);
	vertices[3].y = posi.y + 0.0f;

	itemBody->SetHp(10);
	itemBody->SetType(b2_dynamicBody);

	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;
	fix->fixture.isSensor = false;
	fix->fixture.filter.categoryBits = _category;
	fix->fixture.filter.maskBits = _maskbits;

	SensorFixture = itemBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)setdata);

	lb_list->push_back(itemBody);

	return itemBody;
}


FixtureDef* TiledBodyCreator::createFixture(cocos2d::ValueMap object)
{
	int fixtureType = RECT_FIXTURE;
	for(auto propObj : object)
	{
		if(propObj.first == "points") {
			fixtureType = POLYGON_FIXTURE;
		} else if(propObj.first == "polylinePoints") {
			fixtureType = POLYLINE_FIXTURE;
		}
	}
	if(object["type"].asString() == "Circle") {
		fixtureType = CIRCLE_FIXTURE;
	}


	if(fixtureType == POLYGON_FIXTURE) {
		return createPolygon(object);
	} else if(fixtureType == POLYLINE_FIXTURE) {
		return createPolyline(object);
	} else if(fixtureType == CIRCLE_FIXTURE) {
		return createCircle(object);
	} else if(fixtureType == RECT_FIXTURE) {
		return createRect(object);
	}
}

FixtureDef* TiledBodyCreator::createPolygon(ValueMap object)
{
	ValueVector pointsVector = object["points"].asValueVector();
	auto position = Point(object["x"].asFloat() / PTMRATIO, object["y"].asFloat() / PTMRATIO);

	b2PolygonShape *polyshape = new b2PolygonShape();
	b2Vec2 vertices[b2_maxPolygonVertices];
	int vindex = 0;

	if(pointsVector.size() > b2_maxPolygonVertices) {
		CCLOG("Skipping TMX polygon at x=%d,y=%d for exceeding %d vertices", object["x"].asInt(), object["y"].asInt(), b2_maxPolygonVertices);
		return NULL;
	}

	auto fix = new FixtureDef();

	for(Value point : pointsVector) {
		vertices[vindex].x = (point.asValueMap()["x"].asFloat() / PTMRATIO + position.x);
        vertices[vindex].y = (-point.asValueMap()["y"].asFloat() / PTMRATIO + position.y);
		vindex++;
	}

	polyshape->Set(vertices, vindex);
	fix->fixture.shape = polyshape;

	return fix;
}


FixtureDef* TiledBodyCreator::createPolyline(ValueMap object)
{
	ValueVector pointsVector = object["polylinePoints"].asValueVector();
	auto position = Point(object["x"].asFloat() / PTMRATIO, object["y"].asFloat() / PTMRATIO);

	b2ChainShape *polylineshape = new b2ChainShape();
	
	int verticesCapacity=32;
	b2Vec2* vertices = (b2Vec2*)calloc(verticesCapacity, sizeof(b2Vec2));

	int vindex = 0;

	auto fix = new FixtureDef();

	for(Value point : pointsVector) {
		if(vindex>=verticesCapacity)
		{
			verticesCapacity+=32;
			vertices = (b2Vec2*)realloc(vertices, verticesCapacity*sizeof(b2Vec2));
		}
		vertices[vindex].x = (point.asValueMap()["x"].asFloat() / PTMRATIO + position.x);
        vertices[vindex].y = (-point.asValueMap()["y"].asFloat() / PTMRATIO + position.y);
		vindex++;
	}

	polylineshape->CreateChain(vertices, vindex);
	fix->fixture.shape = polylineshape;

	return fix;
}

FixtureDef* TiledBodyCreator::createCircle(ValueMap object)
{
	auto position = Point(object["x"].asFloat() / PTMRATIO, object["y"].asFloat() / PTMRATIO);
	float radius = object["width"].asFloat()/2 / PTMRATIO;

	b2CircleShape *circleshape = new b2CircleShape();
	circleshape->m_radius = radius;
	circleshape->m_p.Set(position.x + radius, position.y + radius);

	auto fix = new FixtureDef();
	fix->fixture.shape = circleshape;

	return fix;
}

FixtureDef* TiledBodyCreator::createRect(ValueMap object)
{
	auto position = Vec2(0,0);
	float width = object["width"].asFloat() / PTMRATIO;
	float height = object["height"].asFloat() / PTMRATIO;

	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	vertices[0].x = position.x + 0.0f;
	vertices[0].y = position.y + 0.0f;

	vertices[1].x = position.x + 0.0f;
	vertices[1].y = position.y + height;

	vertices[2].x = position.x + width;
	vertices[2].y = position.y + height;

	vertices[3].x = position.x + width;
	vertices[3].y = position.y + 0.0f;

	auto fix = new FixtureDef();
	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;

	return fix;
}