#pragma once
#ifndef __PLAY_LAYER_H__
#define __PLAY_LAYER_H__

#include "cocos2d.h"

USING_NS_CC;

class CubeSprite;

class PlayLayer :public Layer
{
public:
	PlayLayer();
	~PlayLayer();

	virtual bool init();
	virtual void update(float dt);
	//virtual bool onTouchBegan(Touch* touch, Event* unused);
	//virtual void onTouchMoved(Touch* touch, Event* unused);

	CREATE_FUNC(PlayLayer);
private:
	SpriteFrameCache* spriteSheet;
	int** _boardOfIndex;
	CubeSprite*** _boardOfCubes;
	int _numBoardCols;
	int _numBoardRows;
	float _boardLeftTopX;
	float _boardLeftTopY;
	bool _isTouchEnable;
	CubeSprite* _srcCube;
	CubeSprite* _destCube;
	bool _isActing;
	bool _needFillin;

	void dropCube(int row, int col, CubeSprite* cube);
	void fillinEmpties();
	void checkAndClear();

	void initBoardOfIndex();
	Vec2 winPositionOfCube(int row, int col);
	void checkAndClearBoardOfIndex();
	void clearBoardOfIndexRowAndCol(int row, int col, int left, int right, int up, int down);
	void clearBoardOfIndexRow(int row, int col, int left, int right);
	void clearBoardOfIndexCol(int row, int col, int up, int down);
	int numOfCubesLeftChain(int row, int col);
	int numOfCubesRightChain(int row, int col);
	int numOfCubesUpChain(int row, int col);
	int numOfCubesDownChain(int row, int col);
	//void dropExistOnesInIndexBoard();
	//void dropNewOnesInIndexBoard();

	void initBoardOfCubes();
	void clearBoardOfCubes();
	//int dropExistOnesInCubesBoard();
	//void dropNewOnesInCubesBoard(int numNewCubes);
};
#endif // !__PLAY_LAYER_H__
