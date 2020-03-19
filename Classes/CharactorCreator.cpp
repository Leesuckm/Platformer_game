#include "CharactorCreator.h"
#include "Charactor_Hand.h"

CharactorCreator::CharactorCreator() {
	gNumFootContacts = 0;
	m_Charactor_hand = new Charactor_Hand();
}

CharactorCreator::~CharactorCreator() {
	delete m_Charactor_hand;
}

void CharactorCreator::CreateCharactor(b2World* _world, Vec2 position, Layer* layer) {
	m_world = _world;
	m_texture = Director::getInstance()->getTextureCache()->addImage("Images/fox_idle.png");

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.1f);

	for (int i = 0; i < 4; i++) {
		animation->addSpriteFrameWithTexture(m_texture, Rect(i * 33, 0, 33, 32));
	}

	m_sPlayer = Sprite::createWithTexture(m_texture, Rect(0, 0, 33, 32));
	m_sPlayer->setAnchorPoint(Vec2(0.5, 0.3));
	layer->addChild(m_sPlayer, 3);

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	m_sPlayer->runAction(rep);

	m_MyBodyDef.type = b2_dynamicBody;

	//b2PolygonShape polygonShape;
	polygonShape.SetAsBox(0.3f, 0.3f);

	//b2CircleShape circle;
	//circle.m_radius = 0.3f;

	//b2FixtureDef myFixtureDef;
	myFixtureDef.shape = &polygonShape; // 여기에 추가된 Shape를 써서 기존 body에 또 fixture를 넣어야만 적용됨.
	//myFixtureDef.isSensor = true;
	myFixtureDef.density = 1.0f;
	myFixtureDef.friction = 0.0f;
	myFixtureDef.restitution = 0.0;
	
	myFixtureDef.filter.categoryBits = 0x0001;
	myFixtureDef.filter.maskBits = 0xFFFF;

	m_MyBodyDef.position.Set(7, 7);
	m_MyBodyDef.userData = m_sPlayer;

	m_Charactorbody = _world->CreateBody(&m_MyBodyDef);
	m_Charactorbody->CreateFixture(&myFixtureDef);

	m_Charactorbody->SetIsCh(true);

	b2Fixture* SensorFixture = m_Charactorbody->CreateFixture(&myFixtureDef);
	SensorFixture->SetUserData((void*)1);

	b2Vec2 footvertices[3];
	footvertices[0] = b2Vec2(0, -0.4f);
	footvertices[1] = b2Vec2(0.3f, -0.35f);
	footvertices[2] = b2Vec2(-0.3f, -0.35f);

	b2Vec2 rightvertices[3];
	rightvertices[0] = b2Vec2(0.4f, 0);
	rightvertices[1] = b2Vec2(0.35f, 0.3f);
	rightvertices[2] = b2Vec2(0.35f, -0.3f);

	b2Vec2 leftvertices[3];
	leftvertices[0] = b2Vec2(-0.4f, 0);
	leftvertices[1] = b2Vec2(-0.35f, -0.3f);
	leftvertices[2] = b2Vec2(-0.35f, 0.3f);

	CharactorSensor(footvertices, 3, b2Vec2(0, -0.35f), 3, 5.0f, false); // foot (shape, position, userdata, friction, sensor)
	CharactorSensor(leftvertices, 3, b2Vec2(0.35f, 0), 4, 0.0001f, false); //right
	CharactorSensor(rightvertices, 3, b2Vec2(-0.35f, 0), 4, 0.0001f, false); // left

	m_Charactorbody->SetFixedRotation(true);

	CreateCameraSprite(_world);

	m_MoveState = MS_STOP;
	CreateHeart(Heart_Count); // 시작 생명력 Heart_Count개
	CreateBomb(Bomb_Count);
	CreateLadder(Ladder_Count);
}

