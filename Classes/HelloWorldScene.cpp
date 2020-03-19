#include "HelloWorldScene.h"
#include "Charactor_Hand.h"
#include "CharactorCreator.h"
#include "MonsterCreator.h"
#include "Opossum.h"
#include "Frog.h"
#include "Bat.h"
#include "B2DebugDrawLayer.h"
#include "Explosion.h"
#include "EffectAnimation.h"

USING_NS_CC;

#define PTMRATIO 32
#define DIRECTPOW 30
#define JUMPPOW 180
#define ATTACKDELAY 5
#define BOMBDAMAGE 10
#define SWORDDAMAGE 3
#define GUNDAMAGE 2
#define DEGTORAD 3.14 / 180

CMyContactListener gContactInstance;

float currentRayAngle = 0;


Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
	wlayer = DLayer::create();
	wlayer->setContentSize(Size(1920, 1024));
	this->addChild(wlayer, 2);

	m_clayer = LayerColor::create(Color4B(255, 255, 255, 0));
	wlayer->addChild(m_clayer, 3);
	
	m_fDelayTime = 0;
	m_ftime = 0;

	srand(static_cast<unsigned int>(time(NULL)));
	
	if (this->createBox2dWorld(true)) {
		this->schedule(schedule_selector(HelloWorld::tick));
	}

    return true;
}

HelloWorld::~HelloWorld() {
	//월드를 C++의 new로 생성했기때문에 여기서 지워줌
	delete _world;
	_world = nullptr;
}

void HelloWorld::onEnter() {
	Scene::onEnter();

	// 싱글터치모드로 터치리스너 등록
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);

	m_kListener = EventListenerKeyboard::create();
	m_kListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
	m_kListener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_kListener, this);

	//Director::sharedDirector()->getEventDispatcher()->removeEventListenersForTarget(m_CameraSprite);
}

void HelloWorld::onExit() {
	Scene::onExit();
}
/*
void DLayer::onEnter() {
	Layer::onEnter();

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(DLayer::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(DLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(DLayer::onTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
void DLayer::onExit() {
	Layer::onExit();
}

bool DLayer::onTouchBegan(Touch* touch, Event* event) {

	Vec2 touchPoint = touch->getLocation();
	Vec2 touchPoint2 = Node::convertToNodeSpace(touchPoint);
	log("nodeSpace.. %f, %f", touchPoint2.x, touchPoint2.y);
	this->schedule(schedule_selector(HelloWorld::TouchHold), 0.5f);

	return true;
}
void DLayer::onTouchMoved(Touch* touch, Event* event) {

}
void DLayer::onTouchEnded(Touch* touch, Event* event) {
	
}*/


