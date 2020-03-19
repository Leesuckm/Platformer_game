#include "Opossum.h"

Opossum::Opossum() {
	m_Monster = new Monster_Property;
	m_Recognize = new Monster_Property;
}

Opossum::~Opossum() {
	delete m_Monster;
	delete m_Recognize;
}

b2Body* Opossum::CreateMonster(b2World* world, Vec2 position, Layer* layer) {
	m_world = world;

	m_texture = Director::getInstance()->getTextureCache()->addImage("Images/Monsters/opossum.png");

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.1f);

	for (int i = 0; i < 4; i++) {
		animation->addSpriteFrameWithTexture(m_texture, Rect(i * 36, 0, 28, 36));
	}

	m_MonsterSprite = Sprite::createWithTexture(m_texture, Rect(0, 0, 36, 28));
	m_MonsterSprite->setAnchorPoint(Vec2(0.5, 0.3));
	layer->addChild(m_MonsterSprite, 3);

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	m_MonsterSprite->runAction(rep);

	//	b2BodyDef m_MonsterBodyDef;

	m_Monster->body_def.type = b2_dynamicBody;

	b2PolygonShape m_MonsterPolygon;
	m_MonsterPolygon.SetAsBox(0.7814f, 0.3907f);

	//b2CircleShape circle;
	//circle.m_radius = 0.3f;

	m_Monster->fixture_def.shape = &m_MonsterPolygon; // 여기에 추가된 Shape를 써서 기존 body에 또 fixture를 넣어야만 적용됨.
	m_Monster->fixture_def.density = 1.0f;
	m_Monster->fixture_def.friction = 1.0f;
	m_Monster->fixture_def.restitution = 0.0;

	m_Monster->fixture_def.filter.categoryBits = 0x0010;
	m_Monster->fixture_def.filter.maskBits = 0x000F;

	m_Monster->body_def.position.Set(position.x / 32, position.y / 32);
	m_Monster->body_def.userData = m_MonsterSprite;

	m_Monster->body = world->CreateBody(&m_Monster->body_def);
	m_Monster->body->CreateFixture(&m_Monster->fixture_def);

	m_Monster->body->SetIsCh(false);
	m_Monster->body->SetHp(8);
	m_Monster->body->SetFixedRotation(true);

	b2Body* leftLeg = Opossum::CreateBody(m_Monster->body->GetPosition() + b2Vec2(-0.5, -0.5), Size(15, 15), b2_dynamicBody, "Images/Camera.png", 1, layer);
	b2Body* RightLeg = Opossum::CreateBody(m_Monster->body->GetPosition() + b2Vec2(0.5, -0.5), Size(15, 15), b2_dynamicBody, "Images/Camera.png", 1, layer);

	b2Vec2 axis(0.0f, 1.0f);

	b2WheelJointDef jd1;
	b2WheelJointDef jd2;

	jd1.Initialize(m_Monster->body, leftLeg, leftLeg->GetPosition(), axis);
	jd1.motorSpeed = 1.0f;
	jd1.maxMotorTorque = 20.0f;
	jd1.enableMotor = true;
	jd1.frequencyHz = 4.0f;

	jd2.Initialize(m_Monster->body, RightLeg, RightLeg->GetPosition(), axis);
	jd2.motorSpeed = 1.0f;
	jd2.maxMotorTorque = 20.0f;
	jd2.enableMotor = true;
	jd2.frequencyHz = 4.0f;

	m_spring1 = (b2WheelJoint*)world->CreateJoint(&jd1);
	m_spring2 = (b2WheelJoint*)world->CreateJoint(&jd2);

	Opossum::RecognizeMonster(world, layer);

	return m_Monster->body;

	m_MoveState = MS_STOP;
}


void Opossum::RecognizeMonster(b2World* world, Layer* layer) {
	m_texture = Director::getInstance()->getTextureCache()->addImage("Images/Monsters/Monster_Recognize.png");

	m_RecognizeSprite = Sprite::createWithTexture(m_texture, Rect(0, 0, 150, 37));
	m_RecognizeSprite->setAnchorPoint(Vec2(0.5, 0.3));
	//m_RecognizeSprite->setScale(5.0f);
	layer->addChild(m_RecognizeSprite, 3);

	m_Recognize->body_def.type = b2_dynamicBody;

	b2CircleShape circle;
	circle.m_radius = 0.1f;
	m_Recognize->fixture_def.isSensor = true;
	m_Recognize->fixture_def.shape = &circle;
	m_Recognize->fixture_def.density = 0.1f;
	m_Recognize->fixture_def.friction = 1.0f;
	m_Recognize->fixture_def.restitution = 0.0;

	m_Recognize->fixture_def.filter.categoryBits = 0x0030;
	m_Recognize->fixture_def.filter.maskBits = 0x0002;

	m_Recognize->body_def.position.Set(m_Monster->body->GetPosition().x, m_Monster->body->GetPosition().y - 0.5);
	m_Recognize->body_def.userData = m_RecognizeSprite;

	m_Recognize->body = world->CreateBody(&m_Recognize->body_def);
	m_Recognize->body->CreateFixture(&m_Recognize->fixture_def);

	m_Recognize->body->SetHp(100);
	m_Recognize->body->SetIsCh(false);
	// weldhjoint로 붙일 body의 SetFixedRotation을 true로 해버리면 붙질 않는다. false로 해두어야한다.
	m_Recognize->body->SetFixedRotation(false);

	b2Fixture* SensorFixture;
	SensorFixture = m_Recognize->body->CreateFixture(&m_Recognize->fixture_def);
	SensorFixture->SetUserData((void*)8);

	b2WeldJointDef weldJointDef;
	weldJointDef.Initialize(m_Monster->body, m_Recognize->body, m_Monster->body->GetPosition());
	world->CreateJoint(&weldJointDef);
}

