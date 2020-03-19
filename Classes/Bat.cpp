#include "Bat.h"
#include <random>

Bat::Bat() {
	m_Monster = new Monster_Property;
	m_Recognize = new Monster_Property;
}
Bat::~Bat() {
	delete m_Monster;
	delete m_Recognize;
}

b2Body* Bat::CreateMonster(b2World* world, Vec2 position, Layer* layer) {
	m_texture = Director::getInstance()->getTextureCache()->addImage("Images/Monsters/bat_idle.png");

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.1f);

	for (int i = 0; i < 4; i++) {
		animation->addSpriteFrameWithTexture(m_texture, Rect(i * 64, 0, 64, 64));
	}

	Sprite* m_MonsterSprite = Sprite::createWithTexture(m_texture, Rect(0, 0, 64, 64));
	m_MonsterSprite->setAnchorPoint(Vec2(0.5, 0.3));
	m_MonsterSprite->setScale(0.5f);
	layer->addChild(m_MonsterSprite, 3);

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	m_MonsterSprite->runAction(rep);

	//	b2BodyDef m_MonsterBodyDef;

	m_Monster->body_def.type = b2_dynamicBody;

	//b2PolygonShape m_MonsterPolygon;
	//m_MonsterPolygon.SetAsBox(0.3f, 0.3f);

	b2CircleShape circle;
	circle.m_radius = 0.3f;

	m_Monster->fixture_def.shape = &circle; // 여기에 추가된 Shape를 써서 기존 body에 또 fixture를 넣어야만 적용됨.
	m_Monster->fixture_def.density = 1.0f;
	m_Monster->fixture_def.friction = 3.0f;
	m_Monster->fixture_def.restitution = 0.0;

	m_Monster->fixture_def.filter.categoryBits = 0x0010;
	m_Monster->fixture_def.filter.maskBits = 0x000F;

	m_Monster->body_def.position.Set(position.x / 32, position.y / 32);
	m_Monster->body_def.userData = m_MonsterSprite;

	m_Monster->body = world->CreateBody(&m_Monster->body_def);
	m_Monster->body->CreateFixture(&m_Monster->fixture_def);

	m_Monster->body->SetIsCh(false);
	m_Monster->body->SetHp(7);
	m_Monster->body->SetFixedRotation(true);
	m_Monster->body->SetGravityScale(-0.1);

	Bat::RecognizeMonster(world, layer);

	return m_Monster->body;

	m_MoveState = MS_STOP;
}