int n = 0;
int nb = 0;
void HelloWorld::tick(float dt) {
	// 물리적 위치를 이용해서 그래픽 위치를 갱신한다.

	// velocityIterations : 바디들을 정상적으로 이동시키기 위해서 필요한 충돌들을 반복적으로 계산
	// positionIterations : 조인트 분리와, 겹침 현상을 줄이기 위해서 바디의 위치를 반복적으로 적용
	// 값이 클수록 정확한 연산이 가능하지만 성능이 떨어진다.

	// 메뉴얼상의 권장값
	int velocityIterations = 8;
	int positionIterations = 3;
		
	b2Vec2 vel = Player->getCharactorBody()->GetLinearVelocity();
	float desiredVel = 0;

	if (g_isLeft == true) desiredVel = -5;
	if (g_isStop == true) desiredVel = 0;
	if (g_isUp == true) desiredVel = 5;
	if (g_isRight == true) desiredVel = 5;
	if (g_isLeft && g_isRight) desiredVel = 0;
	if (g_isLeft && g_isStop) desiredVel = 0;
	if (g_isRight && g_isStop) desiredVel = 0;
	//if (g_isThrow && g_isLeft || g_isRight) desiredVel = 0;

	m_fLadderCooltime -= dt;
	if (m_fLadderCooltime < -10) m_fLadderCooltime = 0;

	// 공격 쿨타임 1.5초
	if ((m_fAttackCooltime -= dt) <= 0) {
		//m_fAttackCooltime = 2;
		g_isAttackOn = true;
		g_ATTACK = false;
	}

	// 공격 animation 1회 출력후 멈춘뒤 다음 Animation 출력
	if (!g_isAttackOn && (m_fAttackDelaytime -= dt) <= 0) {
	//	m_fAttackDelaytime = 0.5;
		if (m_bTakePickaxe)
			HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
		else if (m_bTakeSword)
			HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
		else if (m_bTakeGun)
			HelloWorld::KeyAction(MS_STOP, Hand_On, "Gun");
		else
			HelloWorld::KeyAction(MS_STOP, Hand_On, "");
	}

	if (g_ATTACK) {
		HelloWorld::AttackMonster(dt);
	}

	power_enegy(dt);
	m_fcleantime += dt;

	// 총알이 월드에 추가됬을 때
	if (Bullets.size() > 0) {
		std::list<Bullet>::iterator biter;
		//std::list<Monster>::iterator miter;
		for (biter = Bullets.begin(); biter != Bullets.end();) {
			b2Vec2 pos = biter->body->GetPosition();
			MyQueryCallback bulletCallback;
			biter->aabb.lowerBound = pos - b2Vec2(0.1, 0.1);
			biter->aabb.upperBound = pos + b2Vec2(0.1, 0.1);
			m_aabbb = biter->aabb;
			_world->QueryAABB(&bulletCallback, biter->aabb);

			std::sort(bulletCallback.foundBodies.begin(), bulletCallback.foundBodies.end());
			bulletCallback.foundBodies.erase(std::unique(bulletCallback.foundBodies.begin(), bulletCallback.foundBodies.end()), bulletCallback.foundBodies.end());
			// 총알의 AABB경계에 다른 object가 접촉되었을 시 1개 이상인 이유는 총알의 body가 항상 aabb영역 안에 포함되기 때문
			if (bulletCallback.foundBodies.size() > 1) {

				for (int i = 0; i < bulletCallback.foundBodies.size(); i++) {
					b2Body* bulletBody = bulletCallback.foundBodies[i];
					b2BodyType bodytype = bulletBody->GetType();
					if (bodytype != b2_staticBody && bodytype != b2_dynamicBody && bodytype != b2_kinematicBody) {
						log("실체가없는 body");
						continue;
					}
					INT16 category = bulletBody->GetFixtureList()->GetFilterData().categoryBits;
					
					if (category == 0x0002) {
						//m_DestroyQ.push(bulletBody);
						m_bBulletcrash = true;
						log("block");
						//	break;
					}
					else if (category == 0x0010 && nb % 2 == 0) {
						m_bBulletcrash = true;
						int num = bulletBody->GetHp();
						num -= GUNDAMAGE;
						bulletBody->SetHp(num);
						log("monster hp = %d",num);
					}
				}
				nb++;
				if (nb > 10) 
					nb = 0;
			}

			if (m_bBulletcrash) {
				m_bBulletcrash = false;
				Sprite* bulletsp = static_cast<Sprite*>(biter->body->GetUserData());
				bulletsp = GunEffect->CreateEffect(bulletsp->getPosition(), 0.5f, wlayer);
				if (!g_bViewing) bulletsp->setFlippedX(true);
				m_DestroyQ.push(biter->body);
				biter = Bullets.erase(biter);
			}
			else 
				++biter;
		}
	}

	if (GunEffect->getGunEffects()->size() > 0) {
		std::list<Effect>::iterator iter;
		for (iter = GunEffect->getGunEffects()->begin(); iter != GunEffect->getGunEffects()->end();) {
			if ((iter->animation_time -= dt) <= 0) {
				wlayer->removeChild(iter->EffectSprite);
				iter = GunEffect->getGunEffects()->erase(iter);
			}
			else ++iter;
		}
	}

	// 폭탄이 월드에 추가됬을 때
	if (m_Explosion->getBombs_size() > 0) {
		Bombs = m_Explosion->getBombs();
		std::list<Bomb>::iterator iter;
		for (iter = Bombs->begin(); iter != Bombs->end(); ++iter) {
			b2Vec2 pos = iter->body->GetPosition();
			iter->aabb.lowerBound = pos - b2Vec2(1,1);
			iter->aabb.upperBound = pos + b2Vec2(2, 2);
			m_aabb = iter->aabb;
			_world->QueryAABB(&queryCallback, iter->aabb);
		}

		// 폭발
		// 바로 터지지 않고 body의 설정시간 후 터진다.
		if (m_Explosion->burst(_world, dt, wlayer)) {
			// 폭탄이 터지면서 사라질때 한 프레임에서의 쿼리영역만 처리하기 위해 중복body삭제
			std::sort(queryCallback.foundBodies.begin(), queryCallback.foundBodies.end());
			queryCallback.foundBodies.erase(std::unique(queryCallback.foundBodies.begin(), queryCallback.foundBodies.end()), queryCallback.foundBodies.end());
			for (int i = 0; i < queryCallback.foundBodies.size(); i++) {
				b2Vec2 pos = queryCallback.foundBodies[i]->GetPosition();
				b2Body* body = queryCallback.foundBodies[i];
				//b2Fixture* fix = body->GetFixtureList();
				//UINT16 categori = fix->GetFilterData().categoryBits;

				

				int obj_hp = body->GetHp();
				if (obj_hp > 0) {
					obj_hp = obj_hp - BOMBDAMAGE;
					body->SetHp(obj_hp);
				}

//				log("pos.x %f, pos.y %f %d", pos.x, pos.y, i);
			}
		}
	
	}

	if (m_fcleantime > 1) {
		m_fcleantime = 0;
		if (m_Explosion->getChips_size() != 0) {
			m_Explosion->cleanChips(_world);
		}
	}

	if (gNumFootContacts < 1 && !g_isUp) {
	//	Player->setMoveState(MS_STOP);
		//log("No Jump");
		// 캐릭터의 현재 y축을 이전 프레임의 y축과 비교하여 Sprite를 변경해준다.
		if (m_Player_before_position.y < Player->getCharactorBody()->GetPosition().y) {
			m_Player_before_position = Player->getCharactorBody()->GetPosition();
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "Gun");
			else
				HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "");
		}
		else if (m_Player_before_position.y > Player->getCharactorBody()->GetPosition().y) {
			m_Player_before_position = Player->getCharactorBody()->GetPosition();
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_JUMPDOWN, JumpHand_Down, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(MS_JUMPDOWN, JumpHand_Down, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(MS_JUMPDOWN, JumpHand_Down, "Gun");
			else
				HelloWorld::KeyAction(MS_JUMPDOWN, JumpHand_Down, "");
		}

		m_bCan_Move = false;
		m_bDo_Jump = true;
	}
	else if (gNumFootContacts > 0) {
		if (m_bDo_Jump) {
			m_bDo_Jump = false;
			if (!g_isLeft && !g_isRight) m_before_state = MS_STOP;
			if (m_bTakePickaxe || m_bTakeSword || m_bTakeGun && !g_isLeft && !g_isRight) m_before_handstate = Hand_On;
			if (m_bTakePickaxe || m_bTakeSword || m_bTakeGun && g_isLeft) m_before_handstate = LeftHand_On;
			if (m_bTakePickaxe || m_bTakeSword || m_bTakeGun && g_isRight) m_before_handstate = RightHand_On;
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(m_before_state, m_before_handstate, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(m_before_state, m_before_handstate, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(m_before_state, m_before_handstate, "Gun");
			else
				HelloWorld::KeyAction(m_before_state, m_before_handstate, "");
		}
		if (g_isLeft && g_isJump) Player->setMoveState(MS_LEFTJUMP);
		if (g_isRight && g_isJump) Player->setMoveState(MS_RIGHTJUMP);
		if (g_isJump && g_isLeft == false && g_isRight == false) Player->setMoveState(MS_JUMP);
		m_bCan_Move = true;
		//log("Yes Jump");
		switch (Player->getMoveState()) {
		case MS_RIGHTJUMP:
			//if (nJumpStep > 0) {
			Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse, m_fImpulse), Player->getCharactorBody()->GetWorldCenter(), true);
			//	m_nJumpStep--;
			log("RJ");
			//}
			break;
		case MS_LEFTJUMP:
			//if (nJumpStep > 0) {
			Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse, m_fImpulse), Player->getCharactorBody()->GetWorldCenter(), true);
			//	m_nJumpStep--;
			log("LJ");
			//	}
			break;
		case MS_JUMP:
			//if (nJumpStep > 0) {
			Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(0, m_fImpulse), Player->getCharactorBody()->GetWorldCenter(), true);
			//Player->getCharactorBody()->ApplyForce(b2Vec2(0, JUMPPOW), Player->getCharactorBody()->GetWorldCenter(), true);
			//body->ApplyForceToCenter(b2Vec2(500,500), true);
			//body->ApplyTorque(45.0f, true);
		//	m_nJumpStep--;
			log("IJ");
			//}
			break;
		}
	}

	if (m_EnableLadder.size() > 0) {
		std::list<b2Body*>::iterator iter;
		for (iter = m_EnableLadder.begin(); iter != m_EnableLadder.end();) {
			b2Body* Ladderbody = *iter;
			Sprite* LadderSp = static_cast<Sprite*>(Ladderbody->GetUserData());
			Sprite* CharactorSp = static_cast<Sprite*>(Player->getCharactorBody()->GetUserData());
			Rect Ladder_rect = CharactorSp->getBoundingBox();

			if (Ladder_rect.intersectsRect(LadderSp->getBoundingBox())) {
				m_bCan_UpDown = true;
				b2Vec2 velocity = Player->getCharactorBody()->GetLinearVelocity();
				log("velocity %f %f on!", velocity.x, velocity.y);
				++iter;
				break;
			}
			else {
				m_bCan_UpDown = false;
				++iter;
			}
		}
	}

	if (gNumObstacleContacts < 1) {
		//log("Obstacle Contacts is No");
	}
	else if (gNumObstacleContacts > 0 || gNumpickaxeContacts > 0) {
		if (gNumRightContacts > 0) { // Can_Broken는 어디까지나 부술 수 있는 영역을 다루기 때문에 다른 오브젝트와 접촉시 무시되야하므로 장애물과 캐릭터의 접촉시에만 동작하도록
			log("Obstacle Contacts is Yes");
			m_fDelayTime = 1; // 장애물 충돌 시 딜레이 1초동안 동작을 할 수 없다.
			// 장애물 충돌시 튕겨지는 힘.
			if (g_bViewing)	Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			else if (!g_bViewing) Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);

			if (n % 2 == 0) {
				gHeartCount--;
				if (gHeartCount <= 0) GameOver(this);
				Sprite* sp = Player->getHeartSprite()->at(gHeartCount);
				m_clayer->removeChild(sp);
				Player->getHeartSprite()->pop_back();
			}
			n++;
			if (n > 10) n = 0;
		}
	}
	// 충돌 후 딜레이
	if (m_fDelayTime > (m_ftime += dt)) {
		g_isLeft = false;
		g_isRight = false;
		g_isStop = false;
		m_fDelayTime = 0;
		m_ftime = 0;
	}
	if (m_ftime > 1) {
		m_ftime = 0;
	}
	
	if (gNumChipToCharactor > 0) {
		b2Vec2 player_pos = Player->getCharactorBody()->GetPosition();
		std::vector<BombChip>* chips = m_Explosion->getChips();
		std::vector<BombChip>::iterator iter;
		int i = 0;
		for (iter = chips->begin(); iter != chips->end();) {
			b2Body* chip_body = iter->body;
			b2Vec2 chip_pos = chip_body->GetPosition();
			b2Vec2 distance = player_pos - chip_pos;
			float x = fabs(distance.x);
			float y = fabs(distance.y);

			b2Vec2 velocity = chip_body->GetLinearVelocity();

			if (x < 0.5 && y < 0.5 && iter->power == true) {
				iter->power = false;
				gHeartCount -= BOMBDAMAGE;
				//_world->DestroyBody(chip_body);
				//m_Explosion->getChips().erase(iter++);
				Player->Heartmanager(m_clayer, gHeartCount);
			}
			else ++iter;
			i++;
		}
	}

	if (m_bTakePickaxe) {
		m_BrokenAABB.lowerBound = Player->getCharactorBody()->GetPosition() - b2Vec2(1, 1);
		m_BrokenAABB.upperBound = Player->getCharactorBody()->GetPosition() + b2Vec2(1, 1);
		_world->QueryAABB(&m_BrokenCallback, m_BrokenAABB);
	}

	// 줍기 key 클릭
	if (Player->getMoveState() == MS_GET) {
		// 물약과 충돌시
		if (gNumItemContacts > 0) {
			log("Item Contacts is Yes");										
			PickupItem(&Map_Body.lb_Heartlist);
			
			n++;
			if (n > 10) n = 0;
		}

		if (gNumBombContacts > 0) {
			log("Bomb Contacts is Yes");
			PickupItem(&Map_Body.lb_Bomblist);
		}

		if (gNumLadderContacts > 0) {
			log("Ladder Contacts is Yes");
			PickupItem(&Map_Body.lb_Ladderlist);
		}
		
		// 곡괭이 아이템이 지면과 닿아있고 손에 든 것이 아무것도 없을 때
		if (gNumpickaxeContacts > 0 && !m_bTakePickaxe && !m_bTakeSword && !m_bTakeGun) {
			log("pickaxe contacts");
			PickupItem(&Map_Body.lb_Pickaxelist);
			
			n++;
			if (n > 10) n = 0;
		}
		// 칼 아이템이 지면과 닿아있고 손에 아무것도 든 것이 없을 때
		if (gNumSwordContacts > 0 && !m_bTakeSword && !m_bTakePickaxe && !m_bTakeGun) {
			log("sword contacts");
			PickupItem(&Map_Body.lb_Swordlist);

			n++;
			if (n > 10) n = 0;
		}
		// 총 아이템이 지면과 닿아있고 손에 아무것도 든 것이 없을 때
		if (gNumGunContacts > 0 && !m_bTakeGun && !m_bTakePickaxe && !m_bTakeSword) {
			log("gun contacts");
			PickupItem(&Map_Body.lb_Gunlist);

			n++;
			if (n > 10) n = 0;
		}

	}
	if (g_isJump) {
	
	}
	//log("Player Velocity = (%f , %f)", Player->getCharactorBody()->GetLinearVelocity().x, Player->getCharactorBody()->GetLinearVelocity().y);
	
	// 캐릭터가 몬스터와 충돌 후 캐릭터는 바라보는 방향 반대로 튕겨나가게 되는데 이순간 body에 또 다른 힘을 주면 impulse +연산되어
	// 캐릭터가 의도와 다르게 크게 튕겨나가는 경우가 발생하기 때문에 캐릭터의 Velocity가 일정 수준을 넘어가면 튕겨나가는 정도의 상한을 두었다.
	if (m_Charactor_body->GetLinearVelocity().x > 19 || m_Charactor_body->GetLinearVelocity().y > 14 &&
		m_Charactor_body->GetLinearVelocity().x > -19) {
		log("Player Velocity = (%f , %f)", Player->getCharactorBody()->GetLinearVelocity().x, Player->getCharactorBody()->GetLinearVelocity().y);
		log("!?");

		if(g_bViewing) m_Charactor_body->SetLinearVelocity(b2Vec2(-3, 5));
		else if (!g_bViewing) m_Charactor_body->SetLinearVelocity(b2Vec2(3, 5));
	}


	m_clayer->setPosition(Vec2(wlayer->getPosition() * -1));	// ui Layer를 캐릭터와 함께 이동시켜준다.
	m_RecognizeSp->setPosition(m_CameraSprite->getPosition()); // 캐릭터 인식 범위를 캐릭터와 함께 이동

	m_fdt += dt;
	m_Monster->MonsterPatrol(_world, &Monsters, Player->getCharactorBody(), dt, wlayer, gNumMonsterFootContacts);
	m_Opossum->MonsterPatrol(_world, &Monsters, Player->getCharactorBody(), dt, wlayer, gNumMonsterFootContacts);
	m_Frog->MonsterPatrol(_world, &Monsters, Player->getCharactorBody(), dt, wlayer, gNumFrogFootContacts);
	m_Bat->MonsterPatrol(_world, &Monsters, Player->getCharactorBody(), dt, wlayer, gNumMonsterFootContacts);
	// 몬스터 충돌 및 죽음 체크
	std::list<Monster>::iterator iter;
	for (iter = Monsters.begin(); iter != Monsters.end();) { // 몬스터 리스트를 돌며 죽은 몬스터가 있는지 검사하고 있으면 죽은상태 애니메이션 출력 후 삭제.
		Sprite* sp = static_cast<Sprite*>(iter->body->GetUserData());
		Sprite* recognize_sp = static_cast<Sprite*>(iter->recognize_body->GetUserData());
		

		Rect Monster_rect = sp->getBoundingBox();

		b2Vec2 distance = iter->body->GetPosition() - m_Charactor_body->GetPosition();
		
		// 몬스터 body와 캐릭터 좌우body가 충돌 할 경우 두 거리를 계산해 가장 가까운 body
		if (gNumMonsterContacts > 0) {
			if (distance.x > -1.5 && distance.x < 1.5 && m_fdt > 1) {
				log("Monster Contacts is Yes");
				m_fDelayTime = 2;

				if (g_bViewing) Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
				else if (!g_bViewing) Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
				
				m_fdt = 0;
				gHeartCount--;
				if (gHeartCount <= 0) GameOver(this);
				m_clayer->removeChild(m_HeartLabel, true);

				Player->Heartmanager(m_clayer, gHeartCount);
			}
		}
		if (iter->state == MS_DEATH && iter->time < m_fdt) {
			wlayer->removeChild(sp, true);
			wlayer->removeChild(recognize_sp, true);
			_world->DestroyBody(iter->body);
			_world->DestroyBody(iter->recognize_body);
			Monsters.erase(iter++);
		}
		else ++iter;
	}
	if (gNumFootAttackContacts > 0) {
		AttackMonster(dt);
	}

	// 충돌 후 딜레이
	if (m_fDelayTime > (m_ftime += dt)) {
		g_isLeft = false;
		g_isRight = false;
		g_isStop = false;
	}
	if (m_ftime >= 2) {
		m_ftime = 0;
	}

	if (g_isLeft || g_isRight || g_isStop) {
		float velChange = desiredVel - vel.x;
		float impulse = Player->getCharactorBody()->GetMass() * velChange;
		Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(impulse, 0), Player->getCharactorBody()->GetWorldCenter(), true);
	}
	if (g_isUp) {
		float velChange = desiredVel - vel.y;
		float impulse = Player->getCharactorBody()->GetMass() * velChange;
		Player->getCharactorBody()->SetLinearVelocity(b2Vec2(0, 2));
	}
	// Step : 물리 세계를 시뮬레이션 한다.
	_world->Step(dt, velocityIterations, positionIterations);

	// 모든 물리 객체들은 링크드 리스트에 저장되어 참조해 볼 수 있도록 구현되어 있다.
	// 만들어진 객체 만큼 루프를 돌리면서 바디에 붙인 스프라이트를 여기서 제어한다.

	for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != nullptr) {
			Sprite* spriteData = (Sprite*)b->GetUserData();
			spriteData->setPosition(Vec2(b->GetPosition().x * PTMRATIO, b->GetPosition().y * PTMRATIO));
			spriteData->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));

			if (b->GetIsCh()) {	// World의 Charactor body도 포함하여 검사하기때문에 Charactor body는 Sprite충돌 건너뛰도록
				continue;
			}
			Sprite* Charactor = m_RecognizeSp;
			//log("%f, %f", Charactor->getPosition().x, Charactor->getPosition().y);
			Rect rect = spriteData->getBoundingBox();
			uint16 categori = b->GetFixtureList()->GetFilterData().categoryBits;
			if (rect.intersectsRect(Charactor->getBoundingBox()) && categori == 2) { // 오브젝트 충돌 시 부술 수 있는가 없는가 영역은 Sprite 충돌로 처리하였다.
				//log("Broken On");
				b->SetBroken(true);
			}
			else {
				b->SetBroken(false);
			}
			if (b->GetHp() <= 0) {
				m_DestroyQ.push(b);
			}
		}
	}

	HelloWorld::Destroy_Component(dt);

	for (int i = 0; i < m_DestroyQ.size() + i; i++) {
		b2Body* body = m_DestroyQ.front();

		Sprite* spriteData = static_cast<Sprite*>(body->GetUserData());
		b2Body* bd;
		Sprite* otherData;
		std::vector<b2Body*> joint_body;
		std::vector<b2Joint*>::iterator iter;
		if (body->GetJointList() != nullptr) {
			b2Joint* jointfront = body->GetJointList()->joint;
			b2Joint* j = body->GetJointList()->joint;
			b2JointEdge* jointlist = body->GetJointList();
			for (b2Joint* joint = body->GetJointList()->joint; joint != nullptr; joint = joint->GetNext()) {
				// 이게 어떻게되있는건지 body가 서로 달라도 jointlist는 모든body의 jointbody가 연결되어있다 말도안돼
				// jointlist에서 joint로 연결된 body가 어떤body인지 구분하기위해 category bits를 부여해 구분한뒤 body에 붙어있는 부속body들만 리스트에 넣어 후에 제거해준다.
				// Monster 부속 body category bits 는 0x0030
				UINT16 category = joint->GetBodyA()->GetFixtureList()->GetFilterData().categoryBits;
				UINT16 category2 = joint->GetBodyB()->GetFixtureList()->GetFilterData().categoryBits;
				if(category == 0x0030) 
					joint_body.push_back(joint->GetBodyA());
				if (category2 == 0x0030) 
					joint_body.push_back(joint->GetBodyB());
			}
			std::sort(joint_body.begin(), joint_body.end());
			joint_body.erase(std::unique(joint_body.begin(), joint_body.end()), joint_body.end());
		}

		UINT16 kategori = body->GetFixtureList()->GetFilterData().categoryBits;
		switch (kategori) {
		case 0x0002:
		case 0x0100:
			Map_Body.getLayer()->removeChild(spriteData, true);
			
			break;

		case 0x0010:
			wlayer->removeChild(spriteData, true);
	//		for (int i = 0; i < joint_body.size(); i++) {
		//		bd = joint_body.at(i);

		//		otherData = static_cast<Sprite*>(bd->GetUserData());
		//		wlayer->removeChild(otherData, true);
		//		_world->DestroyBody(bd);
				// 부속body는 Hp를 높게 설정했기때문에 DestoryQ에 올라오지 않는다. 따라서 Q에서 제거해줄 필요가 없다
			//}

			break;

		case 0x2000:
		case 0x3000:
			if (body->GetNewobj()) wlayer->removeChild(spriteData, true);
			else Map_Body.getItemLayer()->removeChild(spriteData, true);
			break;
		}
		_world->DestroyBody(body);
		m_DestroyQ.pop();
	}
}

