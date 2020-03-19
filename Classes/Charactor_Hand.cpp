#include "CharactorCreator.h"
#include "Charactor_Hand.h"

extern bool g_bViewing;

Charactor_Hand::Charactor_Hand() {
	m_HandState = TwoHand_Off;
}

Charactor_Hand::~Charactor_Hand() {

}

void Charactor_Hand::Holeinone_LeftHand(std::string sPath, b2Body* Charactorbody, _moveState state, Layer* layer, b2World* _world) {
	m_world = _world;
	m_Layer = layer;
	m_HandState = LeftHand_On;

	m_texture = Director::getInstance()->getTextureCache()->addImage(sPath);

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.1f);
	
	for (int i = 0; i < 4; i++) {
		animation->addSpriteFrameWithTexture(m_texture, Rect(i * 36, 0, 36, 32));
	}
	m_Pickup_Lefthand = Sprite::createWithTexture(m_texture, Rect(0, 0, 36, 32));
	m_Pickup_Lefthand->setAnchorPoint(Vec2(0.5, 0.3));
	if (g_bViewing) m_Pickup_Lefthand->setFlippedX(false);
	else if(!g_bViewing) m_Pickup_Lefthand->setFlippedX(true);
	layer->addChild(m_Pickup_Lefthand, 3);

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	m_Pickup_Lefthand->runAction(rep);
	m_Charactorbody = Charactorbody;
	m_Charactorbody->SetUserData(m_Pickup_Lefthand);

	//Charactor_Hand::CreateHandObj(sPath);
}

