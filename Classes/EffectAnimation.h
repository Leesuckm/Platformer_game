#ifndef __EFFECT_ANIMATION_H__
#define __EFFECT_ANIMATION_H__
#pragma once
#include "HelloWorldScene.h"

class EffectAnimation {
private:
	std::list<Effect>* Gun_Crash_Effects;
	cocos2d::Texture2D* m_texture;
	Layer* m_layer;

public:
	EffectAnimation();
	~EffectAnimation();

	Sprite* CreateEffect(Vec2 position, float animation_time, Layer* layer);
	
	std::list<Effect>* getGunEffects() { return Gun_Crash_Effects; };
};
#endif __EFFECT_ANIMATION_H__