void CharactorCreator::CharactorSensor(b2Vec2* vertices, int size, b2Vec2 position, int part, float fric, bool sensor) {

	
	polygonShape.Set(vertices, size);

	if (sensor) {
		polygonShape.SetAsBox(1, 1, position, 0);
	}
	myFixtureDef.isSensor = sensor;
	myFixtureDef.friction = fric;
	//myFixtureDef.filter.categoryBits = 0x0001;
	//myFixtureDef.filter.categoryBits = 0xFFFF;
	b2Fixture* footSensorFixture = m_Charactorbody->CreateFixture(&myFixtureDef);
	footSensorFixture->SetUserData((void*)part);
}

void CharactorCreator::CreateCameraSprite(b2World* _world) {
	auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Camera.png");
	Sprite* CameraSp = Sprite::createWithTexture(texture, Rect(0, 0, 37, 37));
	CameraSp->setScale(0.3f);
	CameraSp->setVisible(true);
	m_MyCameraDef.type = b2_dynamicBody;

	m_MyCameraDef.position.Set(7, 7);
	m_MyCameraDef.userData = CameraSp;

	b2CircleShape circle;
	circle.m_radius = 0.3f;

	b2FixtureDef CameraFixture;
	CameraFixture.isSensor = true;
	CameraFixture.shape = &circle;
	CameraFixture.density = 0.1f;
	CameraFixture.friction = 0.0f;
	CameraFixture.restitution = 0;

	m_Camerabody = _world->CreateBody(&m_MyCameraDef);
	m_Camerabody->CreateFixture(&CameraFixture);
}

void CharactorCreator::CreateHeart(int nHeart) {
	m_nHeartCount = nHeart;
	m_nHeartPosition = m_svHeart.size();
	auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Heart.png");
	for (int i = 0; i < m_nHeartCount; i++) {
		m_svHeart.push_back(Sprite::createWithTexture(texture, Rect(0, 0, 32, 29)));
		m_svHeart.at(m_nHeartPosition)->setPosition(Vec2(20 + m_nHeartPosition * 30, 300));
		m_nHeartPosition++;
	}
}

void CharactorCreator::CreateBomb(int nBomb) {
	m_nBombCount = nBomb;
	auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Bomb.png");
	m_sBomb = Sprite::createWithTexture(texture, Rect(0, 0, 34, 32));
	m_sBomb->setPosition(Vec2(50, 300));
}

void CharactorCreator::CreateLadder(int nLadder) {
	m_nLadderCount = nLadder;
	auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Ladder.png");
	m_sLadder = Sprite::createWithTexture(texture, Rect(0, 0, 32, 32));
	m_sLadder->setPosition(Vec2(80, 300));
}

void CharactorCreator::Heartmanager(Layer* layer, int heartcount) {
	std::string Heart = std::to_string(heartcount);
	layer->removeChild(m_HeartLabel, true);
	m_HeartLabel = Label::createWithSystemFont(Heart, "", 30);
	m_HeartLabel->setPosition(m_svHeart.at(0)->getPosition());
	m_HeartLabel->setColor(Color3B(255, 255, 255));
	layer->addChild(m_HeartLabel, 3);
}

void CharactorCreator::Bombmanager(Layer* layer, int bombcount) {
	std::string Bomb = std::to_string(bombcount);
	layer->removeChild(m_BombLabel, true);
	m_BombLabel = Label::createWithSystemFont(Bomb, "", 30);
	m_BombLabel->setPosition(m_sBomb->getPosition());
	m_BombLabel->setColor(Color3B(255, 255, 255));
	layer->addChild(m_BombLabel, 3);
}

void CharactorCreator::Laddermanager(Layer* layer, int laddercount) {
	std::string Ladder = std::to_string(laddercount);
	layer->removeChild(m_LadderLabel, true);
	m_LadderLabel = Label::createWithSystemFont(Ladder, "", 30);
	m_LadderLabel->setPosition(Vec2(80, 300));
	m_LadderLabel->setColor(Color3B(255, 255, 255));
	layer->addChild(m_LadderLabel, 3);

}