void HelloWorld::Destroy_Component(float dt) {
	std::vector<Monster_Component>::iterator iter;
	for (iter = m_Monster_Component->begin(); iter != m_Monster_Component->end();) {
		if (!iter->body->IsAwake()) {
			if ((iter->time -= dt) <= 0 && (iter->body->GetJointList() == nullptr)) {
				m_DestroyQ.push(iter->body);
				iter = m_Monster_Component->erase(iter);
			}
			else { iter++; }
		}
		else {
			iter->time = 10;
			iter++;
		}
	}
}

void HelloWorld::power_enegy(float power) {
	m_fpowertime += power;
}


bool HelloWorld::onTouchBegan(Touch* touch, Event* event) {
	m_bTouchHolding = true;
	m_clayer->removeChild(m_HeartLabel, true);
	m_touchPoint = touch->getLocation();
	m_touchPoint2 = Node::convertToNodeSpace(m_touchPoint);
	log("nodeSpace.. %f, %f", m_touchPoint2.x, m_touchPoint2.y);
	
	//AttackMonster();

	this->schedule(schedule_selector(HelloWorld::TouchHold), 0.5f);

	return true;
}

void HelloWorld::onTouchMoved(Touch* touch, Event* event) {
}

void HelloWorld::onTouchEnded(Touch* touch, Event* event) {
	m_bTouchHolding = false;
	// 캐릭터의 왼손에 쥔 아이템 내구도가 0이 될때
	if (m_bTakePickaxe && Player->getCharactor_Hand()->getDurability() <= 0) {
		m_bTakePickaxe = false;
		
		//wlayer->removeChild(m_PlayerSprite);					// 현재 body에 적용된 Sprite 지움
		wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
		m_PlayerSprite = Player->AnimationState(MS_STOP, wlayer);		// 다시 body에 movestate 상태에 따라 애니메이션을 만들어 넣어줌
		m_PlayerSprite = Player->getPlayer();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
	
		if (g_bViewing) m_PlayerSprite->setFlippedX(false);
		else if (!g_bViewing) m_PlayerSprite->setFlippedX(true);
	}
	else if(m_bTakePickaxe) {
		wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
		m_PlayerSprite = Player->getCharactor_Hand()->AnimationState("Pickaxe", Hand_On, wlayer, g_bViewing);
		m_PlayerSprite = Player->getPlayer();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴

		if (g_bViewing) m_PlayerSprite->setFlippedX(false);
		else if (!g_bViewing) m_PlayerSprite->setFlippedX(true);
	}
}

