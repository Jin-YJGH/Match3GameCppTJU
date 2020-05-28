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
		delete (_boardOfIndex);
	}
	if (_boardOfCubes) {
		delete (_boardOfCubes);
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
		_boardOfIndex[r] = new int[_numBoardCols] {EMPTY};
		_boardOfCubes[r] = new CubeSprite * [_numBoardCols];
	}
	for (int r = 0; r < _numBoardRows; r++) {
		for (int c = 0; c < _numBoardCols; c++) {
			_boardOfIndex[r][c] = EMPTY;
			_boardOfCubes[r][c] = new CubeSprite;
		}
	}

	initBoardOfIndex();
	initBoardOfCubes();
	this->scheduleUpdate();

	//event listener

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
		{2, 2, 2, 2, 1},
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
			if (_boardOfIndex[r][c] != EMPTY) {
			_boardOfCubes[r][c] = CubeSprite::create(r, c, _boardOfIndex[r][c]);
			_boardOfCubes[r][c]->setPosition(winPositionOfCube(r, c));
			this->addChild(_boardOfCubes[r][c]);
			dropCube(r, c, _boardOfCubes[r][c]);
			}
			
		}
	}
}

void PlayLayer::dropCube(int row, int col, CubeSprite* cube)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	Vec2 endPosition = winPositionOfCube(row, col);
	Vec2 startPosition = Vec2(endPosition.x, endPosition.y + visibleSize.height / 2);
	float time = startPosition.y / (visibleSize.height);

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

	for (int r = 0; r < _numBoardRows; r++) {
		for (int c = 0; c < _numBoardCols; c++) {
			if (_boardOfIndex[r][c] == EMPTY) {
				continue;
			}

			int leftCubes = numOfCubesLeftChain(r, c);
			int rightCubes = numOfCubesRightChain(r, c);
			int upCubes = numOfCubesUpChain(r, c);
			int downCubes = numOfCubesDownChain(r, c);

			int rowCubes = leftCubes + rightCubes + 1;
			int colCubes = upCubes + downCubes + 1;
			
			if (rowCubes < 3 && colCubes < 3) {
				continue;
			}
			else if (rowCubes >= 3 && colCubes >= 3) {
				clearBoardOfIndexRowAndCol(r, c, leftCubes, rightCubes, upCubes, downCubes);
			}
			else {
				if (rowCubes > colCubes) {
					clearBoardOfIndexRow(r, c, leftCubes, rightCubes);
				}
				else {
					clearBoardOfIndexCol(r, c, upCubes, downCubes);
				}
			}
			
		}
	}
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

void PlayLayer::clearBoardOfIndexRowAndCol(int row, int col, int left, int right, int up, int down)
{
	for (int r = row, c = col - left; c <= col + right; c++) {
		_boardOfIndex[r][c] = EMPTY;
	}
	for (int r = row - up, c = col; r <= row + down; r++) {
		_boardOfIndex[r][c] = EMPTY;
	}
}

void PlayLayer::clearBoardOfIndexRow(int row, int col, int left, int right)
{
	for (int r = row, c = col - left; c <= col + right; c++) {
		_boardOfIndex[r][c] = EMPTY;
	}
}

void PlayLayer::clearBoardOfIndexCol(int row, int col, int up, int down)
{
	for (int r = row - up, c = col; r < row + down; r++) {
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
					float time = (startPosition.y - endPositon.y) / (0.5 * visibleSize.height);
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
				_boardOfCubes[r][c] = CubeSprite::create(r, c, _boardOfIndex[r][c]);
				this->addChild(_boardOfCubes[r][c]);
				dropCube(r, c, _boardOfCubes[r][c]);
			}
		}
	}
	

	delete numOfEmptyInCol;
}