void Charactor_Hand::CreateHandObj(std::string sPath) {
	//월드 매개변수로 가져와서 무기바디 만들고 조인트로 캐릭터바디에 붙이기
	b2BodyDef HandObjBodyDef;

	HandObjBodyDef.type = b2_dynamicBody;
	HandObjBodyDef.position.Set(m_Charactorbody->GetPosition().x, m_Charactorbody->GetPosition().y);
	//HandObjBodyDef.userData = HandObjSprite;

	b2PolygonShape HandObjShape;
	HandObjShape.SetAsBox(0.1f, 0.1f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &HandObjShape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.1f;
	fixtureDef.restitution = 0.0;
	fixtureDef.isSensor = false;

	fixtureDef.filter.categoryBits = 0x0003;
	fixtureDef.filter.maskBits = 0x0010; // 몬스터만 충돌

	m_HandObjbody = m_world->CreateBody(&HandObjBodyDef);
	m_HandObjbody->CreateFixture(&fixtureDef);

	m_HandObjbody->SetIsCh(false);
	m_HandObjbody->SetFixedRotation(false);
	//-------------------------------------------------------------------------------------------------------------
	m_texture = Director::getInstance()->getTextureCache()->addImage("Images/Items/sword1.png");

	Sprite* ItemObjSprite = Sprite::createWithTexture(m_texture, Rect(0, 0, 26, 26));
	m_Layer->addChild(ItemObjSprite, 3);

	b2BodyDef ItemObjBodyDef;
	ItemObjBodyDef.type = b2_dynamicBody;
	ItemObjBodyDef.position.Set(m_HandObjbody->GetPosition().x, m_HandObjbody->GetPosition().y);
	ItemObjBodyDef.userData = ItemObjSprite;

	b2PolygonShape ItemObjShape;
	//ItemObjShape.SetAsBox(0.1f, 1.0f);

	auto posi = Vec2(0, 0);
	b2Vec2 vertices[4];
	vertices[0].x = posi.x + 0.0f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.0f;
	vertices[1].y = posi.y + 1;

	vertices[2].x = posi.x + (1 - 0.8);
	vertices[2].y = posi.y + 1;

	vertices[3].x = posi.x + (1 - 0.8);
	vertices[3].y = posi.y + 0.0f;

	ItemObjShape.Set(vertices, 4);
	b2FixtureDef ItemfixtureDef;
	ItemfixtureDef.shape = &ItemObjShape;
	ItemfixtureDef.density = 0.1f;
	ItemfixtureDef.friction = 0.1f;
	ItemfixtureDef.restitution = 0.0;
	ItemfixtureDef.isSensor = false;

	ItemfixtureDef.filter.categoryBits = 0x0003;
	ItemfixtureDef.filter.maskBits = 0x0010; // 몬스터만 충돌

	m_ItemObjbody = m_world->CreateBody(&ItemObjBodyDef);
	m_ItemObjbody->CreateFixture(&ItemfixtureDef);

	m_ItemObjbody->SetHp(1000);
	m_ItemObjbody->SetIsCh(false);

	b2Fixture* SensorFixture;
	SensorFixture = m_ItemObjbody->CreateFixture(&ItemfixtureDef);
	SensorFixture->SetUserData((void*)2);

	b2WeldJointDef weldJointDef;
	weldJointDef.Initialize(m_HandObjbody, m_ItemObjbody, m_HandObjbody->GetPosition() - b2Vec2(1.5, 1.3));
	m_world->CreateJoint(&weldJointDef);

	b2WheelJointDef Wjd;
	Wjd.Initialize(m_Charactorbody, m_HandObjbody, m_HandObjbody->GetPosition(), b2Vec2(0.0f, 1.0f));
	Wjd.motorSpeed = -1.0f;
	Wjd.maxMotorTorque = 5.0f;
	Wjd.enableMotor = true;
	Wjd.frequencyHz = 50.0f;

	b2WheelJoint* Wheel = (b2WheelJoint*)m_world->CreateJoint(&Wjd);
}

Sprite* Charactor_Hand::AnimationState(std::string Obj_name, _HandState state, Layer* layer, bool viewing) {
	int nSheetmx;
	Size size;
	std::string sPath;
	if (Obj_name == "Pickaxe") {
		switch (state) {
		case LeftHand_On:
			nSheetmx = 6;
			sPath = "Images/Items/fox_run_pickaxe.png";
			size.width = 36;
			size.height = 32;
			break;
		case RightHand_On:
			nSheetmx = 6;
			sPath = "Images/Items/fox_run_pickaxe.png";
			size.width = 36;
			size.height = 32;
			break;
		case Hand_On:
			nSheetmx = 4;
			sPath = "Images/Items/fox_idle_pickaxe.png";
			size.width = 36;
			size.height = 32;
			break;
		case JumpHand_On:
			nSheetmx = 1;
			sPath = "Images/Items/fox_jump_pickaxe.png";
			size.width = 36;
			size.height = 32;
			break;
		case JumpHand_Down:
			nSheetmx = 1;
			sPath = "Images/Items/fox_jump_pickaxe2.png";
			size.width = 36;
			size.height = 32;
			break;
		case ATTACK:
			nSheetmx = 3;
			sPath = "Images/Items/fox_attack_pickaxe.png";
			size.width = 39;
			size.height = 32;
			break;
		case UP_ATTACK:
			nSheetmx = 3;
			sPath = "Images/Items/fox_up_attack_pickaxe.png";
			size.width = 33;
			size.height = 32;
			break;
		case UNDER_ATTACK:
			nSheetmx = 3;
			sPath = "Images/Items/fox_under_attack_pickaxe.png";
			size.width = 39;
			size.height = 32;
			break;
		case THROW_BOMB:
			nSheetmx = 1;
			sPath = "Images/Items/fox_throw_bomb.png";
			size.width = 40;
			size.height = 32;
			break;
		default:
			log("state error");
			break;
		}
	}

	else if (Obj_name == "Sword") {
		switch (state) {
		case LeftHand_On:
			nSheetmx = 6;
			sPath = "Images/Items/fox_run_sword.png";
			size.width = 50;
			size.height = 32;
			break;
		case RightHand_On:
			nSheetmx = 6;
			sPath = "Images/Items/fox_run_sword.png";
			size.width = 50;
			size.height = 32;
			break;
		case Hand_On:
			nSheetmx = 4;
			sPath = "Images/Items/fox_idle_sword.png";
			size.width = 45;
			size.height = 32;
			break;
		case JumpHand_On:
			nSheetmx = 1;
			sPath = "Images/Items/fox_jump_sword1.png";
			size.width = 33;
			size.height = 32;
			break;
		case JumpHand_Down:
			nSheetmx = 1;
			sPath = "Images/Items/fox_jump_sword2.png";
			size.width = 33;
			size.height = 32;
			break;
		case ATTACK:
			nSheetmx = 5;
			sPath = "Images/Items/fox_attack_sword.png";
			size.width = 55;
			size.height = 60;
			break;
		default:
			log("state error");
			break;
		}
	}

	else if (Obj_name == "Gun") {
	switch (state) {
	case LeftHand_On:
		nSheetmx = 6;
		sPath = "Images/Items/fox_run_gun.png";
		size.width = 33;
		size.height = 32;
		break;
	case RightHand_On:
		nSheetmx = 6;
		sPath = "Images/Items/fox_run_gun.png";
		size.width = 33;
		size.height = 32;
		break;
	case Hand_On:
		nSheetmx = 4;
		sPath = "Images/Items/fox_idle_gun.png";
		size.width = 33;
		size.height = 32;
		break;
	case JumpHand_On:
		nSheetmx = 1;
		sPath = "Images/Items/fox_jump_gun1.png";
		size.width = 33;
		size.height = 32;
		break;
	case JumpHand_Down:
		nSheetmx = 1;
		sPath = "Images/Items/fox_jump_gun2.png";
		size.width = 33;
		size.height = 32;
		break;
	case ATTACK:
		nSheetmx = 4;
		sPath = "Images/Items/fox_attack_gun.png";
		size.width = 40;
		size.height = 30;
		break;
	default:
		log("state error");
		break;
	}
	}

	auto texture = Director::getInstance()->getTextureCache()->addImage(sPath);

	auto animation = Animation::create();
	animation->setDelayPerUnit(0.1f);

	for (int i = 0; i < nSheetmx; i++) {
		animation->addSpriteFrameWithTexture(texture,
			Rect(i * size.width, 0, size.width, size.height));
	}

	m_Pickup_Lefthand = Sprite::createWithTexture(texture, Rect(0, 0, size.width, size.height));
	m_Pickup_Lefthand->setPosition(Vec2(1, 1));
	m_Pickup_Lefthand->setAnchorPoint(Vec2(0.5f, 0.3f));
	if (state == ATTACK && viewing)
		m_Pickup_Lefthand->setAnchorPoint(Vec2(0.3f, 0.3f));
	else if (state == ATTACK && !viewing)
		m_Pickup_Lefthand->setAnchorPoint(Vec2(0.7f, 0.3f));

	if (g_bViewing)
		m_Pickup_Lefthand->setFlippedX(false);
	else if (!g_bViewing)
		m_Pickup_Lefthand->setFlippedX(true);
	layer->addChild(m_Pickup_Lefthand, 3);

	auto animate = Animate::create(animation);
	auto rep = RepeatForever::create(animate);

	m_Pickup_Lefthand->runAction(rep);

	m_Charactorbody->SetUserData(m_Pickup_Lefthand);

	return m_Pickup_Lefthand;
}