void HelloWorld::AttackMonster(float dt) {
	std::list<Monster>::iterator iter;
	for (iter = Monsters.begin(); iter != Monsters.end(); ++iter) {
		if (iter->state == MS_DEATH) break; // 몬스터가 죽은 후 바로 사라지는게 아니라 애니메이션 출력 후 죽은 상태라면 입력받는 Attack을 무시
		Sprite* sp = static_cast<Sprite*>(Player->getCharactorBody()->GetUserData());
		Sprite* MonsterSp = static_cast<Sprite*>(iter->body->GetUserData());
		Rect rect = sp->getBoundingBox();
		Vec2 p = sp->getPosition();

		b2Vec2 distance = iter->body->GetPosition() - m_Charactor_body->GetPosition();

		// 터치 포인트가 몬스터 스프라이트 내부이며 캐릭터인식범위 내에 몬스터 스프라이트가 있을시.
		if (rect.intersectsRect(MonsterSp->getBoundingBox()) && (iter->shooting_Delay -= dt) <= 0) {
			iter->shooting_Delay = 0.5f;
			int hp = iter->body->GetHp();
			hp = hp - 2;
			iter->body->SetHp(hp);
		//	if (hp <= 0) hp = 0;
			if (hp <= 0) {
				iter->state = MS_DEATH;
				m_fdt = 0;
				iter->time = 0.7f;
			}
			else {
				iter->state = MS_PAIN;
				iter->dt = 0;
				iter->time = 0.5;
			}
			/*
			wlayer->removeChild(m_PlayerSprite);
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
			
			Player->getCharactor_Hand()->AnimationState("Pickaxe", ATTACK, wlayer, g_bViewing);
			m_PlayerSprite = Player->getCharactor_Hand()->getSprite();
			m_Charactor_body->SetUserData(m_PlayerSprite);

			// 카메라 스프라이트 기준 왼쪽 오른쪽 클릭시 출력되는 애니메이션을 다르게 해준다.
			if (m_touchPoint2.x > camera->getPosition().x) {
				m_PlayerSprite->setFlippedX(false);
			}
			else {
				m_PlayerSprite->setFlippedX(true);
			}
			m_Monster->SpriteChange(iter, wlayer);
			//if (iter->state == MS_DEATH ) {
			//	wlayer->removeChild(sp, true);
			//	_world->DestroyBody(iter->body);
			//	iter = Monsters->erase(iter);
			//}*/
		}

		else if (distance.x > -0.5 && distance.x < 0.5 && distance.y < 0.1 && (iter->shooting_Delay -= dt) <= 0) {
			iter->shooting_Delay = 0.5f;
			int hp = iter->body->GetHp();
			hp--;
			iter->body->SetHp(hp);
			iter->recognize_body->SetHp(hp);
			//	if (hp <= 0) hp = 0;
			
			m_Charactor_body->ApplyLinearImpulse(b2Vec2(0, 1), m_Charactor_body->GetWorldCenter(), true);

			if (hp == 0) {
				iter->state = MS_DEATH;
				m_fdt = 0;
				iter->time = 0.7f;
			}
			else {
				iter->state = MS_PAIN;
				iter->dt = 0;
				iter->time = 0.5;
			}


			m_Monster->SpriteChange(iter, wlayer);
		}

		if (iter->before_hp > iter->body->GetHp()) {
			log("데미지 받음!");
			iter->state = MS_PAIN;
			iter->before_hp = iter->body->GetHp();
			if (iter->obj_name == "Skel") {
				iter->body->SetLinearVelocity(b2Vec2(0, 0));
				m_Monster->SpriteChange(iter, wlayer);
			}
			else if (iter->obj_name == "Opossum") {
				iter->body->SetLinearVelocity(b2Vec2(0, 0));
				m_Opossum->SpriteChange(iter, wlayer);
			}
			else if (iter->obj_name == "Frog") {
				iter->body->SetLinearVelocity(b2Vec2(0, 0));
				m_Frog->SpriteChange(iter, wlayer);
			}
			else if (iter->obj_name == "Bat") {
				iter->body->SetLinearVelocity(b2Vec2(0, 0));
				m_Bat->SpriteChange(iter, wlayer);
			}
		}
	}
}


void HelloWorld::TouchHold(float touchhold_time) {
	if (m_bTouchHolding) {
		for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext()) {
			if (b->GetUserData() != nullptr) {
				Sprite* SpriteData = (Sprite*)b->GetUserData();
				Rect rect = SpriteData->getBoundingBox();
				if (rect.containsPoint(m_touchPoint2)) {
					if (b->GetBroken() && m_bTakePickaxe) { // 블럭의 상태가 부술 수 있는 상태여야 하고, 캐릭터가 곡괭이 아이템을 들고 있을 경우

						int nBlockHp = b->GetHp();
						nBlockHp--;
						b->SetHp(nBlockHp);
						Sprite* sp = (Sprite*)Player->getCharactorBody()->GetUserData();
						Sprite* camera = (Sprite*)Player->getCameraBody()->GetUserData();
						Rect rect = sp->getBoundingBox();

						wlayer->removeChild(m_PlayerSprite);
						wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
						if (m_bTakePickaxe) {
							float MaxY = rect.getMaxY();
							float MinY = rect.getMinY();

							// 캐릭터의 현재 위치와 터치좌표를 비교해서 크게 3가지 경우로 나누어
							// 캐릭터 상단, 중단, 하단 클릭시 출력되는 애니메이션을 다르게 설정.
							if (m_touchPoint2.y < MaxY && m_touchPoint2.y > MinY) { // 중단
								Player->getCharactor_Hand()->AnimationState("Pickaxe", ATTACK, wlayer, g_bViewing);
								//log("touch %f, max %f, Min %f", m_touchPoint2.y, MaxY, MinY);
							}
							else if(m_touchPoint2.y > MaxY) {						// 상단
								Player->getCharactor_Hand()->AnimationState("Pickaxe", UP_ATTACK, wlayer, g_bViewing);
								//log("touch %f, max %f", m_touchPoint2.y, MaxY);
							}
							else if (m_touchPoint2.y < MinY) {						// 하단
								Player->getCharactor_Hand()->AnimationState("Pickaxe", UNDER_ATTACK, wlayer, g_bViewing);
								//log("touch %f, Min %f", m_touchPoint2.y, MinY);
							}
							m_PlayerSprite = Player->getCharactor_Hand()->getSprite();

							// 카메라 스프라이트 기준 왼쪽 오른쪽 클릭시 출력되는 애니메이션을 다르게 해준다.
							if (m_touchPoint2.x > camera->getPosition().x) {
								m_PlayerSprite->setFlippedX(false);
							}
							else {
								m_PlayerSprite->setFlippedX(true);
							}
							//if(g_bViewing) m_PlayerSprite->setFlippedX(false);
							//else m_PlayerSprite->setFlippedX(true);
						}
						int Durability = Player->getCharactor_Hand()->getDurability();
						if (nBlockHp >= 2) {
							m_brokenbl = Sprite::create("Images/brokenblock1.png");
							m_brokenbl->setAnchorPoint(Vec2(0, 0));
							Map_Body.getLayer()->removeChild((Sprite*)b->GetUserData(), true);	// 먼저 Tilemap Layer에 있는 Sprite를 지워주고
							wlayer->addChild(m_brokenbl, 3);									// 새로 그자리에 배경레이어위로 Sprite를 올려준다.
							b->SetUserData((Sprite*)m_brokenbl);
							Player->getCharactor_Hand()->setDurability(--Durability);			// 블럭을 부술때마다 내구도 감소
							break;
						}
						else if (nBlockHp == 1) {
							m_brokenbl = Sprite::create("Images/brokenblock2.png");
							m_brokenbl->setAnchorPoint(Vec2(0, 0));
							wlayer->removeChild((Sprite*)b->GetUserData(), true);				// 이제 그자리에는 배경 레이어 위의 Sprite가 올라가있으므로 배경레이어의 Sprite를 지워주고
							wlayer->addChild(m_brokenbl, 3);									// 새 Sprite를 올려준다.
							b->SetUserData((Sprite*)m_brokenbl);
							Player->getCharactor_Hand()->setDurability(--Durability);			// 블럭을 부술때마다 내구도 감소
							break;
						}
						else if (nBlockHp == 0) {
							wlayer->removeChild((Sprite*)b->GetUserData(), true);				// 블럭의 hp가 다했으므로 Sprite를 완전히 지워준다.
							_world->DestroyBody(b);												// 블럭의 body도 world에서 지워준다.	
							Player->getCharactor_Hand()->setDurability(--Durability);			// 블럭을 부술때마다 내구도 감소
							break;
						}
						
					}
					break;
				}
			}
		}
	}
}

