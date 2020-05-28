#pragma once
#ifndef __CUBE_SPRITE_H__
#define __CUBE_SPRITE_H__

#include "cocos2d.h"

USING_NS_CC;

#define TOTAL_CUBE (4)
#define EMPTY (-1)

class CubeSprite : public Sprite
{
public:
	CubeSprite();
	static CubeSprite* create(int row, int col, int index);
	static float getCubeLength();

	CC_SYNTHESIZE(int, _row, Row);
	CC_SYNTHESIZE(int, _col, Col);
	CC_SYNTHESIZE(int, _imgIndex, ImgIndex);
	//CC_SYNTHESIZE(bool, _isNeedRemove, IsNeedRemove);

};
#endif // !__CUBE_SPRITE_H__
