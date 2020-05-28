#include "CubeSprite.h"

static const char* cubeSprites[TOTAL_CUBE] =
{
	"cube1.png",
	"cube2.png",
	"cube3.png",
	"cube4.png"
};

CubeSprite::CubeSprite()
{
	_row = 0;
	_col = 0;
	_imgIndex = EMPTY;
}

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in CubeSprites.cpp\n");
}

CubeSprite* CubeSprite::create(int row, int col, int index)
{
	CubeSprite* cube = new CubeSprite;
	cube->_row = row;
	cube->_col = col;
	cube->_imgIndex = index;
	cube->initWithSpriteFrameName(cubeSprites[index]);

	cube->autorelease();

	return cube;
}

float CubeSprite::getCubeLength()
{
	static float length = 0;

	if (length == 0)
	{
		auto sprite = Sprite::create(cubeSprites[0]);
		if (!sprite) {
			problemLoading("cube1.png");
		}
		length = sprite->getContentSize().width;
		//delete sprite;
	}

	return length;
}