void HelloWorld::PickupItem(std::list<b2Body*> *_item) {
	std::list<b2Body*>::iterator iter;
	if (_item->size() <= 0) return;

	b2Body* b = _item->front();

	b2Fixture* fixture = b->GetFixtureList();

	uint16 nKategori = fixture->GetFilterData().categoryBits;

	for (iter = _item->begin(); iter != _item->end(); ++iter) {
		b2Body* item = *iter;
		Sprite* item_sp = static_cast<Sprite*>(item->GetUserData());
		Sprite* Charactor = (Sprite*)Player->getCharactorBody()->GetUserData();
		Rect rect = Charactor->getBoundingBox();
		if (rect.intersectsRect(item_sp->getBoundingBox())) {

			if (nKategori == 0x1000) {
				//Player->PlusHeart();										// 물약을 획득했으므로 생명력(Heart) 하나 생성
				//Sprite* sp = Player->getHeartSprite()->at(gHeartCount);		// 하트Sprite를 Scene에 올리기 위해 HeartSprite vector에서 가져옴 이 Sprite는 위치정보를 포함하고 있음.
				//m_clayer->addChild(sp, 3);										// clayer 에 추가.
				gHeartCount++;
				m_clayer->removeChild(m_HeartLabel, true);
				
				Player->Heartmanager(m_clayer, gHeartCount);

				Map_Body.getLayer()->removeChild(item_sp, true);
				_world->DestroyBody(item);								// 또한 해당 Body도 같이 지워줌

				Map_Body.lb_Heartlist.erase(iter);

				break;												// 한번에 하나의 물약만 처리하므로 break;
			}

			else if (nKategori == 0x2000) {
				m_bTakeSword = false;
				m_bTakeGun = false;
				m_bTakePickaxe = true;
				std::string pickaxe = "Pickaxe";
				if (item->GetNewobj()) {
					wlayer->removeChild(item_sp, true);
				}
				else Map_Body.getItemLayer()->removeChild(item_sp, true);		// 초기 생성되는 Item은 itemLayer에 Node로 등록되어 있지만
																			// 캐릭터가 획득 후 버려진 Item은 wlayer에 Node로 등록되어있다.
				_world->DestroyBody(item);
				Map_Body.lb_Pickaxelist.erase(iter);

				Player->Pickup(&pickaxe, Player->getMoveState(), wlayer); // 아이템 이름을 넘김
				break;
			}

			else if (nKategori == 0x3000) {

				gBombCount++;
				if (item->GetNewobj()) {
					wlayer->removeChild(item_sp, true);
				}
				else Map_Body.getItemLayer()->removeChild(item_sp, true);

				Player->Bombmanager(m_clayer, gBombCount);

				_world->DestroyBody(item);
				Map_Body.lb_Bomblist.erase(iter);

				break;
			}
			else if (nKategori == 0x4000) {
				m_bTakePickaxe = false;
				m_bTakeGun = false;
				m_bTakeSword = true;
				std::string sword = "Sword";
				if (item->GetNewobj()) {
					wlayer->removeChild(item_sp, true);
				}
				else Map_Body.getItemLayer()->removeChild(item_sp, true);

				_world->DestroyBody(item);
				Map_Body.lb_Swordlist.erase(iter);

				Player->Pickup(&sword, Player->getMoveState(), wlayer);
				break;
			}

			else if (nKategori == 0x5000) {
				m_bTakePickaxe = false;
				m_bTakeSword = false;
				m_bTakeGun = true;
				std::string gun = "Gun";
				if (item->GetNewobj()) {
					wlayer->removeChild(item_sp, true);
				}
				else Map_Body.getItemLayer()->removeChild(item_sp, true);

				_world->DestroyBody(item);
				Map_Body.lb_Gunlist.erase(iter);

				Player->Pickup(&gun, Player->getMoveState(), wlayer);
				break;
			}
			else if (nKategori == 0x7000) {
				gLadderCount++;
				if (item->GetNewobj()) {
					wlayer->removeChild(item_sp, true);
				}
				else Map_Body.getItemLayer()->removeChild(item_sp, true);

				Player->Laddermanager(m_clayer, gLadderCount);
				_world->DestroyBody(item);
				Map_Body.lb_Ladderlist.erase(iter);
				break;
			}
		}
	}
}

bool HelloWorld::createBox2dWorld(bool debug) {

	HelloWorld::CreateMap();
	HelloWorld::CreateCharactor();
	HelloWorld::CreateMonster();
	HelloWorld::CreateBt();
//	HelloWorld::CreateCharactor();
	

	// 디버그 드로잉 설정
	if (debug) {
		// 적색 : 현재 물리운동을 하는 것.
		// 회색 : 현재 물리 운동량이 없는것.
		m_debugDraw = new GLESDebugDraw(PTMRATIO);
		_world->SetDebugDraw(m_debugDraw);

		uint32 flags = 0;
		flags += b2Draw::e_shapeBit;
		//flags += b2Draw::e_aabbBit;
		//flags += b2Draw::e_jointBit;
		//flags += b2Draw::e_pairBit;
		//flags += b2Draw::e_centerOfMassBit;
		m_debugDraw->SetFlags(flags);
	}



	return true;
}

void HelloWorld::CreateBt() {
	m_pAttackButton_Images = MenuItemImage::create("Images/ui/Attack_button.png", "Images/ui/Attack_button_selected.png",
		CC_CALLBACK_1(HelloWorld::doAction, this));

	m_pAttackButton = Menu::create(m_pAttackButton_Images, nullptr);
	m_pos = m_clayer->getContentSize();
	m_pAttackButton->setPosition(Vec2(m_pos.x - 60, m_pos.y - 290));
	m_clayer->addChild(m_pAttackButton, 4);

	m_pLadderButton_Images = MenuItemImage::create("Images/ui/Ladder_button.png", "Images/ui/Ladder_button_selected.png",
		CC_CALLBACK_1(HelloWorld::doLadderequip, this));
	m_pLadderButton = Menu::create(m_pLadderButton_Images, nullptr);
	m_pos = m_clayer->getContentSize();
	m_pLadderButton->setPosition(Vec2(m_pos.x - 120, m_pos.y - 290));
	m_clayer->addChild(m_pLadderButton, 4);
}

void HelloWorld::CreateMap() {
	texture = Director::getInstance()->getTextureCache()->addImage("Images/SpinningPeas.png");

	this->setAnchorPoint(Point::ZERO);

	b2Vec2 gravity;
	gravity.Set(0.0f, -30.0f);
	_world = new b2World(gravity);

	_world->SetAllowSleeping(true);
	_world->SetContinuousPhysics(true);

	auto debugLayer = B2DebugDrawLayer::create(_world, PTMRATIO);
	this->addChild(debugLayer, 9999);

	map = TMXTiledMap::create("TileMaps/BaseGround7.tmx");
	map->setAnchorPoint(Point::ZERO);
	map->setPosition(0, 0);

	m_Explosion = new Explosion;

	First_Floor.CreateCave(map, _world);

	Map_Body.initCollisionMap(map, _world);

	//this->setScale(0.23f);

	wlayer->addChild(map);
}

void HelloWorld::CreateCharactor() {
	TMXObjectGroup* objects = map->getObjectGroup("SpawnPoint");
	ValueMap spawnPoint = objects->getObject("Sp");

	m_HeartLabel = new Label;

	int x = spawnPoint["x"].asInt();
	int y = spawnPoint["y"].asInt();

	CharactorPoint = Vec2(x, y);

	Player = new CharactorCreator;
	Player->CreateCharactor(_world, CharactorPoint, wlayer); // Charactor Sprite Charactor Layer
	m_PlayerSprite = Player->getPlayer();
	m_Charactor_body = Player->getCharactorBody();

	m_CameraSprite = (Sprite*)Player->getCameraBody()->GetUserData();
	wlayer->addChild(m_CameraSprite, 1); // Camera Sprite Charactor Layer

	//m_BrokenAABB.lowerBound = Player->getCharactorBody()->GetPosition() - b2Vec2(1, 1);
	//m_BrokenAABB.upperBound = Player->getCharactorBody()->GetPosition() + b2Vec2(1, 1);

	auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Recognize.png");
	m_RecognizeSp = Sprite::createWithTexture(texture, Rect(0, 0, 37, 37));
	//m_RecognizeSp->setAnchorPoint(Vec2::ZERO);
	m_RecognizeSp->setScale(1.5);
	wlayer->addChild(m_RecognizeSp, 3);

	b2WeldJointDef weldJointDef;
	weldJointDef.Initialize(Player->getCharactorBody(), Player->getCameraBody(), Player->getCharactorBody()->GetPosition());
	_world->CreateJoint(&weldJointDef);

	//b2WeldJointDef b;
	//b.Initialize(Player->getCharactorBody(), m_Monster->a(), Player->getCharactorBody()->GetPosition());
	//_world->CreateJoint(&b);

	Rect myBoundary = Rect(0, 0, 1920, 1024);

	wlayer->runAction(Follow::create(m_CameraSprite, myBoundary));  // Camera Action Charactor Sprite
	//this->runAction(Follow::create(m_CameraSprite, myBoundary));
	//m_clayer->runAction(Follow::create(m_CameraSprite, myBoundary));

	_world->SetContactListener(&gContactInstance); // contacts recognize
	m_fImpulse = Player->getCharactorBody()->GetMass() * 7; // Jump Impulse

	Size size = m_CameraSprite->getContentSize();

	gHeartCount = Heart_Count;
	gBombCount = Bomb_Count;
	gLadderCount = Ladder_Count;
	m_clayer->addChild(Player->getHeartSprite()->at(0), 3);
	m_clayer->addChild(Player->getBombSprite(), 3);
	m_clayer->addChild(Player->getLadderSprite(), 3);
	
	//m_HeartLabel = Label::createWithSystemFont(m_Heart, "", 30);
	
	Player->Heartmanager(m_clayer, Heart_Count);
	//m_clayer->addChild(m_HeartLabel, 3);

	Player->Bombmanager(m_clayer, Bomb_Count);

	Player->Laddermanager(m_clayer, Ladder_Count);

	GunEffect = new EffectAnimation;
}


