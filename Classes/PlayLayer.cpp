#include "PlayLayer.h"
#include "CubeSprite.h"

#define BOARD_ROWS (5)
#define BOARD_COLS (5)

#define CUBE_GAP (0)

PlayLayer::PlayLayer()
{
	spriteSheet = NULL;
	_boardOfIndex = NULL;
	_boardOfCubes = NULL;
	_numBoardRows = 0;
	_numBoardCols = 0;
	_boardLeftTopX = 0;
	_boardLeftTopY = 0;
	_isTouchEnable = true;
	_srcCube = NULL;
	_destCube = NULL;
	_isActing = false;
	_needFillin = false;
}

PlayLayer::~PlayLayer()
{
	if (_boardOfIndex) {
		for (int r = 0; r < _numBoardRows; r++) {
			delete[] _boardOfIndex[r];
			_boardOfIndex[r] = NULL;
		}
		_boardOfIndex = NULL;
	}
	if (_boardOfCubes) {
		for (int r = 0; r < _numBoardRows; r++) {
			for (int c = 0; c < _numBoardCols; c++) {
				delete _boardOfCubes[r][c];
				_boardOfCubes[r][c] = NULL;
			}
			delete[] _boardOfCubes[r];
			_boardOfCubes[r] = NULL;
		}
		delete[] (_boardOfCubes);
		_boardOfCubes = NULL;
	}
}

