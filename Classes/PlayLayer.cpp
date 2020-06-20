#include "PlayLayer.h"
#include "CubeSprite.h"
#include "GameEndScene.h"

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
	_steps = 100;
	_labelPointStr = NULL;
	_labelPointNum = NULL;
	_labelStepStr = NULL;
	_points = 0;
	_isEnd = false;
	_endType = "NOT END";
	_level = 1;
}

PlayLayer::~PlayLayer()
{
	std::vector<std::vector<int> >::iterator rBoardIndex = _boardOfIndex->begin();
	std::vector<std::vector<CubeSprite*> >::iterator rBoardCubes;
	std::vector<CubeSprite*>::iterator cBoardCubes;

	for (; rBoardIndex != _boardOfIndex->end(); rBoardIndex++) {
		rBoardIndex->clear();
	}
	for (rBoardCubes = _boardOfCubes->begin(); rBoardCubes != _boardOfCubes->end(); rBoardCubes++) {
		for (cBoardCubes = rBoardCubes->begin(); cBoardCubes != rBoardCubes->end(); cBoardCubes++) {
			(*cBoardCubes) = NULL;
		}
		rBoardCubes->clear();
	}

}

PlayLayer* PlayLayer::create(int& level)
{
	auto playLayer = new PlayLayer;
	if (playLayer && playLayer->init(level)) {
		playLayer->autorelease();
		return playLayer;
	}
	CC_SAFE_DELETE(playLayer);
	return nullptr;
}

