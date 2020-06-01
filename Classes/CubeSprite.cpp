#include "CubeSprite.h"

CubeSprite::CubeSprite()
{
	_row = 0;
	_col = 0;
	//_imgIndex = EMPTY;
}

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in CubeSprites.cpp\n");
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
		delete sprite;
	}

	return length;
}
