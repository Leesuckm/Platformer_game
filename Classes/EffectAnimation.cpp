#include "EffectAnimation.h"

EffectAnimation::EffectAnimation() {
	Gun_Crash_Effects = new std::list<Effect>;
}

EffectAnimation::~EffectAnimation() {
	delete Gun_Crash_Effects;
}

Sprite* EffectAnimation::CreateEffect(Vec2 position, float animation_time, Layer* layer) {
	m_layer = layer;
	m_texture = Director::getInstance()->getTextureCache()->addImage("Images/effects/gun_crash_effect.png");

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.1f);

	for (int i = 0; i < 5; i++) {
		animation->addSpriteFrameWithTexture(m_texture, Rect(i * 20, 0, 20, 14));
	}

	Sprite* sprite = Sprite::createWithTexture(m_texture, Rect(0, 0, 20, 14));
	sprite->setPosition(position);
	sprite->setAnchorPoint(Vec2(0.5, 0.5));
	m_layer->addChild(sprite, 3);

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	sprite->runAction(rep);

	Gun_Crash_Effects->push_back(Effect(sprite, animation_time));

	return sprite;
}