bool PlayLayer::init(int& level)
{
	if (!Layer::init()) {
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	_level = level;
	_numBoardRows = (_level == 1) ? 5 : 7;
	_numBoardCols = 5;
	int totalPoints = _level * 1000;

	//bkg and sprite list
	auto bkg = Sprite::create("bkg.png");
	float x = origin.x + visibleSize.width / 2;
	float y = origin.y + visibleSize.height / 2;
	bkg->setPosition(Vec2(x, y));
	addChild(bkg, -1);

	TTFConfig _labelConfig;
	_labelConfig.fontFilePath = "arial.ttf";
	_labelConfig.fontSize = 36;
	_labelConfig.bold = true;

	_labelPointStr = Label::createWithTTF(_labelConfig, "POINTS");
	_labelPointStr->setTextColor(Color4B::BLACK);
	_labelPointNum = Label::createWithTTF(_labelConfig, std::to_string(_points) + "/" + std::to_string(totalPoints));
	_labelPointNum->setTextColor(Color4B::BLACK);
	_labelStepStr = Label::createWithTTF(_labelConfig, "STEPS");
	_labelStepStr->setTextColor(Color4B::BLACK);
	_labelStepNum = Label::createWithTTF(_labelConfig, std::to_string(_steps));
	_labelStepNum->setTextColor(Color4B::BLACK);

	x = origin.x + visibleSize.width * 0.90;
	y = origin.y + visibleSize.height * 0.65;
	_labelPointStr->setPosition(Vec2(x, y));
	y = origin.y + visibleSize.height * 0.55;
	_labelPointNum->setPosition(Vec2(x, y));
	y = origin.y + visibleSize.height * 0.40;
	_labelStepStr->setPosition(Vec2(x, y));
	y = origin.y + visibleSize.height * 0.30;
	_labelStepNum->setPosition(Vec2(x, y));

	addChild(_labelPointStr);
	addChild(_labelPointNum);
	addChild(_labelStepStr);
	addChild(_labelStepNum);

	MenuItemImage* pause = MenuItemImage::create("PAUSE1.png", "PAUSE2.png", 
		[&](Ref* pSender) {
			this->pauseSchedulerAndActions();
		});

	x = origin.x + visibleSize.width * 0.10;
	y = origin.y + visibleSize.height * 0.65;
	pause->setPosition(Vec2(x, y));

	MenuItemImage* resume = MenuItemImage::create("RESUME1.png", "RESUME2.png", 
		[&](Ref* pSender) {
			this->resumeSchedulerAndActions();
		});

	x = origin.x + visibleSize.width * 0.10;
	y = origin.y + visibleSize.height * 0.45;
	resume->setPosition(Vec2(x, y));

	auto menu = Menu::create(pause, resume, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);

	spriteSheet = SpriteFrameCache::getInstance();
	spriteSheet->addSpriteFramesWithFile("cubeSprites.plist");
	spriteSheet->addSpriteFramesWithFile("cubeCol.plist");
	spriteSheet->addSpriteFramesWithFile("cubeRow.plist");

	_boardLeftTopX = (visibleSize.width - _numBoardCols * CubeSprite::getCubeLength() - (_numBoardCols - 1) * CUBE_GAP) / 2;
	_boardLeftTopY = (visibleSize.height + _numBoardRows * CubeSprite::getCubeLength() + (_numBoardRows - 1) * CUBE_GAP) / 2;

	_boardOfIndex = new std::vector<std::vector<int> >(_numBoardRows);
	_boardOfCubes = new std::vector<std::vector<CubeSprite*> >(_numBoardRows);

	std::vector<std::vector<int> >::iterator rBoardIndex = _boardOfIndex->begin();
	std::vector<int>::iterator cBoardIndex;
	for (; rBoardIndex != _boardOfIndex->end(); rBoardIndex++) {
		for (int c = 0; c < _numBoardCols; c++) {
			rBoardIndex->push_back(EMPTY);
		}
	}
	std::vector<std::vector<CubeSprite*> >::iterator rBoardCubes = _boardOfCubes->begin();
	std::vector<CubeSprite*>::iterator cBoardCubes;
	for (; rBoardCubes != _boardOfCubes->end(); rBoardCubes++) {
		for (int c = 0; c < _numBoardCols; c++) {
			rBoardCubes->push_back(NULL);
		}
	}

	std::srand(time(NULL));
	initBoardOfIndex();
	initBoardOfCubes();
	this->scheduleUpdate();

	//event listener
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(PlayLayer::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(PlayLayer::onTouchMoved, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

	return true;
}

void PlayLayer::update(float dt)
{
	if (!swappable(_boardOfIndex) || (_steps < 0 && _points < _level * 1000)) {
		PlayLayer::pauseSchedulerAndActions();
		_endType = "GAME OVER";
		//_isEnd = true;
		GameEnd* nextScene = GameEnd::create(_endType, _points, _level);
		nextScene->setEndType(_endType);
		Director::getInstance()->replaceScene(
			TransitionSlideInT::create(1.0f / 60, nextScene));
	}
	
	if (_points >= _level * 1000 && _steps >= 0) {
		PlayLayer::pauseSchedulerAndActions();
		_endType = "YOU WIN";
		//_isEnd = true;
		GameEnd* nextScene = GameEnd::create(_endType, _points, _level);
		nextScene->setEndType(_endType);
		Director::getInstance()->replaceScene(
			TransitionSlideInT::create(1.0f / 60, nextScene));
	}

	if (_isActing) {
		_isActing = false;
		
		for (int r = 0; r < _numBoardRows; r++) {
			for (int c = 0; c < _numBoardCols; c++) {
				if (((*_boardOfCubes)[r][c] != NULL) && ((*_boardOfCubes)[r][c]->getNumberOfRunningActions() > 0)) {
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
		{0, 2, 3, 1, 3},
		{6, 0, 2, 10, 1},
		{0, 1, 3, 0, 1},
		{3, 10, 1, 6, 0},
		{1, 1, 2, 0, 1}
	};

	std::vector<std::vector<int> >* initBoard = new std::vector<std::vector<int> >(_numBoardRows);
	std::vector<std::vector<int> >::iterator rInitBoard = initBoard->begin();
	for (; rInitBoard != initBoard->end(); rInitBoard++) {
		for (int c = 0; c < _numBoardCols; c++) {
			rInitBoard->push_back(EMPTY);
		}
	}
	generateInitBoardIndex(initBoard);

	/*
	std::stringstream initBoardStr;
	for (int i = 0; i < _numBoardRows; i++) {
		for (int j = 0; j < _numBoardCols; j++) {
			initBoardStr << (*initBoard)[i][j] << ' ';
		}
		initBoardStr << std::endl;
	}
	MessageBox(initBoardStr.str().c_str(), "InitBoard");
	*/

	for (int row = 0; row < _numBoardRows; row++) {
		for (int col = 0; col < _numBoardCols; col++) {
			(*_boardOfIndex)[row][col] = (*initBoard)[row][col];
		}
	}
}

void PlayLayer::initBoardOfCubes()
{
	for (int r = _numBoardRows - 1; r >= 0; r--) {
		for (int c = 0; c < _numBoardCols; c++) {
			(*_boardOfCubes)[r][c] = new CubeSprite();
			(*_boardOfCubes)[r][c]->setRow(r);
			(*_boardOfCubes)[r][c]->setCol(c);
			//(*_boardOfCubes)[r][c]->setImgIndex((*_boardOfIndex)[r][c]);

			//for debugging
			/*
			int i = (*_boardOfIndex)[r][c];
			if (i / TOTAL_CUBE == SPECIAL_COL_CUBE_INDEX) {
				(*_boardOfCubes)[r][c]->initWithSpriteFrameName(cubeCol[i % 4]);
			}
			else if (i / TOTAL_CUBE == SPECIAL_ROW_CUBE_INDEX) {
				(*_boardOfCubes)[r][c]->initWithSpriteFrameName(cubeRow[i % 4]);
			}
			else {
				(*_boardOfCubes)[r][c]->initWithSpriteFrameName(cubeSprites[i]);
			}
			*/
			///////////////

			(*_boardOfCubes)[r][c]->initWithSpriteFrameName(cubeSprites[(*_boardOfIndex)[r][c]]);
			(*_boardOfCubes)[r][c]->setPosition(winPositionOfCube(r, c));

			this->addChild((*_boardOfCubes)[r][c]);
			dropCube((*_boardOfCubes)[r][c]);
		}
	}
	
}

void PlayLayer::dropCube(CubeSprite* cube)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	Vec2 endPosition = cube->getPosition();
	Vec2 startPosition = Vec2(endPosition.x, endPosition.y + visibleSize.height);
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
	//checkAndClearBoardOfIndex(SPECIAL_COL_CUBE_INDEX);
	//checkAndClearBoardOfIndex(SPECIAL_ROW_CUBE_INDEX);
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
			if ((*_boardOfIndex)[r][c] == EMPTY) {
				continue;
			}
			leftCubesOfCube[r][c] = numOfCubesLeftChain(r, c, _boardOfIndex);
			rightCubesOfCube[r][c] = numOfCubesRightChain(r, c, _boardOfIndex);
			upCubesOfCube[r][c] = numOfCubesUpChain(r, c, _boardOfIndex);
			downCubesOfCube[r][c] = numOfCubesDownChain(r, c, _boardOfIndex);
		}
	}

	for (int r = 0; r < _numBoardRows; r++) {
		for (int c = 0; c < _numBoardCols; c++) {
			if (leftCubesOfCube[r][c] + rightCubesOfCube[r][c] >= 2) {
			//	switch (mode)
			//	{
			//	case 0:
					clearBoardOfIndexRow(r, c, leftCubesOfCube[r][c], rightCubesOfCube[r][c]);
			//		break;
				/*
				case SPECIAL_COL_CUBE_INDEX:
					if ((*_boardOfIndex)[r][c] / 4 == SPECIAL_COL_CUBE_INDEX) {
						clearBoardOfIndexCol(c);
						clearBoardOfIndexRow(r, c, leftCubesOfCube[r][c], rightCubesOfCube[r][c]);
					}
					break;
				case SPECIAL_ROW_CUBE_INDEX:
					if ((*_boardOfIndex)[r][c] / 4 == SPECIAL_ROW_CUBE_INDEX) {
						clearBoardOfIndexRow(r);
					}
					break;
				*/
			//	}
			}
			
			if (upCubesOfCube[r][c] + downCubesOfCube[r][c] >= 2) {
			//	switch (mode)
			//	{
			//	case 0:
					clearBoardOfIndexCol(r, c, upCubesOfCube[r][c], downCubesOfCube[r][c]);
			//		break;
				/*
				case SPECIAL_ROW_CUBE_INDEX:
					if ((*_boardOfIndex)[r][c] / 4 == SPECIAL_ROW_CUBE_INDEX) {
						clearBoardOfIndexRow(r);
						clearBoardOfIndexCol(r, c, upCubesOfCube[r][c], downCubesOfCube[r][c]);
					}
					break;
				case SPECIAL_COL_CUBE_INDEX:
					if ((*_boardOfIndex)[r][c] / 4 == SPECIAL_COL_CUBE_INDEX) {
						clearBoardOfIndexCol(c);
					}
					break;
				*/
			//	}
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
			if ((*_boardOfIndex)[r][c] == EMPTY) {
				_needFillin = true;
				if ((*_boardOfCubes)[r][c]) {
					(*_boardOfCubes)[r][c]->runAction(FadeOut::create(60.0f));
					(*_boardOfCubes)[r][c]->removeFromParent();
				}
				//delete (*_boardOfCubes)[r][c];
				(*_boardOfCubes)[r][c] = NULL;
				_points += 10;
				if (_labelPointNum != NULL) {
					_labelPointNum->setString(std::to_string(_points)+"/1000");
				}
			}
			else if ((*_boardOfIndex)[r][c] / TOTAL_CUBE == SPECIAL_COL_CUBE_INDEX) {
				if ((*_boardOfCubes)[r][c]) {
					int i = (*_boardOfIndex)[r][c] % TOTAL_CUBE;
					(*_boardOfCubes)[r][c]->setTexture(cubeCol[i]);
				}
			}
			else if ((*_boardOfIndex)[r][c] / TOTAL_CUBE == SPECIAL_ROW_CUBE_INDEX) {
				if ((*_boardOfIndex)[r][c]) {
					int i = (*_boardOfIndex)[r][c] % TOTAL_CUBE;
					(*_boardOfCubes)[r][c]->setTexture(cubeRow[i]);
				}
			}
		}
	}
	
}

int PlayLayer::numOfCubesLeftChain(int row, int col, std::vector<std::vector<int> >* board)
{
	int numOfCubes = 0;
	int leftNeighbourCol = col - 1;
	while (leftNeighbourCol >= 0) {
		if (((*board)[row][leftNeighbourCol] % TOTAL_CUBE) == ((*board)[row][col] % TOTAL_CUBE)) {
			numOfCubes++;
		}
		else {
			break;
		}
		leftNeighbourCol--;
	}
	return numOfCubes;
}

int PlayLayer::numOfCubesRightChain(int row, int col, std::vector<std::vector<int> >* board)
{
	int numOfCubes = 0;
	int rightNeighbourCol = col + 1;
	while (rightNeighbourCol < _numBoardCols) {
		if (((*board)[row][rightNeighbourCol] % TOTAL_CUBE) == ((*board)[row][col] % TOTAL_CUBE)) {
			numOfCubes++;
		}
		else {
			break;
		}
		rightNeighbourCol++;
	}
	return numOfCubes;
}

int PlayLayer::numOfCubesUpChain(int row, int col, std::vector<std::vector<int> >* board)
{
	int numOfCubes = 0;
	int upNeighbourRow = row - 1;
	while (upNeighbourRow >= 0) {
		if (((*board)[upNeighbourRow][col] % TOTAL_CUBE) == ((*board)[row][col] % TOTAL_CUBE)) {
			numOfCubes++;
		}
		else {
			break;
		}
		upNeighbourRow--;
	}
	return numOfCubes;
}

int PlayLayer::numOfCubesDownChain(int row, int col, std::vector<std::vector<int> >* board)
{
	int numOfCubes = 0;
	int downNeighbourRow = row + 1;
	while (downNeighbourRow < _numBoardRows) {
		if (((*board)[downNeighbourRow][col] % TOTAL_CUBE) == ((*board)[row][col] % TOTAL_CUBE)) {
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
	int i = (*_boardOfIndex)[row][col + right];
	for (int r = row, c = col - left; c <= col + right; c++) {
		switch ((*_boardOfIndex)[r][c] / TOTAL_CUBE)
		{
		case 0:
			(*_boardOfIndex)[r][c] = EMPTY;
			break;
		case SPECIAL_COL_CUBE_INDEX:
			clearBoardOfIndexCol(c);
			break;
		case SPECIAL_ROW_CUBE_INDEX:
			clearBoardOfIndexRow(r);
			break;
		}
		/*
		if ((*_boardOfIndex)[r][c] < TOTAL_CUBE) {
			(*_boardOfIndex)[r][c] = EMPTY;
		}
		*/
	}
	if (left + right >= 3) {
		(*_boardOfIndex)[row][col + right] = (SPECIAL_ROW_CUBE_INDEX * TOTAL_CUBE) + (i % TOTAL_CUBE);
	}
	else {
		(*_boardOfIndex)[row][col + right] = EMPTY;
	}
}

void PlayLayer::clearBoardOfIndexCol(int row, int col, int up, int down)
{
	int i = (*_boardOfIndex)[row + down][col];
	for (int r = row - up, c = col; r <= row + down; r++) {
		switch ((*_boardOfIndex)[r][c] / TOTAL_CUBE)
		{
		case 0:
			(*_boardOfIndex)[r][c] = EMPTY;
			break;
		case SPECIAL_COL_CUBE_INDEX:
			clearBoardOfIndexCol(c);
			break;
		case SPECIAL_ROW_CUBE_INDEX:
			clearBoardOfIndexRow(r);
			break;
		}
		/*
		if ((*_boardOfIndex)[r][c] < TOTAL_CUBE) {
			(*_boardOfIndex)[r][c] = EMPTY;
		}
		*/
	}
	if (up + down >= 3) {
		(*_boardOfIndex)[row + down][col] = (SPECIAL_COL_CUBE_INDEX * TOTAL_CUBE) + (i % TOTAL_CUBE);
	}
	else {
		(*_boardOfIndex)[row + down][col] = EMPTY;
	}
}

void PlayLayer::clearBoardOfIndexRow(int row)
{
	for (int c = 0; c < _numBoardCols; c++) {
		switch ((*_boardOfIndex)[row][c] / TOTAL_CUBE)
		{
		case SPECIAL_COL_CUBE_INDEX:
			clearBoardOfIndexCol(c);
		case 0:
		case SPECIAL_ROW_CUBE_INDEX:
			(*_boardOfIndex)[row][c] = EMPTY;
			break;
		}
	}
}

void PlayLayer::clearBoardOfIndexCol(int col)
{
	for (int r = 0; r < _numBoardRows; r++) {
		switch ((*_boardOfIndex)[r][col] / TOTAL_CUBE)
		{
		case SPECIAL_ROW_CUBE_INDEX:
			clearBoardOfIndexRow(r);
			break;
		case 0:
		case SPECIAL_COL_CUBE_INDEX:
			(*_boardOfIndex)[r][col] = EMPTY;
			break;
		}
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
			if ((*_boardOfIndex)[r][c] == EMPTY) {
				removedCubes++;
			}
			else {
				if (removedCubes > 0) {
					int newRow = r + removedCubes;
					(*_boardOfIndex)[newRow][c] = (*_boardOfIndex)[r][c];
					(*_boardOfIndex)[r][c] = EMPTY;

					Vec2 startPosition = winPositionOfCube(r, c);
					Vec2 endPositon = winPositionOfCube(newRow, c);
					float time = (startPosition.y - endPositon.y) / (2 * visibleSize.height);

					(*_boardOfCubes)[newRow][c] = (*_boardOfCubes)[r][c];
					(*_boardOfCubes)[r][c] = NULL;

					(*_boardOfCubes)[newRow][c]->stopAllActions();
					(*_boardOfCubes)[newRow][c]->runAction(MoveTo::create(time, endPositon));
					(*_boardOfCubes)[newRow][c]->setRow(newRow);
				}
			}
		}
		numOfEmptyInCol[c] = removedCubes;
	}
	
	for (int c = 0; c < _numBoardCols; c++) {
		if (numOfEmptyInCol[c] > 0) {
			for (int r = numOfEmptyInCol[c] - 1; r >= 0; r--) {
				(*_boardOfIndex)[r][c] = random() % TOTAL_CUBE;

				(*_boardOfCubes)[r][c] = new CubeSprite();
				(*_boardOfCubes)[r][c]->setRow(r);
				(*_boardOfCubes)[r][c]->setCol(c);
				//(*_boardOfCubes)[r][c]->setImgIndex((*_boardOfIndex)[r][c]);
				(*_boardOfCubes)[r][c]->initWithSpriteFrameName(cubeSprites[(*_boardOfIndex)[r][c]]);
				(*_boardOfCubes)[r][c]->setPosition(winPositionOfCube(r, c));

				this->addChild((*_boardOfCubes)[r][c]);
				dropCube((*_boardOfCubes)[r][c]);
			}
		}
	}
	
	delete[] numOfEmptyInCol;
	numOfEmptyInCol = NULL;
}

bool PlayLayer::onTouchBegan(Touch* touch, Event* event)
{
	_srcCube = NULL;
	_destCube = NULL;
	if (_isTouchEnable) {
		auto touchLocation = touch->getLocation();
		_srcCube = cubeOfPoint(&touchLocation);
	}
	return _isTouchEnable;
}

void PlayLayer::onTouchMoved(Touch* touch, Event* event)
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
			_destCube = (*_boardOfCubes)[row][col];
		}
		swapCubes();
		return;
	}
	
	if (rightRect.containsPoint(touchLocation)) {
		col++;
		if (col < _numBoardCols) {
			_destCube = (*_boardOfCubes)[row][col];
		}
		swapCubes();
		return;
	}

	if (upRect.containsPoint(touchLocation)) {
		row--;
		if (row >= 0) {
			_destCube = (*_boardOfCubes)[row][col];
		}
		swapCubes();
		return;
	}

	if (downRect.containsPoint(touchLocation)) {
		row++;
		if (row < _numBoardRows) {
			_destCube = (*_boardOfCubes)[row][col];
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
			if ((*_boardOfCubes)[r][c] != NULL) {
				cubeZone.origin.x = (*_boardOfCubes)[r][c]->getPositionX() - (*_boardOfCubes)[r][c]->getContentSize().width / 2;
				cubeZone.origin.y = (*_boardOfCubes)[r][c]->getPositionY() - (*_boardOfCubes)[r][c]->getContentSize().height / 2;
				cubeZone.size = (*_boardOfCubes)[r][c]->getContentSize();
				if (cubeZone.containsPoint(*point)) {
					return (*_boardOfCubes)[r][c];
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

	std::swap((*_boardOfIndex)[srcRow][srcCol], (*_boardOfIndex)[destRow][destCol]);

	if  ((numOfCubesLeftChain(srcRow, srcCol, _boardOfIndex) + numOfCubesRightChain(srcRow, srcCol, _boardOfIndex) >= 2) ||
		((numOfCubesUpChain(srcRow, srcCol, _boardOfIndex) + numOfCubesDownChain(srcRow, srcCol, _boardOfIndex)) >= 2) ||
		((numOfCubesLeftChain(destRow, destCol, _boardOfIndex) + numOfCubesRightChain(destRow, destCol, _boardOfIndex)) >= 2) ||
		((numOfCubesUpChain(destRow, destCol, _boardOfIndex) + numOfCubesDownChain(destRow, destCol, _boardOfIndex)) >= 2)) {

		(*_boardOfCubes)[srcRow][srcCol] = _destCube;
		(*_boardOfCubes)[srcRow][srcCol]->setRow(srcRow);
		(*_boardOfCubes)[srcRow][srcCol]->setCol(srcCol);

		(*_boardOfCubes)[destRow][destCol] = _srcCube;
		(*_boardOfCubes)[destRow][destCol]->setRow(destRow);
		(*_boardOfCubes)[destRow][destCol]->setCol(destCol);

		_srcCube->runAction(MoveTo::create(time, positonOfDest));
		_destCube->runAction(MoveTo::create(time, positonOfSrc));

		_steps--;
		_labelStepNum->setString(std::to_string(_steps));

		return;
	}

	std::swap((*_boardOfIndex)[srcRow][srcCol], (*_boardOfIndex)[destRow][destCol]);

	_srcCube->runAction(Sequence::create(MoveTo::create(time, positonOfDest), 
										 MoveTo::create(time, positonOfSrc), NULL));
	_destCube->runAction(Sequence::create(MoveTo::create(time, positonOfSrc),
										  MoveTo::create(time, positonOfDest), NULL));
	return;
}

void PlayLayer::generateInitBoardIndex(std::vector<std::vector<int> >* board)
{
	bool twoSameInRow = false;
	bool twoSameInCol = false;
	int colIndex;
	int rowIndex;

	for (int r = 0; r < _numBoardRows; r++) {
		for (int c = 0; c < _numBoardCols; c++) {
			twoSameInRow = false;
			twoSameInCol = false;
			if (r > 1) {
				if ((*board)[r - 1][c] == (*board)[r - 2][c]) {
					colIndex = (*board)[r - 1][c];
					twoSameInCol = true;
				}
			}
			if (c > 1) {
				if ((*board)[r][c - 1] == (*board)[r][c - 2]) {
					rowIndex = (*board)[r][c - 1];
					twoSameInRow = true;
				}
			}
			if (twoSameInCol && twoSameInRow) {
				(*board)[r][c] = indexExcept(colIndex, rowIndex);
			}
			else if (twoSameInCol) {
				(*board)[r][c] = indexExcept(colIndex);
			}
			else if (twoSameInRow) {
				(*board)[r][c] = indexExcept(rowIndex);
			}
			else {
				(*board)[r][c] = rand() % TOTAL_CUBE;
			}
		}
	}
	if (swappable(board)) {
		return;
	}
	else {
		generateInitBoardIndex(board);
	}
}

int PlayLayer::indexExcept(int given)
{
	std::vector<int> indexes = { 0, 1, 2, 3 };
	std::vector<int> legalIndexes;
	for (int i = 0; i < TOTAL_CUBE; i++) {
		if (indexes[i] != given) {
			legalIndexes.push_back(indexes[i]);
		}
	}
	int index = legalIndexes[rand() % legalIndexes.size()];
	return index;
}

int PlayLayer::indexExcept(int given1, int given2)
{
	std::vector<int> indexes = { 0, 1, 2, 3 };
	std::vector<int> legalIndexes;
	for (int i = 0; i < TOTAL_CUBE; i++) {
		if (indexes[i] != given1 && indexes[i] != given2) {
			legalIndexes.push_back(indexes[i]);
		}
	}
	int index = legalIndexes[rand() % legalIndexes.size()];
	return index;
}

bool PlayLayer::swappable(std::vector<std::vector<int> >* board)
{
	for (int r = 0; r < _numBoardRows - 1; r++) {
		for (int c = 0; c < _numBoardCols - 1; c++) {
			std::swap((*board)[r][c], (*board)[r][c + 1]);
			if (numOfCubesLeftChain(r, c, board) + numOfCubesRightChain(r, c, board) >= 2) {
				std::swap((*board)[r][c], (*board)[r][c + 1]);
				return true;
			}
			if (numOfCubesUpChain(r, c, board) + numOfCubesDownChain(r, c, board) >= 2) {
				std::swap((*board)[r][c], (*board)[r][c + 1]);
				return true;
			}
			if (numOfCubesLeftChain(r, c + 1, board) + numOfCubesRightChain(r, c + 1, board) >= 2) {
				std::swap((*board)[r][c], (*board)[r][c + 1]);
				return true;
			}
			if (numOfCubesUpChain(r, c + 1, board) + numOfCubesDownChain(r, c + 1, board) >= 2) {
				std::swap((*board)[r][c], (*board)[r][c + 1]);
				return true;
			}
			std::swap((*board)[r][c], (*board)[r][c + 1]);

			std::swap((*board)[r][c], (*board)[r + 1][c]);
			if (numOfCubesLeftChain(r, c, board) + numOfCubesRightChain(r, c, board) >= 2) {
				std::swap((*board)[r][c], (*board)[r + 1][c]);
				return true;
			}
			if (numOfCubesUpChain(r, c, board) + numOfCubesDownChain(r, c, board) >= 2) {
				std::swap((*board)[r][c], (*board)[r + 1][c]);
				return true;
			}
			if (numOfCubesLeftChain(r + 1, c, board) + numOfCubesRightChain(r + 1, c, board) >= 2) {
				std::swap((*board)[r][c], (*board)[r + 1][c]);
				return true;
			}
			if (numOfCubesUpChain(r + 1, c, board) + numOfCubesDownChain(r + 1, c, board) >= 2) {
				std::swap((*board)[r][c], (*board)[r + 1][c]);
				return true;
			}
			std::swap((*board)[r][c], (*board)[r + 1][c]);
		}
	}

	for (int r = 0, c = _numBoardCols - 1; r < _numBoardRows - 1; r++) {
		std::swap((*board)[r + 1][c], (*board)[r + 1][c]);
		if (numOfCubesLeftChain(r, c, board) + numOfCubesRightChain(r, c, board) >= 2) {
			std::swap((*board)[r][c], (*board)[r + 1][c]);
			return true;
		}
		if (numOfCubesUpChain(r, c, board) + numOfCubesDownChain(r, c, board) >= 2) {
			std::swap((*board)[r][c], (*board)[r + 1][c]);
			return true;
		}
		if (numOfCubesLeftChain(r + 1, c, board) + numOfCubesRightChain(r + 1, c, board) >= 2) {
			std::swap((*board)[r][c], (*board)[r + 1][c]);
			return true;
		}
		if (numOfCubesUpChain(r + 1, c, board) + numOfCubesDownChain(r + 1, c, board) >= 2) {
			std::swap((*board)[r][c], (*board)[r + 1][c]);
			return true;
		}
		std::swap((*board)[r + 1][c], (*board)[r + 1][c]);
	}

	for (int r = _numBoardRows - 1, c = 0; c < _numBoardCols - 1; c++) {
		std::swap((*board)[r][c], (*board)[r][c + 1]);
		if (numOfCubesLeftChain(r, c, board) + numOfCubesRightChain(r, c, board) >= 2) {
			std::swap((*board)[r][c], (*board)[r][c + 1]);
			return true;
		}
		if (numOfCubesUpChain(r, c, board) + numOfCubesDownChain(r, c, board) >= 2) {
			std::swap((*board)[r][c], (*board)[r][c + 1]);
			return true;
		}
		if (numOfCubesLeftChain(r, c + 1, board) + numOfCubesRightChain(r, c + 1, board) >= 2) {
			std::swap((*board)[r][c], (*board)[r][c + 1]);
			return true;
		}
		if (numOfCubesUpChain(r, c + 1, board) + numOfCubesDownChain(r, c + 1, board) >= 2) {
			std::swap((*board)[r][c], (*board)[r][c + 1]);
			return true;
		}
		std::swap((*board)[r][c], (*board)[r][c + 1]);
	}

	return false;
}