void HelloWorld::CreateMonster() {
	TMXObjectGroup* objects = map->getObjectGroup("M_SpawnPoint");
	MonsterOnWorld(objects);

	Monster Lastbody = Monsters.back();
	m_Monster_Component = new std::vector<Monster_Component>;
	for (b2Joint* joint = Lastbody.body->GetJointList()->joint; joint != nullptr; joint = joint->GetNext()) {
		// 이게 어떻게되있는건지 body가 서로 달라도 jointlist는 world의 모든body들의 joint가 연결되어있다 말도안돼
		// 따라서 맨 마지막 생성된 body의 jointlist를 가져와 모든 몬스터의 joint를 저장한 뒤 일정시간 동안 움직임이 없을 경우 DestoryQ에 올려주는식으로 world에서 제거
		// Monster 부속 body category bits 는 0x0030
		UINT16 category = joint->GetBodyA()->GetFixtureList()->GetFilterData().categoryBits;
		UINT16 category2 = joint->GetBodyB()->GetFixtureList()->GetFilterData().categoryBits;
		if (category == 0x0030)
			m_Monster_Component->push_back(Monster_Component(joint->GetBodyA(), 10.0f));
		if (category2 == 0x0030)
			m_Monster_Component->push_back(Monster_Component(joint->GetBodyB(), 10.0f));
	}
}

void HelloWorld::MonsterOnWorld(TMXObjectGroup* obj) {
	cocos2d::ValueVector monster_collision = obj->getObjects();
	m_Monster = new MonsterCreator;
	m_Opossum = new Opossum;
	m_Frog = new Frog;
	m_Bat = new Bat;
	//Monsters = new std::list<Monster>;
	// 몬스터즈 자체를 구조체로
	for (Value object : monster_collision) {
		ValueMap monster = object.asValueMap();
		std::string obj_name = monster["Name"].asString();
	
		if (obj_name == "Skel") {
			float x = monster["x"].asFloat();
			float y = monster["y"].asFloat();

			b2Body* Mob = m_Monster->CreateMonster(_world, Vec2(x, y), wlayer);
			b2Body* recognize = m_Monster->getRecognizeBody();
			Monsters.push_back(Monster(obj_name, Mob, recognize, MS_STOP, Mob->GetHp(), 0));
		}
		else if (obj_name == "Opossum") {
			float x = monster["x"].asFloat();
			float y = monster["y"].asFloat();

			b2Body* Mob = m_Opossum->CreateMonster(_world, Vec2(x, y), wlayer);
			b2Body* recognize = m_Opossum->getRecognizeBody();
			Monsters.push_back(Monster(obj_name, Mob, recognize, MS_STOP, Mob->GetHp(), 0));
		}
		else if (obj_name == "Frog") {
			float x = monster["x"].asFloat();
			float y = monster["y"].asFloat();

			b2Body* Mob = m_Frog->CreateMonster(_world, Vec2(x, y), wlayer);
			b2Body* recognize = m_Frog->getRecognizeBody();
			Monsters.push_back(Monster(obj_name, Mob, recognize, MS_STOP, Mob->GetHp(), 0));
		}
		else if (obj_name == "Bat") {
			float x = monster["x"].asFloat();
			float y = monster["y"].asFloat();

			b2Body* Mob = m_Bat->CreateMonster(_world, Vec2(x, y), wlayer);
			b2Body* recognize = m_Bat->getRecognizeBody();
			Monsters.push_back(Monster(obj_name, Mob, recognize, MS_STOP, Mob->GetHp(), 0));
		}
	}
}

void HelloWorld::doAction(Ref* pSender) {
	log("어택!");
	if (m_bTakePickaxe) {
		if (g_isAttackOn) {
			g_ATTACK = true;
			g_isAttackOn = false;
			g_isLeft = false;
			g_isRight = false;
			m_fAttackDelaytime = 0.5f;
			m_fAttackCooltime = 1.5f;
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_STOP, ATTACK, "Pickaxe");
			else
				HelloWorld::KeyAction(MS_GET, ATTACK, "");
		}
	}
	if (m_bTakeSword) {
		if (g_isAttackOn) {
			g_ATTACK = true;
			g_isAttackOn = false;
			g_isLeft = false;
			g_isRight = false;
			m_fAttackDelaytime = 0.5f; // 공격 모션 시간
			m_fAttackCooltime = 1.5f; // 공격 후 쿨타임
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_STOP, ATTACK, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(MS_STOP, ATTACK, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(MS_STOP, ATTACK, "Gun");
			else
				HelloWorld::KeyAction(MS_GET, ATTACK, "");
		}
	}
	else if (m_bTakeGun) {
		if (g_isAttackOn) {
			g_ATTACK = true;
			g_isAttackOn = false;
			g_isLeft = false;
			g_isRight = false;
			m_fAttackDelaytime = 0.5f;
			m_fAttackCooltime = 1.5f;
			HelloWorld::KeyAction(MS_STOP, ATTACK, "Gun");
			HelloWorld::BulletFire();
		}
	}
}