b2Body* Opossum::CreateBody(b2Vec2 point, Size size, b2BodyType bodytype, const char* spriteName, int type, Layer* layer) {
	
	b2BodyDef bodyDef;
	bodyDef.type = bodytype;
	bodyDef.position.Set(point.x, point.y);

	if (spriteName) {
		if (strcmp(spriteName, "test") == 0) {
		}

		else {
			Sprite* sprite = Sprite::create(spriteName);
			sprite->setPosition(Vec2(point.x * 32, point.y * 32));
			sprite->setVisible(false);
			this->addChild(sprite);

			bodyDef.userData = sprite;
		}
	}

	// 월드에 바디데프의 정보로 바디를 만든다.
	b2Body* body = m_world->CreateBody(&bodyDef);

	// 바디에 적용할 물리 속성용 바디의 모양을 만든다.
	b2FixtureDef fixtureDef;
	b2PolygonShape dynamicBox;
	b2CircleShape circle;

	if (type == 0) {
		dynamicBox.SetAsBox(size.width / 2 / 32, size.height / 2 / 32);
		fixtureDef.shape = &dynamicBox;
	}

	else {
		circle.m_radius = (size.width / 2) / 32;
		fixtureDef.shape = &circle;
	}

	// Define the dynamic body fixture.
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 0.0f;
	fixtureDef.filter.categoryBits = 0x0030;
	fixtureDef.filter.maskBits = 0x0002;

	body->CreateFixture(&fixtureDef);

	body->SetHp(100);

	return body;
}

Sprite* Opossum::AnimationState(_moveState state, Layer* layer) {
	int nSheetmx;
	Size size;
	std::string sPath;
	switch (state) {
	case MS_LEFT:
		nSheetmx = 6;
		size.width = 36;
		size.height = 28;
		sPath = "Images/Monsters/opossum.png";
		break;
	case MS_RIGHT:
		nSheetmx = 6;
		size.width = 36;
		size.height = 28;
		sPath = "Images/Monsters/opossum.png";
		break;
	case MS_STOP:
		nSheetmx = 6;
		size.width = 36;
		size.height = 28;
		sPath = "Images/Monsters/opossum.png";
		break;
	case MS_LEFTJUMP:
		nSheetmx = 6;
		size.width = 36;
		size.height = 26;
		sPath = "Images/Monsters/opossum.png";
		break;
	case MS_RIGHTJUMP:
		nSheetmx = 6;
		size.width = 36;
		size.height = 26;
		sPath = "Images/Monsters/opossum.png";
		break;
	case MS_PAIN:
		nSheetmx = 3;
		size.width = 36;
		size.height = 28;
		sPath = "Images/Monsters/opossum_pain.png";
		break;
	case MS_DEATH:
		nSheetmx = 6;
		size.width = 36;
		size.height = 28;
		sPath = "Images/Monsters/opossum.png";
		break;
	default:
		log("Monster state error");
		break;
	}

	auto texture = Director::getInstance()->getTextureCache()->addImage(sPath);

	auto animation = Animation::create();
	animation->setDelayPerUnit(0.1f);

	for (int i = 0; i < nSheetmx; i++) {
		animation->addSpriteFrameWithTexture(texture,
			Rect(i * size.width, 0, size.width, size.height));
	}

	m_MonsterSprite = Sprite::createWithTexture(texture, Rect(0, 0, size.width, size.height));
	m_MonsterSprite->setAnchorPoint(Vec2(0.5f, 0.3f));
	m_MonsterSprite->setScale(1.5f);
	layer->addChild(m_MonsterSprite, 3);

	auto animate = Animate::create(animation);
	auto rep = RepeatForever::create(animate);

	m_MonsterSprite->runAction(rep);

	return m_MonsterSprite;
}

