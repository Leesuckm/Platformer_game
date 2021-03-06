#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#pragma execution_character_set("utf-8")
#pragma once
#endif

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "CaveCreator.h"
#include "TiledBodyCreator.h"
//#include "CharactorCreator.h"
#include "GameOverScene.h"
#include <GLES-Render.h>
#include <random>

#define Heart_Count 15
#define Bomb_Count 10
#define Ladder_Count 3

#define DEGTORAD 3.14 / 180

using namespace cocos2d;

static int gNumFootContacts;
static int gNumRightContacts;
static int gNumObstacleContacts;
static int gNumItemContacts;
static int gNumpickaxeContacts;
static int gNumMonsterContacts;
static int gNumMonsterFootContacts;
static int gNumFrogFootContacts;
static int gNumFootAttackContacts;
static int gNumBombContacts;
static int gNumSwordContacts;
static int gNumGunContacts;
static int gNumLadderContacts;
static int gNumLadderUpDown;

static int gNumChipToCharactor;
static int gNumChipToItem;
static int gNumChipToPickaxe;
static int gNumChipToMonster;
static int gNumChipToBomb;

static int gHeartCount;
static int gBombCount;
static int gLadderCount;
static bool g_bViewing;

static bool g_isLeft;
static bool g_isStop;
static bool g_isRight;
static bool g_isUp;
static bool g_isDown;
static bool g_isLadderStop;
static bool g_isJump;
static bool g_isRightJump;
static bool g_isLeftJump;
static bool g_isThrow;
static bool g_isAttackOn;
static bool g_ATTACK;

enum _moveState {
	MS_STOP,
	MS_LEFT,
	MS_RIGHT,
	MS_UPDOWN,
	MS_JUMP,
	MS_LEFTJUMP,
	MS_RIGHTJUMP,
	MS_GET,
	MS_DEATH,
	MS_PAIN,
	MS_JUMPUP,
	MS_JUMPDOWN,
};

enum _HandState;

struct Monster {
	b2Body* body;
	b2Body* recognize_body;
	b2Vec2 before_position;
	_moveState st;
	_moveState state;
	int before_hp;
	float time = 0;
	float jump_CT = 0;
	float shooting_Delay = 0;
	std::string obj_name;
	Monster(std::string _obj_name, b2Body* _body, b2Body* _recognize_body, _moveState _state, int _before_hp, int _time) {
		obj_name = _obj_name;
		body = _body;
		recognize_body = _recognize_body;
		state = _state;
		before_hp = _before_hp;
		time = _time;
	}
	float dt = 0;
};

struct Monster_Component {
	b2Body* body;
	float time;
	Monster_Component(b2Body* _body, float _time) {
		body = _body;
		time = _time;
	}
};

struct Bomb {
	b2Body* body;
	float time;
	b2Vec2 body_pos;
	b2AABB aabb;
	Bomb(b2Body* _body, float _time, b2Vec2 _body_pos, b2AABB _aabb) {
		body = _body;
		time = _time;
		body_pos = _body_pos;
		aabb = _aabb;
	}
};

struct Bullet {
	b2Body* body;
	b2Vec2 body_pos;
	b2AABB aabb;
	Bullet(b2Body* _body, b2Vec2 _body_pos, b2AABB _aabb) {
		body = _body;
		body_pos = _body_pos;
		aabb = _aabb;
	}
};

struct BombChip{
	b2Body* body;
	bool power;
	BombChip(b2Body* _body, bool _power) {
		body = _body;
		power = _power;
	}
};

struct Effect {
	Sprite* EffectSprite;
	float animation_time;
	Effect(Sprite* _EffectSprite, float _animation_time) {
		EffectSprite = _EffectSprite;
		animation_time = _animation_time;
	}
};


class CharactorCreator;
class MonsterCreator;
class Opossum;
class Frog;
class Bat;
class Explosion;
class EffectAnimation;

class MyQueryCallback : public b2QueryCallback {
public:
	std::vector<b2Body*> foundBodies;

	bool ReportFixture(b2Fixture* fixture) {
		foundBodies.push_back(fixture->GetBody());
		return true;
	}
};

static b2AABB m_BrokenAABB;
static b2World* _world;

class DLayer : public cocos2d::Layer {
public:
	CREATE_FUNC(DLayer);
	cocos2d::CustomCommand _customCmd;
	virtual bool init() {
		cocos2d::Layer::init();;
		return true;
	};