void HelloWorld::doLadderequip(Ref* pSender) {
	log("사다리 설치!");
	if (gLadderCount > 0 && m_fLadderCooltime <= 0) {
		m_fLadderCooltime = 2;

		gLadderCount--;
		Player->Laddermanager(m_clayer, gLadderCount);

		if (g_bViewing) {
			Map_Body.CreateObj("Ladder", Player->getCharactorBody()->GetPosition() + b2Vec2(1, 0), _world, wlayer);
			Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
		}
		else if (!g_bViewing) {
			Map_Body.CreateObj("Ladder", Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
			Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
		}

		m_EnableLadder.push_back(Map_Body.getItemBody());
		if (g_bViewing) m_PlayerSprite->setFlippedX(false);
		else if (!g_bViewing) m_PlayerSprite->setFlippedX(true);
		//log("Player position %f %f", sp->getPosition().x, sp->getPosition().y);
	}

}

void HelloWorld::BulletFire() {
	b2Body* bullet;
	if (g_bViewing) {
		Map_Body.CreateBullet(Player->getCharactorBody()->GetPosition() + b2Vec2(1, 0), _world, wlayer);
		bullet = Map_Body.getBulletBody();
		bullet->ApplyLinearImpulse(b2Vec2(20, 0), Player->getCharactorBody()->GetWorldCenter(), true);
		Bullets.push_back(Bullet(bullet, bullet->GetPosition(), m_aabbb));
	}
	else if (!g_bViewing) {
		Map_Body.CreateBullet(Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
		bullet = Map_Body.getBulletBody();
		bullet->ApplyLinearImpulse(b2Vec2(-20, 0), Player->getCharactorBody()->GetWorldCenter(), true);
		Bullets.push_back(Bullet(bullet, bullet->GetPosition(), m_aabbb));
	}
}

void HelloWorld::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		if (m_bCan_Move && Player->getMoveState() != MS_UPDOWN) {
			if (!g_isThrow) {
				g_isRight = false;
				g_isLeft = true;									// Body 움직임위해 변경
				g_bViewing = false;
				g_isAttackOn = true;
				if (m_bTakePickaxe)
					HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Pickaxe");
				else if (m_bTakeSword)
					HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Sword");
				else if (m_bTakeGun)
					HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Gun");
				else
					HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "");

				m_before_state = MS_LEFT;
				m_before_handstate = LeftHand_On;
			}
		}
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		if (!g_isThrow) {
			if (m_bCan_Move && Player->getMoveState() != MS_UPDOWN) {
				g_isRight = false;
				g_isLeft = false;
				g_isStop = true;
				g_isAttackOn = true;
				if (m_bTakePickaxe)
					HelloWorld::KeyAction(MS_GET, Hand_On, "Pickaxe");
				else if (m_bTakeSword)
					HelloWorld::KeyAction(MS_GET, Hand_On, "Sword");
				else if (m_bTakeGun)
					HelloWorld::KeyAction(MS_GET, Hand_On, "Gun");
				else
					HelloWorld::KeyAction(MS_GET, Hand_On, "");

				if (g_bViewing) m_PlayerSprite->setFlippedX(false);
				else if (!g_bViewing) m_PlayerSprite->setFlippedX(true);

				m_before_state = MS_GET;
				m_before_handstate = Hand_On;
			}
		}
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		if (m_bCan_Move && Player->getMoveState() != MS_UPDOWN) {
			if (!g_isThrow) {
				g_isLeft = false;
				g_isRight = true;
				g_bViewing = true;
				g_isAttackOn = true;
				if (m_bTakePickaxe)
					HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Pickaxe");
				else if (m_bTakeSword)
					HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Sword");
				else if (m_bTakeGun)
					HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Gun");
				else
					HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "");

				m_before_state = MS_RIGHT;
				m_before_handstate = RightHand_On;
			}
		}
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		if (!g_isThrow) {
			if (m_bCan_UpDown) {
				g_isUp = true;
				g_isLadderStop = false;
				if (m_bTakePickaxe)
					HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Pickaxe");
				else if (m_bTakeSword)
					HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Sword");
				else if (m_bTakeGun)
					HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Gun");
				else
					HelloWorld::KeyAction(MS_UPDOWN, Hand_On, "");
			}
		}
		break;
	case EventKeyboard::KeyCode::KEY_ALT:
		g_isJump = true;
		//if (m_bTakePickaxe)
		//	HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "Pickaxe");
		//else if (m_bTakeSword)
		//	HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "Sword");
		//else
		//	HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "");

		if (g_isLeft) m_PlayerSprite->setFlippedX(true);
		if (g_isRight) m_PlayerSprite->setFlippedX(false);

		m_nJumpStep = 5;
		break;

	case EventKeyboard::KeyCode::KEY_Z:
		if (m_bTakePickaxe) {
			m_bTakePickaxe = false;
			
			if (g_bViewing) {
				Map_Body.CreatePickaxe(Player->getCharactorBody()->GetPosition(), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			else if (!g_bViewing) {
				Map_Body.CreatePickaxe(Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
			m_PlayerSprite = Player->AnimationState(MS_STOP, wlayer);		// 다시 body에 movestate 상태에 따라 애니메이션을 만들어 넣어줌
			m_PlayerSprite = Player->getPlayer();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
			
			if (g_bViewing) m_PlayerSprite->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->setFlippedX(true);
			//log("Player position %f %f", sp->getPosition().x, sp->getPosition().y);
		}
		else if (m_bTakeSword) {
			m_bTakeSword = false;

			if (g_bViewing) {
				Map_Body.CreateSword(Player->getCharactorBody()->GetPosition(), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			else if (!g_bViewing) {
				Map_Body.CreateSword(Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
			m_PlayerSprite = Player->AnimationState(MS_STOP, wlayer);		// 다시 body에 movestate 상태에 따라 애니메이션을 만들어 넣어줌
			m_PlayerSprite = Player->getPlayer();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴

			if (g_bViewing) m_PlayerSprite->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->setFlippedX(true);
		}
		else if (m_bTakeGun) {
			m_bTakeGun = false;

			if (g_bViewing) {
				Map_Body.CreateGun(Player->getCharactorBody()->GetPosition(), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			else if (!g_bViewing) {
				Map_Body.CreateGun(Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
			m_PlayerSprite = Player->AnimationState(MS_STOP, wlayer);
			m_PlayerSprite = Player->getPlayer();

			if (g_bViewing) m_PlayerSprite->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->setFlippedX(true);
		}
		break;
	case EventKeyboard::KeyCode::KEY_B:
		if (gBombCount > 0 && !g_isLeft && !g_isRight && !g_isStop) { //멈췃을 시 + 폭탄 소유 시
			g_isThrow = true;
			m_bCan_Move = false;

			if (m_bTakePickaxe) 
				wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
			else 
				wlayer->removeChild(m_PlayerSprite, true);

			std::string bomb = "Bomb";
			m_PlayerSprite = Player->Pickup(&bomb, Player->getMoveState(), wlayer);

			if (g_bViewing)
				m_PlayerSprite->setFlippedX(false);
			else if (!g_bViewing)
				m_PlayerSprite->setFlippedX(true);
		}
		break;
	}

//	if (g_isLeft && g_isRight) {
	//	if (m_bTakePickaxe)
	//		HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
	//	else if (m_bTakeSword)
	//		HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
//	}
//	if (g_isStop && g_isRight) {
//		if (m_bTakePickaxe)
//			HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
//		else if (m_bTakeSword)
//			HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
//	}
	//if (g_isThrow && g_isLeft || g_isRight) {
	//	Player->setMoveState(MS_STOP);
		//if (m_bTakePickaxe) {
	//		wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
	//		Player->getCharactor_Hand()->AnimationState(Hand_On, wlayer);
		//	m_PlayerSprite = Player->getCharactor_Hand()->getSprite();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
		//}
	//	else {
	//		wlayer->removeChild(m_PlayerSprite);					// 현재 body에 적용된 Sprite 지움
	//		Player->AnimationState(Player->getMoveState(), wlayer);		// 다시 body에 movestate 상태에 따라 애니메이션을 만들어 넣어줌
	//		m_PlayerSprite = Player->getPlayer();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
	//	}
	//}
	
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		if (!g_isThrow && Player->getMoveState() != MS_UPDOWN) {
			g_isRight = false;
			g_isLeft = false;
			g_bViewing = false;
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Gun");
			else
				HelloWorld::KeyAction(MS_STOP, Hand_On, "");
		}
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		if (!g_isThrow && Player->getMoveState() != MS_UPDOWN) {
			g_isStop = false;
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Gun");
			else
				HelloWorld::KeyAction(MS_STOP, Hand_On, "");

			if (g_bViewing) m_PlayerSprite->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->setFlippedX(true);
		}
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		if (!g_isThrow && Player->getMoveState() != MS_UPDOWN) {
			g_isLeft = false;
			g_isRight = false;
			g_bViewing = true;
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Gun");
			else
				HelloWorld::KeyAction(MS_STOP, Hand_On, "");

		}
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		if (!g_isThrow) {
			//	g_isLeft = false;
			//	g_isRight = false;
				g_isUp = false;
				g_isLadderStop = true;

				//HelloWorld::KeyAction(MS_UPDOWN, Hand_On, "");
			
		}
		break;
	case EventKeyboard::KeyCode::KEY_ALT:
		g_isJump = false;
		//if (m_bTakePickaxe)
		//	HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
		//else if (m_bTakeSword)
		//	HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
		//else
		//	HelloWorld::KeyAction(MS_STOP, Hand_On, "");

		break;
	case EventKeyboard::KeyCode::KEY_Z:
		if (m_bTakePickaxe) m_bTakePickaxe = false;
		break;

	case EventKeyboard::KeyCode::KEY_B:
		g_isThrow = false;
		m_bCan_Move = true;
		if (gBombCount > 0 && !g_isLeft && !g_isRight && !g_isStop) {
			//if (m_fpowertime < 1) {
				m_throwpower.x = 6.01;
				m_throwpower.y = 6;
			//}
		/*
			if (m_fpowertime > 1 && m_fpowertime < 2) {
				m_throwpower.x = 2.3;
				m_throwpower.y = 2.2;
			}
			if (m_fpowertime > 2 && m_fpowertime < 3) {
				m_throwpower.x = 3.6;
				m_throwpower.y = 2.4;
			}
			if (m_fpowertime > 3 && m_fpowertime < 4) {
				m_throwpower.x = 4.9;
				m_throwpower.y = 3.6;
			}
			if (m_fpowertime > 4 && m_fpowertime < 5) {
				m_throwpower.x = 5.2;
				m_throwpower.y = 4.8;
			}
			if (m_fpowertime > 5 && m_fpowertime < 6) {
				m_throwpower.x = 6.5;
				m_throwpower.y = 6;
			}*/
			if (m_fpowertime > 6) m_fpowertime = 5.5;
			
			gBombCount--;
			Player->Bombmanager(m_clayer, gBombCount);
			b2Body* bomb_body;
			if (g_bViewing) {
				bomb_body = Map_Body.CreateBomb(Player->getCharactorBody()->GetPosition(), _world, wlayer);
				Map_Body.getBombBody()->ApplyLinearImpulse(b2Vec2(m_throwpower.x, m_throwpower.y), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			else if (!g_bViewing) {
				bomb_body = Map_Body.CreateBomb(Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
				Map_Body.getBombBody()->ApplyLinearImpulse(b2Vec2(-m_throwpower.x, m_throwpower.y), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			m_Explosion->CreateExplosion(bomb_body, 3, wlayer);

			if (m_bTakePickaxe) {
				wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
				std::string bomb = "Pickaxe";
				Player->Pickup(&bomb, Player->getMoveState(), wlayer);		// 다시 body에 movestate 상태에 따라 애니메이션을 만들어 넣어줌
			}
			else {
				wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
				m_PlayerSprite = Player->AnimationState(MS_STOP, wlayer);		// 다시 body에 movestate 상태에 따라 애니메이션을 만들어 넣어줌
			}

			m_PlayerSprite = Player->getPlayer();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
			
			if (g_bViewing) 
				m_PlayerSprite->setFlippedX(false);
			else if (!g_bViewing)
				m_PlayerSprite->setFlippedX(true);
			m_fpowertime = 0;
		}
	}
	/*
	if (!g_isLeft && g_isRight) {
		if (m_bTakePickaxe)
			HelloWorld::KeyAction(MS_RIGHT, LeftHand_On, "Pickaxe");
		else if (m_bTakeSword)
			HelloWorld::KeyAction(MS_RIGHT, LeftHand_On, "Sword");
	}
	else if (g_isLeft && !g_isRight) {
		if (m_bTakePickaxe)
			HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Pickaxe");
		else if (m_bTakeSword)
			HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Sword");
	}*/
}

void HelloWorld::KeyAction(_moveState state, _HandState handstate, std::string Item) {
	Player->setMoveState(state);
	if (m_bTakePickaxe) {
		wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
		Player->getCharactor_Hand()->AnimationState(Item, handstate, wlayer, g_bViewing);
		m_PlayerSprite = Player->getCharactor_Hand()->getSprite();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
		m_PlayerSprite->setFlippedX(true);
	}
	else if (m_bTakeSword) {
		wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
		Player->getCharactor_Hand()->AnimationState(Item, handstate, wlayer, g_bViewing);
		m_PlayerSprite = Player->getCharactor_Hand()->getSprite();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
		m_PlayerSprite->setFlippedX(true);
	}
	else if (m_bTakeGun) {
		wlayer->removeChild(Player->getCharactor_Hand()->getSprite(), true);
		Player->getCharactor_Hand()->AnimationState(Item, handstate, wlayer, g_bViewing);
		m_PlayerSprite = Player->getCharactor_Hand()->getSprite();
		m_PlayerSprite->setFlippedX(true);
	}
	else {
		if (state == MS_GET) 
			state = MS_STOP;
		wlayer->removeChild(m_PlayerSprite);					// 현재 body에 적용된 Sprite 지움
		Player->AnimationState(state, wlayer);		// 다시 body에 movestate 상태에 따라 애니메이션을 만들어 넣어줌
		m_PlayerSprite = Player->getPlayer();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
		m_PlayerSprite->setFlippedX(true);
	}
	if (g_isLeft || !g_bViewing) m_PlayerSprite->setFlippedX(true);
	else if (g_isRight || g_bViewing) m_PlayerSprite->setFlippedX(false);
}




void HelloWorld::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
	Scene::draw(renderer, transform, flags);

	glLineWidth(2);
	ccDrawColor4F(255, 0, 0, 255);

	std::list<Bomb>::iterator iter;
	if (Bombs != nullptr) {
		for (iter = Bombs->begin(); iter != Bombs->end(); ++iter) {
			ccDrawRect(ccp(m_aabb.lowerBound.x * 32, m_aabb.lowerBound.y * 32), ccp(m_aabb.upperBound.x * 32, m_aabb.upperBound.y * 32));
		}
	}
	
	std::list<Bullet>::iterator biter;
	if (Bullets.size() > 0) {
		for (biter = Bullets.begin(); biter != Bullets.end(); ++biter) {
			ccDrawRect(ccp(m_aabbb.lowerBound.x * 32, m_aabbb.lowerBound.y * 32), ccp(m_aabbb.upperBound.x * 32, m_aabbb.upperBound.y * 32));
		}
	}

//	ccDrawRect(ccp(m_BrokenAABB.lowerBound.x * 32, m_BrokenAABB.lowerBound.y * 32), ccp(m_BrokenAABB.upperBound.x * 32, m_BrokenAABB.upperBound.y * 32));

	_customCmd.init(3, transform, flags);
	_customCmd.func = CC_CALLBACK_0(HelloWorld::onDraw, this, transform, flags);

	renderer->addCommand(&_customCmd);
}

void HelloWorld::onDraw(const Mat4 &transform, uint32_t flags) {
	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when seting matrix stack");
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION);
	//_world->DrawDebugData();
	CHECK_GL_ERROR_DEBUG();

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}






CMyContactListener::CMyContactListener() {
	gNumFootContacts = 0;
}

CMyContactListener::~CMyContactListener() {

}


void CMyContactListener::BeginContact(b2Contact* contact) {
	// 3 = 캐릭터 발
	// 4 = 캐릭터 좌우
	// 5 = 장애물
	// 6 = 아이템
	// 7 = pickaxe
	// 8 = monster
	// 9 = bomb

	UINT16 Contact_Object_A = contact->GetFixtureA()->GetFilterData().categoryBits;
	UINT16 Contact_Object_B = contact->GetFixtureB()->GetFilterData().categoryBits;

	void* fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 3) {			// 발이
		if (Contact_Object_B == 0x0010)			// 몬스터 머리위에 닿았을때.
			gNumFootAttackContacts++;
		if (Contact_Object_B == 0x3000)			// item상태의 폭탄에 닿았을 때.
			gNumBombContacts++;

		gNumFootContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 3) {
		if (Contact_Object_A == 0x0010)
			gNumFootAttackContacts++;
		if (Contact_Object_A == 0x3000)
			gNumBombContacts++;

		gNumFootContacts++;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 4) {		// 캐릭터 좌우 body가
		if (Contact_Object_B == 0x0010)		// 몬스터에 닿았을 경우
			gNumMonsterContacts++;
		if (Contact_Object_B == 0x3000)		// item상태의 폭탄에 닿았을때
			gNumBombContacts++;
		if (Contact_Object_B == 0x0003) {	// 폭탄파편에 닿았을 때
			gNumChipToCharactor++;
			gNumChipToCharactor++;
		}

		gNumRightContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 4) {
		if (Contact_Object_A == 0x0010)
			gNumMonsterContacts++;
		if (Contact_Object_A == 0x3000)
			gNumBombContacts++;
		if (Contact_Object_A == 0x0003) {	// 폭탄파편에 닿았을 때
			gNumChipToCharactor++;
			gNumChipToCharactor++;
		}

		gNumRightContacts++;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 5) {
		gNumObstacleContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 5) {
		gNumObstacleContacts++;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 6) {
		gNumItemContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 6) {
		gNumItemContacts++;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 7) {
		if(Contact_Object_B == 2)
			gNumpickaxeContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 7) {
		if (Contact_Object_A == 2)
			gNumpickaxeContacts++;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 8) {
		if (Contact_Object_B == 0x0002) { // categoryBits 가 기본 블럭일 시 충돌연산은 되지만 충돌한 것으로 치지 않기 위해(블럭이 아닌 다른 오브젝트와 충돌을 위해)
			gNumMonsterFootContacts++;
		}
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 8) {
		if (Contact_Object_A == 0x0002) {
			gNumMonsterFootContacts++;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 9) {
		if (Contact_Object_B == 2)
			gNumBombContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 9) {
		if (Contact_Object_A == 2) {
			gNumBombContacts++;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 10) {
		if (Contact_Object_B == 0x0002)
			gNumFrogFootContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 10) {
		if (Contact_Object_A == 0x0002) {
			gNumFrogFootContacts++;
		}
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 11) {
		if (Contact_Object_B == 2)
			gNumSwordContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 11) {
		if (Contact_Object_A == 2) {
			gNumSwordContacts++;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 12) {
		if (Contact_Object_B == 2)
			gNumGunContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 12) {
		if (Contact_Object_A == 2) {
			gNumGunContacts++;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 13) {
		if (Contact_Object_B == 2)
			gNumLadderContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 13) {
		if (Contact_Object_A == 2) {
			gNumLadderContacts++;
		}
	}


	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 15) {
		if ( Contact_Object_B == 3 || Contact_Object_B == 4){}
		//	gNumChipToCharactor++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 15) { // 폭탄 파편과
		if (  Contact_Object_A == 3 || Contact_Object_A == 4) {// 캐릭터의 충돌
		//	gNumChipToCharactor++;
		}
	}
}

void CMyContactListener::EndContact(b2Contact* contact) {
	UINT16 Contact_Object_A = contact->GetFixtureA()->GetFilterData().categoryBits;
	UINT16 Contact_Object_B = contact->GetFixtureB()->GetFilterData().categoryBits;

	void* fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 3) {
		if (Contact_Object_B == 0x0010) 
			gNumFootAttackContacts--;
		if (Contact_Object_B == 0x3000)			
			gNumBombContacts--;

		gNumFootContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 3) {
		if (Contact_Object_A == 0x0010) 
			gNumFootAttackContacts--;
		if (Contact_Object_A == 0x3000)
			gNumBombContacts--;

		gNumFootContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 4) {
		if (Contact_Object_B == 0x0010)
			gNumMonsterContacts--;
		if (Contact_Object_B == 0x3000)
			gNumBombContacts--;
		if (Contact_Object_B == 0x0003)		// 폭탄파편에 닿았을 때
			gNumChipToCharactor--;

		gNumRightContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 4) {
		if (Contact_Object_A == 0x0010)
			gNumMonsterContacts--;
		if (Contact_Object_A == 0x3000)
			gNumBombContacts--;
		if (Contact_Object_A == 0x0003)		// 폭탄파편에 닿았을 때
			gNumChipToCharactor--;

		gNumRightContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 5) {
		gNumObstacleContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 5) {
		gNumObstacleContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 6) {
		gNumItemContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 6) {
		gNumItemContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 7) {
		if (Contact_Object_B == 2)
			gNumpickaxeContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 7) {
		if (Contact_Object_A == 2)
			gNumpickaxeContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 8) {
		if (Contact_Object_B == 0x0002) {
			gNumMonsterFootContacts--;
		}
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 8) {
		if (Contact_Object_A == 0x0002) {
			gNumMonsterFootContacts--;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 9) {
		if (Contact_Object_B == 2)
		gNumBombContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 9) {
		if (Contact_Object_A == 2)
		gNumBombContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 10) {
		if (Contact_Object_B == 0x0002)
			gNumFrogFootContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 10) {
		if (Contact_Object_A == 0x0002) {
			gNumFrogFootContacts--;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 11) {
		if (Contact_Object_B == 2)
			gNumSwordContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 11) {
		if (Contact_Object_A == 2) {
			gNumSwordContacts--;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 12) {
		if (Contact_Object_B == 2)
			gNumGunContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 12) {
		if (Contact_Object_A == 2) {
			gNumGunContacts--;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 13) {
		if (Contact_Object_B == 2)
			gNumLadderContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 13) {
		if (Contact_Object_A == 2) {
			gNumLadderContacts--;
		}
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 15) {
		if (Contact_Object_B == 1 || Contact_Object_B == 3 || Contact_Object_B == 4) {}
		//	gNumChipToCharactor--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 15) {
		if (Contact_Object_A == 1 || Contact_Object_A == 3 || Contact_Object_A == 4){
		//	gNumChipToCharactor--;
		}
	}
}


// Game Over Scene으로 전환
void HelloWorld::GameOver(Ref* pSender) {
	auto pScene = GameOverScene::createScene();
	Director::getInstance()->replaceScene(pScene);
}