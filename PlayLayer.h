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

	static PlayLayer* create(int& level);
	virtual bool init(int& level);
	virtual void update(float dt) override;
	virtual bool onTouchBegan(Touch* touch, Event* event) override;
	virtual void onTouchMoved(Touch* touch, Event* event) override;
	CC_SYNTHESIZE(bool, _isEnd, IsEnd);
	CC_SYNTHESIZE(std::string, _endType, EndType);
	CC_SYNTHESIZE(int, _points, Points);

	//CREATE_FUNC(PlayLayer);
private:
	SpriteFrameCache* spriteSheet;
	//int** _boardOfIndex;
	std::vector<std::vector<int> >* _boardOfIndex;
	//std::vector<std::vector<int> >::iterator _rBoardIndex;
	//std::vector<int>::iterator _cBoardIndex;
	//CubeSprite*** _boardOfCubes;
	std::vector<std::vector<CubeSprite*> >* _boardOfCubes;
	//std::vector<std::vector<CubeSprite*> > ::iterator _rc;
	//std::vector<CubeSprite*>::iterator _cc;
	int _numBoardCols;
	int _numBoardRows;
	float _boardLeftTopX;
	float _boardLeftTopY;
	bool _isTouchEnable;
	CubeSprite* _srcCube;
	CubeSprite* _destCube;
	bool _isActing;
	bool _needFillin;
	Label* _labelPointStr;
	Label* _labelPointNum;
	int _steps;
	Label* _labelStepStr;
	Label* _labelStepNum;
	int _level;

	void generateInitBoardIndex(std::vector<std::vector<int> >* board);
	void dropCube(CubeSprite* cube);
	void fillinEmpties();
	void checkAndClear();
	CubeSprite* cubeOfPoint(Vec2* point);
	void swapCubes();
	bool swappable(std::vector<std::vector<int> >* board);
	int indexExcept(int given);
	int indexExcept(int given1, int given2);

	void initBoardOfIndex();
	Vec2 winPositionOfCube(int row, int col);
	void checkAndClearBoardOfIndex();
	//void checkAndClearBoardOfIndex(int mode);
	void clearBoardOfIndexRow(int row, int col, int left, int right);
	void clearBoardOfIndexCol(int row, int col, int up, int down);
	void clearBoardOfIndexCol(int col);
	void clearBoardOfIndexRow(int row);
	int numOfCubesLeftChain(int row, int col, std::vector<std::vector<int> >* board);
	int numOfCubesRightChain(int row, int col, std::vector<std::vector<int> >* board);
	int numOfCubesUpChain(int row, int col, std::vector<std::vector<int> >* board);
	int numOfCubesDownChain(int row, int col, std::vector<std::vector<int> >* board);

	void initBoardOfCubes();
	void clearBoardOfCubes();
};
#endif // !__PLAY_LAYER_H__
