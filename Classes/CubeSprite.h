#pragma once
#ifndef __CUBE_SPRITE_H__
#define __CUBE_SPRITE_H__

#include "cocos2d.h"

USING_NS_CC;

#define TOTAL_CUBE (4)
#define EMPTY (-1)

static const char* cubeSprites[TOTAL_CUBE] =
{
	"cube1.png",
	"cube2.png",
	"cube3.png",
	"cube4.png"
};

class CubeSprite : public Sprite
{
public:
	CubeSprite();
	static float getCubeLength();

	CC_SYNTHESIZE(int, _row, Row);
	CC_SYNTHESIZE(int, _col, Col);
	CC_SYNTHESIZE(int, _imgIndex, ImgIndex);

};
#endif // !__CUBE_SPRITE_H__