bool PlayLayer::init()
{
	if (!Layer::init()) {
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//bkg and sprite list
	auto bkg = Sprite::create("bkg.png");
	float x = origin.x + visibleSize.width / 2;
	float y = origin.y + visibleSize.height / 2;
	bkg->setPosition(Vec2(x, y));
	addChild(bkg);

	spriteSheet = SpriteFrameCache::getInstance();
	spriteSheet->addSpriteFramesWithFile("cubeSprites.plist");

	_numBoardRows = BOARD_ROWS;
	_numBoardCols = BOARD_COLS;

	_boardLeftTopX = (visibleSize.width - _numBoardCols * CubeSprite::getCubeLength() - (_numBoardCols - 1) * CUBE_GAP) / 2;
	_boardLeftTopY = (visibleSize.height + _numBoardRows * CubeSprite::getCubeLength() + (_numBoardRows - 1) * CUBE_GAP) / 2;

	_boardOfIndex = new int* [_numBoardRows];
	_boardOfCubes = new CubeSprite * *[_numBoardRows];

	for (int r = 0; r < _numBoardRows; r++) {
		_boardOfIndex[r] = new int[_numBoardCols];
		_boardOfCubes[r] = new CubeSprite * [_numBoardCols];
	}
	for (int r = 0; r < _numBoardRows; r++) {
		for (int c = 0; c < _numBoardCols; c++) {
			_boardOfIndex[r][c] = EMPTY;
			_boardOfCubes[r][c] = new CubeSprite();
		}
	}

	initBoardOfIndex();
	initBoardOfCubes();
	this->scheduleUpdate();

	//event listener
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(PlayLayer::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(PlayLayer::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	return true;
}

void PlayLayer::update(float dt)
{
	if (_isActing) {
		_isActing = false;
		for (int r = 0; r < _numBoardRows; r++) {
			for (int c = 0; c < _numBoardCols; c++) {
				if (_boardOfCubes[r][c] && _boardOfCubes[r][c]->getNumberOfRunningActions() > 0) {
					_isActing = true;
					break;
				}
			}
			if (_isActing) {
				break;
			}
		}
	}

	_isTouchEnable = !_isActing;

	if (!_isActing) {
		if (_needFillin) {
			fillinEmpties();
			_needFillin = false;
		}
		else {
			checkAndClear();
		}
	}
}

void PlayLayer::initBoardOfIndex()
{
	int testBoard[5][5] = {
		{0, 1, 0, 1, 3},
		{2, 3, 2, 2, 1},
		{0, 2, 3, 0, 1},
		{3, 2, 1, 3, 0},
		{1, 3, 2, 0, 1}
	};
	for (int r = 0; r < _numBoardRows; r++) {
		for (int c = 0; c < _numBoardCols; c++) {
			_boardOfIndex[r][c] = testBoard[r][c];
		}
	}
}

void PlayLayer::initBoardOfCubes()
{
	for (int r = _numBoardRows - 1; r >= 0; r--) {
		for (int c = 0; c < _numBoardCols; c++) {
			_boardOfCubes[r][c]->setRow(r);
			_boardOfCubes[r][c]->setCol(c);
			_boardOfCubes[r][c]->setImgIndex(_boardOfIndex[r][c]);
			_boardOfCubes[r][c]->initWithSpriteFrameName(cubeSprites[_boardOfIndex[r][c]]);
			_boardOfCubes[r][c]->setPosition(winPositionOfCube(r, c));

			this->addChild(_boardOfCubes[r][c]);
			dropCube(r, c, _boardOfCubes[r][c]);
			
		}
	}
}

void PlayLayer::dropCube(int row, int col, CubeSprite* cube)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	Vec2 endPosition = winPositionOfCube(row, col);
	Vec2 startPosition = Vec2(endPosition.x, endPosition.y + visibleSize.height / 2);
	float time = startPosition.y / (2 * visibleSize.height);

	cube->setPosition(startPosition);
	cube->runAction(MoveTo::create(time, endPosition));
}

Vec2 PlayLayer::winPositionOfCube(int row, int col)
{
	float x = _boardLeftTopX + col * (CubeSprite::getCubeLength() + CUBE_GAP) + CubeSprite::getCubeLength() / 2;
	float y = _boardLeftTopY - row * (CubeSprite::getCubeLength() + CUBE_GAP) - CubeSprite::getCubeLength() / 2;

	return Vec2(x, y);
}

void PlayLayer::checkAndClear()
{
	checkAndClearBoardOfIndex();
	clearBoardOfCubes();
}

void PlayLayer::checkAndClearBoardOfIndex()
{
	int** leftCubesOfCube = new int* [_numBoardRows];
	int** rightCubesOfCube = new int* [_numBoardRows];
	int** upCubesOfCube = new int* [_numBoardRows];
	int** downCubesOfCube = new int* [_numBoardRows];

	for (int r = 0; r < _numBoardRows; r++) {
		leftCubesOfCube[r] = new int[_numBoardCols] {0};
		rightCubesOfCube[r] = new int[_numBoardCols] {0};
		upCubesOfCube[r] = new int[_numBoardCols] {0};
		downCubesOfCube[r] = new int[_numBoardCols] {0};
	}

	for (int r = 0; r < _numBoardRows; r++) {
		for (int c = 0; c < _numBoardCols; c++) {
			if (_boardOfIndex[r][c] == EMPTY) {
				continue;
			}
			leftCubesOfCube[r][c] = numOfCubesLeftChain(r, c);
			rightCubesOfCube[r][c] = numOfCubesRightChain(r, c);
			upCubesOfCube[r][c] = numOfCubesUpChain(r, c);
			downCubesOfCube[r][c] = numOfCubesDownChain(r, c);
		}
	}

	for (int r = 0; r < _numBoardRows; r++) {
		for (int c = 0; c < _numBoardCols; c++) {
			if (leftCubesOfCube[r][c] + rightCubesOfCube[r][c] >= 2) {
				clearBoardOfIndexRow(r, c, leftCubesOfCube[r][c], rightCubesOfCube[r][c]);
			}
			if (upCubesOfCube[r][c] + downCubesOfCube[r][c] >= 2) {
				clearBoardOfIndexCol(r, c, upCubesOfCube[r][c], downCubesOfCube[r][c]);
			}
		}
	}

	for (int r = 0; r < _numBoardRows; r++) {
		delete[] leftCubesOfCube[r];
		delete[] rightCubesOfCube[r];
		delete[] upCubesOfCube[r];
		delete[] downCubesOfCube[r];

		leftCubesOfCube[r] = NULL;
		rightCubesOfCube[r] = NULL;
		upCubesOfCube[r] = NULL;
		downCubesOfCube[r] = NULL;
	}

	delete[] leftCubesOfCube;
	delete[] rightCubesOfCube;
	delete[] upCubesOfCube;
	delete[] downCubesOfCube;

	leftCubesOfCube = NULL;
	rightCubesOfCube = NULL;
	upCubesOfCube = NULL;
	downCubesOfCube = NULL;
}

void PlayLayer::clearBoardOfCubes()
{
	for (int r = 0; r < _numBoardRows; r++) {
		for (int c = 0; c < _numBoardCols; c++) {
			if (_boardOfIndex[r][c] == EMPTY) {
				_needFillin = true;
				if (_boardOfCubes[r][c]) {
					_boardOfCubes[r][c]->runAction(FadeOut::create(1.0f));
					_boardOfCubes[r][c]->removeFromParent();
				}
				delete _boardOfCubes[r][c];
				_boardOfCubes[r][c] = NULL;
			}
		}
	}
}

int PlayLayer::numOfCubesLeftChain(int row, int col)
{
	int numOfCubes = 0;
	int leftNeighbourCol = col - 1;
	while (leftNeighbourCol >= 0) {
		if (_boardOfIndex[row][leftNeighbourCol] == _boardOfIndex[row][col]) {
			numOfCubes++;
		}
		else {
			break;
		}
		leftNeighbourCol--;
	}
	return numOfCubes;
}

int PlayLayer::numOfCubesRightChain(int row, int col)
{
	int numOfCubes = 0;
	int rightNeighbourCol = col + 1;
	while (rightNeighbourCol < _numBoardCols) {
		if (_boardOfIndex[row][rightNeighbourCol] == _boardOfIndex[row][col]) {
			numOfCubes++;
		}
		else {
			break;
		}
		rightNeighbourCol++;
	}
	return numOfCubes;
}

int PlayLayer::numOfCubesUpChain(int row, int col)
{
	int numOfCubes = 0;
	int upNeighbourRow = row - 1;
	while (upNeighbourRow >= 0) {
		if (_boardOfIndex[upNeighbourRow][col] == _boardOfIndex[row][col]) {
			numOfCubes++;
		}
		else {
			break;
		}
		upNeighbourRow--;
	}
	return numOfCubes;
}

int PlayLayer::numOfCubesDownChain(int row, int col)
{
	int numOfCubes = 0;
	int downNeighbourRow = row + 1;
	while (downNeighbourRow < _numBoardRows) {
		if (_boardOfIndex[downNeighbourRow][col] == _boardOfIndex[row][col]) {
			numOfCubes++;
		}
		else {
			break;
		}
		downNeighbourRow++;
	}
	return numOfCubes;
}

void PlayLayer::clearBoardOfIndexRow(int row, int col, int left, int right)
{
	for (int r = row, c = col - left; c <= col + right; c++) {
		_boardOfIndex[r][c] = EMPTY;
	}
}

void PlayLayer::clearBoardOfIndexCol(int row, int col, int up, int down)
{
	for (int r = row - up, c = col; r <= row + down; r++) {
		_boardOfIndex[r][c] = EMPTY;
	}
}

void PlayLayer::fillinEmpties()
{
	_isActing = true;
	auto visibleSize = Director::getInstance()->getVisibleSize();
	int* numOfEmptyInCol = new int[_numBoardCols] {0};

	for (int c = 0; c < _numBoardCols; c++) {
		int removedCubes = 0;
		for (int r = _numBoardRows - 1; r >= 0; r--) {
			if (_boardOfIndex[r][c] == EMPTY) {
				removedCubes++;
			}
			else {
				if (removedCubes > 0) {
					int newRow = r + removedCubes;
					_boardOfIndex[newRow][c] = _boardOfIndex[r][c];
					_boardOfIndex[r][c] = EMPTY;

					Vec2 startPosition = winPositionOfCube(r, c);
					Vec2 endPositon = winPositionOfCube(newRow, c);
					float time = (startPosition.y - endPositon.y) / (1.5 * visibleSize.height);

					_boardOfCubes[newRow][c] = _boardOfCubes[r][c];
					_boardOfCubes[r][c] = NULL;

					_boardOfCubes[newRow][c]->stopAllActions();
					_boardOfCubes[newRow][c]->runAction(MoveTo::create(time, endPositon));
					_boardOfCubes[newRow][c]->setRow(newRow);
				}
			}
		}
		numOfEmptyInCol[c] = removedCubes;
	}
	
	for (int c = 0; c < _numBoardCols; c++) {
		if (numOfEmptyInCol[c] > 0) {
			for (int r = numOfEmptyInCol[c] - 1; r >= 0; r--) {
				_boardOfIndex[r][c] = random() % TOTAL_CUBE;

				_boardOfCubes[r][c] = new CubeSprite();
				_boardOfCubes[r][c]->setRow(r);
				_boardOfCubes[r][c]->setCol(c);
				_boardOfCubes[r][c]->setImgIndex(_boardOfIndex[r][c]);
				_boardOfCubes[r][c]->initWithSpriteFrameName(cubeSprites[_boardOfIndex[r][c]]);

				this->addChild(_boardOfCubes[r][c]);
				dropCube(r, c, _boardOfCubes[r][c]);
			}
		}
	}
	
	delete[] numOfEmptyInCol;
	numOfEmptyInCol = NULL;
}

bool PlayLayer::onTouchBegan(Touch* touch, Event* unused)
{
	_srcCube = NULL;
	_destCube = NULL;
	if (_isTouchEnable) {
		auto touchLocation = touch->getLocation();
		_srcCube = cubeOfPoint(&touchLocation);
	}
	return _isTouchEnable;
}

void PlayLayer::onTouchMoved(Touch* touch, Event* unused)
{
	if (!_isTouchEnable || _srcCube == NULL) {
		return;
	}

	int row = _srcCube->getRow();
	int col = _srcCube->getCol();
	auto touchLocation = touch->getLocation();
	int halfWidth = _srcCube->getContentSize().width / 2;
	int halfHeight = _srcCube->getContentSize().height / 2;
	auto size = _srcCube->getContentSize();

	Rect leftRect = Rect(_srcCube->getPositionX() - 3 * halfWidth, _srcCube->getPositionY() - halfHeight, 2 * halfWidth, 2 * halfHeight);
	Rect rightRect = Rect(_srcCube->getPositionX() + halfWidth, _srcCube->getPositionY() - halfHeight, 2 * halfWidth, 2 * halfHeight);
	Rect upRect = Rect(_srcCube->getPositionX() - halfWidth, _srcCube->getPositionY() + halfHeight, 2 * halfWidth, 2 * halfHeight);
	Rect downRect = Rect(_srcCube->getPositionX() - halfWidth, _srcCube->getPositionY() - 3 * halfHeight, 2 * halfWidth, 2 * halfHeight);

	if (leftRect.containsPoint(touchLocation)) {
		col--;
		if (col >= 0) {
			_destCube = _boardOfCubes[row][col];
		}
		swapCubes();
		return;
	}
	
	if (rightRect.containsPoint(touchLocation)) {
		col++;
		if (col < _numBoardCols) {
			_destCube = _boardOfCubes[row][col];
		}
		swapCubes();
		return;
	}

	if (upRect.containsPoint(touchLocation)) {
		row--;
		if (row >= 0) {
			_destCube = _boardOfCubes[row][col];
		}
		swapCubes();
		return;
	}

	if (downRect.containsPoint(touchLocation)) {
		row++;
		if (row < _numBoardRows) {
			_destCube = _boardOfCubes[row][col];
		}
		swapCubes();
		return;
	}
}

CubeSprite* PlayLayer::cubeOfPoint(Vec2* point)
{
	Rect cubeZone = Rect(0, 0, 0, 0);

	for (int r = 0; r < _numBoardRows; r++) {
		for (int c = 0; c < _numBoardCols; c++) {
			if (_boardOfCubes[r][c] != NULL) {
				cubeZone.origin.x = _boardOfCubes[r][c]->getPositionX() - _boardOfCubes[r][c]->getContentSize().width / 2;
				cubeZone.origin.y = _boardOfCubes[r][c]->getPositionY() - _boardOfCubes[r][c]->getContentSize().height / 2;
				cubeZone.size = _boardOfCubes[r][c]->getContentSize();
				if (cubeZone.containsPoint(*point)) {
					return _boardOfCubes[r][c];
				}
			}
		}
	}

	return NULL;
}

void PlayLayer::swapCubes()
{
	_isTouchEnable = false;
	_isActing = true;
	if (_srcCube == NULL || _destCube == NULL) {
		return;
	}

	Vec2 positonOfSrc = _srcCube->getPosition();
	Vec2 positonOfDest = _destCube->getPosition();
	int srcRow = _srcCube->getRow();
	int srcCol = _srcCube->getCol();
	int destRow = _destCube->getRow();
	int destCol = _destCube->getCol();
	float time = 0.2;

	int tmp = _boardOfIndex[srcRow][srcCol];
	_boardOfIndex[srcRow][srcCol] = _boardOfIndex[destRow][destCol];
	_boardOfIndex[destRow][destCol] = tmp;

	if  ((numOfCubesLeftChain(srcRow, srcCol) + numOfCubesRightChain(srcRow, srcCol) >= 2) ||
		((numOfCubesUpChain(srcRow, srcCol) + numOfCubesDownChain(srcRow, srcCol)) >= 2) ||
		((numOfCubesLeftChain(destRow, destCol) + numOfCubesRightChain(destRow, destCol)) >= 2) ||
		((numOfCubesUpChain(destRow, destCol) + numOfCubesDownChain(destRow, destCol)) >= 2)) {

		_boardOfCubes[srcRow][srcCol] = _destCube;
		_boardOfCubes[srcRow][srcCol]->setRow(srcRow);
		_boardOfCubes[srcRow][srcCol]->setCol(srcCol);

		_boardOfCubes[destRow][destCol] = _srcCube;
		_boardOfCubes[destRow][destCol]->setRow(destRow);
		_boardOfCubes[destRow][destCol]->setCol(destCol);

		_srcCube->runAction(MoveTo::create(time, positonOfDest));
		_destCube->runAction(MoveTo::create(time, positonOfSrc));

		return;
	}

	tmp = _boardOfIndex[srcRow][srcCol];
	_boardOfIndex[srcRow][srcCol] = _boardOfIndex[destRow][destCol];
	_boardOfIndex[destRow][destCol] = tmp;

	_srcCube->runAction(Sequence::create(MoveTo::create(time, positonOfDest), 
										 MoveTo::create(time, positonOfSrc), NULL));
	_destCube->runAction(Sequence::create(MoveTo::create(time, positonOfSrc),
										  MoveTo::create(time, positonOfDest), NULL));
	return;
}