	virtual	void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4 &transform, uint32_t flags) override {
		cocos2d::Layer::draw(renderer, transform, flags);

		_customCmd.init(3, transform, flags);
		_customCmd.func = CC_CALLBACK_0(DLayer::onDraw, this, transform, flags);
		renderer->addCommand(&_customCmd);

		ccDrawRect(ccp(m_BrokenAABB.lowerBound.x * 32, m_BrokenAABB.lowerBound.y * 32), ccp(m_BrokenAABB.upperBound.x * 32, m_BrokenAABB.upperBound.y * 32));
	}

	void onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
	{
		cocos2d::Director* director = cocos2d::Director::getInstance();
		director->pushMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		director->loadMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

		GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION);
		_world->DrawDebugData();
		CHECK_GL_ERROR_DEBUG();

		director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		// deprecated
		/*kmGLPushMatrix();
		kmGLLoadMatrix(&transform);*/

		//add your primitive drawing code here
	//	cocos2d::DrawPrimitives::drawLine(Vec2(0, 0), Vec2(100, 100));
	}

	//virtual bool onTouchBegan(Touch* touch, Event* event);
	//virtual void onTouchMoved(Touch* touch, Event* event);
	//virtual void onTouchEnded(Touch* touch, Event* event);
};

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	DLayer* wlayer;
	Layer* m_clayer;
	
	cocos2d::Size winSize;
	cocos2d::Texture2D* texture;
//	b2World* _world;
	TMXTiledMap* map;
	Vec2 CharactorPoint;
	CaveCreator First_Floor;
	TiledBodyCreator Map_Body;

	Menu* m_pAttackButton;
	MenuItemImage* m_pAttackButton_Images;

	Menu* m_pLadderButton;
	MenuItemImage* m_pLadderButton_Images;

	Label* m_HeartLabel;
	std::string m_Heart;

	Label* m_BombLabel;
	std::string m_Bomb;

	GLESDebugDraw* m_debugDraw;
	cocos2d::CustomCommand _customCmd;

	MonsterCreator* m_Monster;
	MonsterCreator* m_Opossum;
	MonsterCreator* m_Frog;
	MonsterCreator* m_Bat;

	CharactorCreator* Player;
	b2Vec2 m_Player_before_position;
	_moveState m_before_state;
	_HandState m_before_handstate;

	std::list<Monster> Monsters;
	Explosion* m_Explosion;

	std::vector<Monster_Component> *m_Monster_Component;
	std::queue<b2Body*> m_DestroyQ;

	b2Body* m_Charactor_body;
	Vec2 m_pos;
	EventListenerKeyboard* m_kListener;

	Vec2 m_old_distance = Vec2(0,0);

	Sprite* m_PlayerSprite;
	Sprite* m_BeforeSprite;
	Sprite* m_CameraSprite;
	Sprite* m_RecognizeSp;

	Sprite* m_brokenbl;
	Sprite* m_brokenbl2;

	Vec2 m_touchPoint;
	Vec2 m_touchPoint2;
	b2Vec2 m_throwpower;

	MyQueryCallback queryCallback;
	std::list<Bullet> Bullets;
	b2AABB m_aabbb;
	EffectAnimation* GunEffect;

	MyQueryCallback m_BrokenCallback;
//	b2AABB m_BrokenAABB;

	std::list<Bomb>* Bombs;
	b2AABB m_aabb;

	std::list<b2Body*> m_EnableLadder;

	float m_fDelayTime;
	float m_ftime;
	float m_fImpulse;
	float m_fAttackCooltime = 0;
	float m_fAttackDelaytime = 0;
	float m_fLadderCooltime = 0;

	float m_fMovetime;
	float m_fdt;

	float m_fpowertime = 0;
	float m_fcleantime = 0;

	bool m_bCan_Move;
	bool m_bCan_UpDown;
	bool m_bCan_Broken;
	bool m_bDo_Jump;
	bool m_bTouchHolding;
	bool m_bBulletcrash;

	bool m_bTakePickaxe = false;
	bool m_bTakeSword = false;
	bool m_bTakeGun = false;

	int m_nJumpStep;
	int m_jumpTimeout;

	~HelloWorld();

	void onEnter();
	void onExit();
	void tick(float dt);

	bool createBox2dWorld(bool debug);
	void CreateMap();
	void CreateCharactor();
	void CreateMonster();
	void CreateBt();

	void MonsterOnWorld(TMXObjectGroup* obj);
	void Destroy_Component(float dt);

	void GameOver(Ref* pSender);

	void AttackMonster(float dt);
	void TouchHold(float touchhold_time);

	void PickupItem(std::list<b2Body*> *item);
	void power_enegy(float power);

	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
	void onDraw(const cocos2d::Mat4& transform, uint32_t flags);

	bool onTouchBegan(Touch* touch, Event* event);
	void onTouchMoved(Touch* touch, Event* event);
	void onTouchEnded(Touch* touch, Event* event);

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void KeyAction(_moveState state, _HandState handstate, std::string Item);

	void doAction(Ref* pSender);
	void doLadderequip(Ref* pSender);
	void BulletFire();

	void drawReflectedRay(b2Vec2 p1, b2Vec2 p2);
};



class CMyContactListener : public b2ContactListener {
private:
	bool m_bContacting;
	int m_nNumFootContacts;

public:
	CMyContactListener();
	~CMyContactListener();

	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);

	void startContact() { m_bContacting = true; }
	void endContact() { m_bContacting = false; }
};

#endif // __HELLOWORLD_SCENE_H__