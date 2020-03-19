#pragma once

enum _HandState {
	Hand_On,
	LeftHand_On,
	RightHand_On,
	LeftHand_Off,
	RightHand_Off,
	TwoHand_On,
	TwoHand_Off,
	JumpHand_On,
	JumpHand_Down,
	ATTACK,
	UP_ATTACK,
	UNDER_ATTACK,
	THROW_BOMB,
};

enum _moveState;
class CharactorCreator;

class Charactor_Hand{
private:
	_HandState m_HandState;
	cocos2d::Texture2D* m_texture;
	Sprite* m_Pickup_Lefthand;
	b2Body* m_Charactorbody;
	b2Body* m_HandObjbody;
	b2Body* m_ItemObjbody;
	b2JointDef m_jointdef;
	b2World* m_world;
	Layer* m_Layer;

	int m_nDurability;

public:
	Charactor_Hand();
	~Charactor_Hand();

	void CreateHandObj(std::string sPath);

	Sprite* getSprite() { return m_Pickup_Lefthand; }

	int getDurability() { return m_nDurability; }
	_HandState getHandState() { return m_HandState; }

	void setDurability(int durability) { m_nDurability = durability; }

	void Holeinone_LeftHand(std::string sPath, b2Body* Charactorbody, _moveState state, Layer* layer, b2World* _world);
	void Holeinone_RightHand();

	Sprite* AnimationState(std::string Obj_name, _HandState state, Layer* layer, bool viewing);
};