Sprite* CharactorCreator::AnimationState(_moveState state, Layer* layer) {
	int nSheetmx;
	std::string sPath;
	switch (state) {
	case MS_LEFT:
		nSheetmx = 6;
		sPath = "Images/fox_run.png";
		break;
	case MS_RIGHT:
		nSheetmx = 6;
		sPath = "Images/fox_run.png";
		break;
	case MS_STOP:
		nSheetmx = 4;
		sPath = "Images/fox_idle.png";
		break;
	case MS_UPDOWN:
		nSheetmx = 2;
		sPath = "Images/fox_climb.png";
		break;
	case MS_JUMP:
		nSheetmx = 1;
		sPath = "Images/fox_jump1.png";
		break;
	case MS_JUMPDOWN:
		nSheetmx = 1;
		sPath = "Images/fox_jump2.png";
		break;
	default:
		log("state error");
		break;
	}

	auto texture = Director::getInstance()->getTextureCache()->addImage(sPath);

	auto animation = Animation::create();
	animation->setDelayPerUnit(0.1f);
	if (state == MS_UPDOWN) animation->setDelayPerUnit(0.4f);

	for (int i = 0; i < nSheetmx; i++) {
		animation->addSpriteFrameWithTexture(texture,
			Rect(i * 33, 0, 33, 32));
	}

	m_sPlayer = Sprite::createWithTexture(texture, Rect(0, 0, 33, 32));
	m_sPlayer->setPosition(Vec2(0, 0));
	m_sPlayer->setAnchorPoint(Vec2(0.5f, 0.3f));
	layer->addChild(m_sPlayer, 3);

	auto animate = Animate::create(animation);
	auto rep = RepeatForever::create(animate);

	m_sPlayer->runAction(rep);

	m_Charactorbody->SetUserData(m_sPlayer);

	return m_sPlayer;
}

Sprite* CharactorCreator::Pickup(std::string* Item, _moveState state, Layer* layer) {
	std::string sPath;
	Sprite* SpriteData;
	if (*Item == "Pickaxe") {
		sPath = "Images/Items/fox_idle_pickaxe.png";
		layer->removeChild((Sprite*)m_Charactorbody->GetUserData(), true);
		m_Charactor_hand->Holeinone_LeftHand(sPath, m_Charactorbody, state,layer, m_world);
		SpriteData = m_Charactor_hand->AnimationState("Pickaxe", Hand_On, layer, g_bViewing);
		m_Charactor_hand->setDurability(30); // Pickaxe의 내구도
	}
	else if (*Item == "Bomb") {
		sPath = "Images/Items/fox_throw_bomb.png";
		m_Charactor_hand->Holeinone_LeftHand(sPath, m_Charactorbody, state, layer, m_world);
		SpriteData = m_Charactor_hand->AnimationState("Pickaxe", THROW_BOMB, layer, g_bViewing);
	}
	else if (*Item == "Sword") {
		sPath = "Images/Items/fox_idle_sword.png";
		layer->removeChild((Sprite*)m_Charactorbody->GetUserData(), true);
		m_Charactor_hand->Holeinone_LeftHand(sPath, m_Charactorbody, state, layer, m_world);
		SpriteData = m_Charactor_hand->AnimationState("Sword",Hand_On, layer, g_bViewing);
	}
	else if (*Item == "Gun") {
		sPath = "Images/Items/fox_idle_gun.png";
		layer->removeChild((Sprite*)m_Charactorbody->GetUserData(), true);
		m_Charactor_hand->Holeinone_LeftHand(sPath, m_Charactorbody, state, layer, m_world);
		SpriteData = m_Charactor_hand->AnimationState("Gun", Hand_On, layer, g_bViewing);
	}

	return SpriteData;
}