void Bat::RecognizeMonster(b2World* world, Layer* layer) {
	m_texture = Director::getInstance()->getTextureCache()->addImage("Images/Monsters/Monster_Recognize.png");

	Sprite* m_RecognizeSprite = Sprite::createWithTexture(m_texture, Rect(0, 0, 150, 37));
	m_RecognizeSprite->setAnchorPoint(Vec2(0.5, 1));
	m_RecognizeSprite->setScaleX(2.0f);
	m_RecognizeSprite->setScaleY(5.0f);
	layer->addChild(m_RecognizeSprite, 3);

	m_Recognize->body_def.type = b2_dynamicBody;

	b2CircleShape circle;
	circle.m_radius = 0.2f;
	m_Recognize->fixture_def.isSensor = false;
	m_Recognize->fixture_def.shape = &circle;
	m_Recognize->fixture_def.density = 0.1f;
	m_Recognize->fixture_def.friction = 0.0f;
	m_Recognize->fixture_def.restitution = 0.0;

	m_Recognize->fixture_def.filter.categoryBits = 0x0030;
	m_Recognize->fixture_def.filter.maskBits = 0x0002;

	m_Recognize->body_def.position.Set(m_Monster->body->GetPosition().x, m_Monster->body->GetPosition().y);
	m_Recognize->body_def.userData = m_RecognizeSprite;

	m_Recognize->body = world->CreateBody(&m_Recognize->body_def);
	m_Recognize->body->CreateFixture(&m_Recognize->fixture_def);

	m_Recognize->body->SetHp(10);
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

// 몬스터 body 각자의 움직임을 서로 다르게, 그리고 랜덤하게 움직이기 위해선
// 서로 다르게 움직일 body 상태에 따른 Sprite 변화 및 움직임을 위한 state,
// 어느시간동안 하나의 움직임을 취할건지 time이 필요하다.
void Bat::MonsterPatrol(b2World* world, std::list<Monster> *Monsters, b2Body* Charactor, float movetime, Layer* layer, int foot) {

	//Monster* monster = new Monster(nullptr, nullptr, nullptr, MS_STOP, 0);
	Sprite* Recognize_sp = static_cast<Sprite*>(Charactor->GetUserData());

	std::list<Monster>::iterator iter;
	for (iter = Monsters->begin(); iter != Monsters->end();) {
		if (iter->obj_name == "Bat") {
			// 몬스터 Hp가 0미만이라면 죽은것이때문에 list에서 제거한다.
			if (iter->body->GetHp() <= 0) {
				Monsters->erase(iter++);
				continue;
			}
			Sprite* spriteData = static_cast<Sprite*>(iter->recognize_body->GetUserData());
			Rect rect = spriteData->getBoundingBox();

			iter->dt += movetime;
			b2Vec2 vel = iter->body->GetLinearVelocity();
			b2Vec2 distance_value = iter->before_position - iter->body->GetPosition();
			float x, y;
			x = distance_value.x;
			y = distance_value.y;
		//	if ((m_Delay_time -= movetime) > 0) {
		//		m_Delay_time = 1;
				m_Between_Ch_Bat = (Charactor->GetPosition() - iter->body->GetPosition()) + distance_value;
				iter->before_position = iter->body->GetPosition();
		//		m_Charactor_before_position = Charactor->GetPosition();
//}
			//else {
		//		m_Between_Ch_Bat = m_Charactor_before_position - iter->body->GetPosition();
		//	}

			// 캐릭터가 몬스터의 인식범위 내에 들어갈 시.
			if (iter->time <= iter->dt && rect.intersectsRect(Recognize_sp->getBoundingBox()) && iter->state != MS_DEATH) {
				iter->time = rand() % (5 + 1 - 2) + 2;


				//log("Monster_Recognize");
				b2Vec2 Charactor_position = Charactor->GetPosition();
				b2Vec2 Monster_position = iter->body->GetPosition();

				// 캐릭터가 몬스터의 위치에서 왼쪽에 위치할때.
				if (Charactor_position.x < Monster_position.x) {
					iter->state = MS_LEFTJUMP;
					Bat::SpriteChange(iter, layer);
					iter->dt = 0;
				}
				else {
					iter->state = MS_RIGHTJUMP;
					Bat::SpriteChange(iter, layer);
					iter->dt = 0;
				}
			}

			else if (iter->time <= iter->dt && iter->state != MS_DEATH) {
				iter->state = MS_JUMPUP;
				Bat::SpriteChange(iter, layer);
				iter->dt = 0;

				if (!iter->body->IsAwake()) {
					iter->state = MS_STOP;
					Bat::SpriteChange(iter, layer);
					iter->dt = 0;
				}
			}

			if (m_Between_Ch_Bat.x > 0 && m_Between_Ch_Bat.x < 1.5) { m_Between_Ch_Bat.x = 1.5f; }
			if (m_Between_Ch_Bat.x < 0 && m_Between_Ch_Bat.x > -1.5) { m_Between_Ch_Bat.x = -1.5f; }
			if (m_Between_Ch_Bat.y > 0 && m_Between_Ch_Bat.y < 1.5) { m_Between_Ch_Bat.y = 1.5f; }
			if (m_Between_Ch_Bat.y < 0 && m_Between_Ch_Bat.y > -1.5) { m_Between_Ch_Bat.y = -1.5f; }
			

			switch (iter->state) {
			case MS_RIGHT:
				iter->body->ApplyLinearImpulse(b2Vec2(0.15, 0), iter->body->GetWorldCenter(), true);
				break;
			case MS_LEFT:
				iter->body->ApplyLinearImpulse(b2Vec2(-0.15, 0), iter->body->GetWorldCenter(), true);
				break;
			case MS_STOP:
				break;
			case MS_JUMPUP:
				break;
			case MS_RIGHTJUMP:
				if ((iter->jump_CT -= movetime) <= 0) {
					iter->jump_CT = 0.5f;
					iter->body->SetLinearVelocity(b2Vec2(m_Between_Ch_Bat.x / 2, m_Between_Ch_Bat.y / 2));
				}
				break;
			case MS_LEFTJUMP:
				if ((iter->jump_CT -= movetime) <= 0) {
					iter->jump_CT = 0.5f;
					iter->body->SetLinearVelocity(b2Vec2(m_Between_Ch_Bat.x / 2, m_Between_Ch_Bat.y / 2));
				}
				break;
			}
			++iter;
		}
		else { ++iter; }
	}
}

Sprite* Bat::AnimationState(_moveState state, Layer* layer) {
	int nSheetmx;
	Size size;
	std::string sPath;
	switch (state) {
	case MS_LEFT:
		nSheetmx = 5;
		size.width = 16;
		size.height = 16;
		sPath = "Images/Monsters/bat_fly.png";
		break;
	case MS_RIGHT:
		nSheetmx = 5;
		size.width = 16;
		size.height = 16;
		sPath = "Images/Monsters/bat_fly.png";
		break;
	case MS_STOP:
		nSheetmx = 1;
		size.width = 16;
		size.height = 16;
		sPath = "Images/Monsters/bat_idle.png";
		break;
	case MS_PAIN:
		nSheetmx = 3;
		size.width = 16;
		size.height = 16;
		sPath = "Images/Monsters/bat_pain.png";
		break;
	case MS_DEATH:
		nSheetmx = 6;
		size.width = 16;
		size.height = 16;
		sPath = "Images/Monsters/death.png";
		break;
	case MS_JUMPUP:
		nSheetmx = 5;
		size.width = 16;
		size.height = 16;
		sPath = "Images/Monsters/bat_fly.png";
		break;
	case MS_RIGHTJUMP:
		nSheetmx = 5;
		size.width = 16;
		size.height = 16;
		sPath = "Images/Monsters/bat_fly.png";
		break;
	case MS_LEFTJUMP:
		nSheetmx = 5;
		size.width = 16;
		size.height = 16;
		sPath = "Images/Monsters/bat_fly.png";
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
	m_MonsterSprite->setScale(2.0f);
	layer->addChild(m_MonsterSprite, 3);

	auto animate = Animate::create(animation);
	auto rep = RepeatForever::create(animate);

	m_MonsterSprite->runAction(rep);

	return m_MonsterSprite;
}

void Bat::SpriteChange(std::list<Monster>::iterator mob, Layer* layer) {
	Sprite* oldsprite = static_cast<Sprite*>(mob->body->GetUserData());
	mob->body->SetUserData(nullptr);
	layer->removeChild(oldsprite, true);

	Sprite* newSprite = Bat::AnimationState(mob->state, layer);
	if (mob->state == MS_LEFT) {
		newSprite->setFlippedX(true);
	}

	else if (mob->state == MS_RIGHT) {
		newSprite->setFlippedX(false);
	}
	mob->body->SetUserData(newSprite);
}