void Opossum::MonsterPatrol(b2World* world, std::list<Monster> *Monsters, b2Body* Charactor, float movetime, Layer* layer, int foot) {

	Sprite* Recognize_sp = static_cast<Sprite*>(Charactor->GetUserData());

	std::list<Monster>::iterator iter;
	for (iter = Monsters->begin(); iter != Monsters->end();) {
		if (iter->obj_name == "Opossum") {
			// 몬스터 Hp가 0미만이라면 죽은것이때문에 list에서 제거한다.
			if (iter->body->GetHp() <= 0) {
				Monsters->erase(iter++);
				continue;
			}
			Sprite* spriteData = static_cast<Sprite*>(iter->recognize_body->GetUserData());
			Rect rect = spriteData->getBoundingBox();

			iter->dt += movetime;
			b2Vec2 vel = iter->body->GetLinearVelocity();
			float desiredVel = 0;
			// 캐릭터가 몬스터의 인식범위 내에 들어갈 시.
			if (iter->time <= iter->dt && rect.intersectsRect(Recognize_sp->getBoundingBox()) && iter->state != MS_DEATH) {
				//log("Monster_Recognize");
				b2Vec2 Charactor_position = Charactor->GetPosition();
				b2Vec2 Monster_position = iter->body->GetPosition();

				// 캐릭터가 몬스터의 위치에서 왼쪽에 위치할때.
				if (Charactor_position.x < Monster_position.x) {
					if(foot > 0){
						iter->state = MS_LEFTJUMP;
						iter->body->SetLinearVelocity(b2Vec2(-2.6, 12));
						Opossum::SpriteChange(iter, layer);
						iter->dt = 3;
						desiredVel = -10;
					}
					else {
						iter->state = MS_LEFT;
						Opossum::SpriteChange(iter, layer);
						iter->dt = 3;
						desiredVel = -10;
					}
				}
				else {
					if (foot > 0) {
						iter->state = MS_RIGHTJUMP;
						iter->body->SetLinearVelocity(b2Vec2(2.6, 12));
						Opossum::SpriteChange(iter, layer);
						iter->dt = 3;
						desiredVel = 10;
					}
					else {
						iter->state = MS_RIGHT;
						Opossum::SpriteChange(iter, layer);
						iter->dt = 0;
						desiredVel = 10;
					}
				}
			}

			else if (iter->time <= iter->dt && iter->state != MS_DEATH && foot > 0) {
				iter->time = rand() % (5 + 1 - 2) + 2;
				int p = rand() % 10;
				if (p >= 0 && p <= 2) {
					iter->state = MS_RIGHT;
					Opossum::SpriteChange(iter, layer);
					iter->dt = 0;
					desiredVel = 10;
				}
				if (p >= 3 && p <= 5) {
					iter->state = MS_LEFT;
					Opossum::SpriteChange(iter, layer);
					iter->dt = 0;
					desiredVel = -10;
				}
				if (p >= 6 && p <= 9) {
					iter->state = MS_STOP;
					Opossum::SpriteChange(iter, layer);
					iter->dt = 0;
					desiredVel = 0;
				}
			}
			float velChange = desiredVel - vel.x;
			float impulse = iter->body->GetMass() * velChange;
			b2Joint* joint = (b2Joint*)iter->body->GetJointList();
			
			b2WheelJoint* WeldJoint = static_cast<b2WheelJoint*>(iter->body->GetJointList()->joint);
			b2WheelJoint* RightWheel = static_cast<b2WheelJoint*>(iter->body->GetJointList()->joint->GetNext());
			b2WheelJoint* LeftWheel = static_cast<b2WheelJoint*>(iter->body->GetJointList()->joint->GetNext()->GetNext());

			switch (iter->state) {
			case MS_RIGHT:
				//iter->body->ApplyLinearImpulse(b2Vec2(0.1, 0), iter->body->GetWorldCenter(), true);
				//Opossum::SpriteChange(iter, layer);
				LeftWheel->SetMotorSpeed(-10.0f);
				RightWheel->SetMotorSpeed(-10.0f);
				break;
			case MS_LEFT:
				//Opossum::SpriteChange(iter, layer);
				LeftWheel->SetMotorSpeed(10.0f);
				RightWheel->SetMotorSpeed(10.0f);
				break;
			case MS_STOP:
				break;

			case MS_RIGHTJUMP:
				LeftWheel->SetMotorSpeed(-10.0f);
				RightWheel->SetMotorSpeed(-10.0f);
				break;
			case MS_LEFTJUMP:
				LeftWheel->SetMotorSpeed(10.0f);
				RightWheel->SetMotorSpeed(10.0f);
				break;
			}
			++iter;
		}
		else { ++iter; }
	}
}

void Opossum::SpriteChange(std::list<Monster>::iterator mob, Layer* layer) {
	Sprite* oldsprite = static_cast<Sprite*>(mob->body->GetUserData());
	mob->body->SetUserData(nullptr);
	layer->removeChild(oldsprite, true);

	Sprite* newSprite = Opossum::AnimationState(mob->state, layer);
	if (mob->state == MS_LEFT || mob->state == MS_LEFTJUMP) {
		newSprite->setFlippedX(false);
	}

	else if (mob->state == MS_RIGHT || mob->state == MS_RIGHTJUMP) {
		newSprite->setFlippedX(true);
	}
	mob->body->SetUserData(newSprite);
}