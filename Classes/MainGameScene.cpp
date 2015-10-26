#include "MainGameScene.h"
USING_NS_CC;

using namespace cocos2d;
Scene* MainGame::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainGame::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainGame::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    _visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = cocos2d::LayerColor::create(Color4B(255, 255, 255, 255));
	this->addChild(bg, 0);

	CCSpriteBatchNode *batchNodeLine = CCSpriteBatchNode::create("line.png", 2000);
	_itemBatch = CCSpriteBatchNode::create("x.png", 2000);

	_dataBoardGame = new CELL_VALUE*[GAME_MATRIX_SIZE];
	for (int i = 0;i < GAME_MATRIX_SIZE;i++) {
		_dataBoardGame[i] = new CELL_VALUE[GAME_MATRIX_SIZE];
		for (int j = 0;j < GAME_MATRIX_SIZE;j++) {
			_dataBoardGame[i][j] = CELL_VALUE::UNSET;
		}
	}

	int numLineEachRow = GAME_MATRIX_SIZE*GAME_CELL_SIZE /_lineWidth + 1; //TODO get size
	int numLineCol = GAME_MATRIX_SIZE;
	_lineMatrixLenght = numLineEachRow*GAME_MATRIX_SIZE * 2;
	_lineMatrix = new cocos2d::Sprite*[_lineMatrixLenght];
	
	_startLocationX = -(GAME_MATRIX_SIZE*GAME_CELL_SIZE - _visibleSize.width) / 2;
	_startLocationY = (GAME_MATRIX_SIZE*GAME_CELL_SIZE - _visibleSize.height) / 2 + _visibleSize.height;

	// Horizontal
	for (int i = 0;i < GAME_MATRIX_SIZE;i++) {
		for (int j = 0;j < numLineEachRow;j++) {
			_lineMatrix[i*numLineEachRow + j] = cocos2d::Sprite::createWithTexture(batchNodeLine->getTexture());
			_lineMatrix[i*numLineEachRow + j]->setAnchorPoint(Vec2(0, 0));
			_lineMatrix[i*numLineEachRow + j]->setPosition(_startLocationX+j*_lineWidth,
				_startLocationY-i*GAME_CELL_SIZE);
			this->addChild(_lineMatrix[i*numLineEachRow + j]);
		}
	}
	// Vertical
	for (int i = 0;i < GAME_MATRIX_SIZE;i++) {
		for (int j = 0;j < numLineEachRow;j++) {
			_lineMatrix[GAME_MATRIX_SIZE*numLineEachRow + i*numLineEachRow + j] = cocos2d::Sprite::createWithTexture(batchNodeLine->getTexture());
			_lineMatrix[GAME_MATRIX_SIZE*numLineEachRow + i*numLineEachRow + j]->setAnchorPoint(Vec2(0, 0));
			_lineMatrix[GAME_MATRIX_SIZE*numLineEachRow + i*numLineEachRow + j]->setRotation(90);
			_lineMatrix[GAME_MATRIX_SIZE*numLineEachRow + i*numLineEachRow + j]->setPosition(_startLocationX + i*GAME_CELL_SIZE,
				_startLocationY - j*_lineWidth);
			this->addChild(_lineMatrix[GAME_MATRIX_SIZE*numLineEachRow + i*numLineEachRow + j]);
		}
	}

	_itemMatrix = new Sprite*[GAME_MATRIX_SIZE*GAME_MATRIX_SIZE];


	// Implement touch event
	auto singleTouchListener = EventListenerTouchOneByOne::create();
	singleTouchListener->onTouchBegan = CC_CALLBACK_2(MainGame::onTouchBegan, this);
	singleTouchListener->onTouchMoved = CC_CALLBACK_2(MainGame::onTouchMoved, this);
	singleTouchListener->onTouchEnded = CC_CALLBACK_2(MainGame::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(singleTouchListener, this);
    return true;
}

void MainGame::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

bool MainGame::onTouchBegan(Touch *touch, Event* event) {
	_isMoving = false;
	return true;
}

void MainGame::onTouchMoved(Touch *touch, Event* event) {
	auto location = touch->getLocation();

	Vec2 deltaMove = touch->getLocation() - _touchBeganLocation;
	_startLocationX += touch->getDelta().x;
	_startLocationY += touch->getDelta().y;

	for (int i = 0;i < _lineMatrixLenght;i++) {
		_lineMatrix[i]->setPosition(_lineMatrix[i]->getPosition() + (touch->getDelta()));
	}

	for (int i = 0;i < _itemMatrixCount;i++) {
		_itemMatrix[i]->setPosition(_itemMatrix[i]->getPosition() + (touch->getDelta()));
	}
	_isMoving = true;
}

void MainGame::onTouchEnded(Touch *touch, Event* event) {
	Vec2 playerLastLocation;
	if (!_isMoving) {
		_touchBeganLocation = touch->getLocation();
		playerLastLocation = markCellWith(touch->getLocationInView());
		
			if (_gameMode == GAME_MODE::SINGLE) {
				if (playerLastLocation != Vec2(-1, -1)) {
					Vec2 loc = AIPlay(playerLastLocation);
					markCellWith(loc.x, loc.y);
				}
			}
	}

	
}

/**
*	Get cell location
*	Set dataBoardGame
*		- Check if cell has set x or o yet
*	Turn player
*	Check win
**/
Vec2 MainGame::markCellWith(Vec2 locationView) {
	
	auto absolutelyX = -_startLocationX + locationView.x;
	auto absolutelyY = _startLocationY - (_visibleSize.height - locationView.y) + _lineHeight;

	int tempX = absolutelyX / (GAME_CELL_SIZE);
	int tempY = absolutelyY / (GAME_CELL_SIZE);
	
	// Click at line
	if (absolutelyX - tempX*GAME_CELL_SIZE < _lineHeight ||
		absolutelyY - tempY*GAME_CELL_SIZE < _lineHeight)
		return Vec2(-1,-1);

	// Out of board game
	if (tempX < 0 || tempX >= GAME_MATRIX_SIZE || tempY < 0 || tempY >= GAME_MATRIX_SIZE)
		return Vec2(-1, -1);
	// Cell has already value
	if (_dataBoardGame[tempY][tempX] != CELL_VALUE::UNSET)
		return Vec2(-1, -1);
	
	if (_currentPlayer == CELL_VALUE::X) {
		_itemMatrix[_itemMatrixCount] = cocos2d::Sprite::create("x.png");
		_dataBoardGame[tempY][tempX] = CELL_VALUE::X;
	}
	else {
		_itemMatrix[_itemMatrixCount] = cocos2d::Sprite::create("o.png");
		_dataBoardGame[tempY][tempX] = CELL_VALUE::O;
	}
	checkWin(tempY, tempX);

	_itemMatrix[_itemMatrixCount]->setPosition(tempX*(GAME_CELL_SIZE)+0.5*GAME_CELL_SIZE + _startLocationX,
		_startLocationY - (tempY*(GAME_CELL_SIZE)+0.5*GAME_CELL_SIZE));
	this->addChild(_itemMatrix[_itemMatrixCount]);
	_itemMatrixCount++;

	if (_currentPlayer == CELL_VALUE::X)
		_currentPlayer = CELL_VALUE::O;
	else _currentPlayer = CELL_VALUE::X;

	return Vec2(tempX, tempY);
}

void MainGame::markCellWith(int locationBoardX, int locationBoardY) {

	int tempX = locationBoardX;
	int tempY = locationBoardY;

	// Out of board game
	if (tempX < 0 || tempX >= GAME_MATRIX_SIZE || tempY < 0 || tempY >= GAME_MATRIX_SIZE)
		return;
	// Cell has already value
	if (_dataBoardGame[tempY][tempX] != CELL_VALUE::UNSET)
		return;

	if (_currentPlayer == CELL_VALUE::X) {
		_itemMatrix[_itemMatrixCount] = cocos2d::Sprite::create("x.png");
		_dataBoardGame[tempY][tempX] = CELL_VALUE::X;
	}
	else {
		_itemMatrix[_itemMatrixCount] = cocos2d::Sprite::create("o.png");
		_dataBoardGame[tempY][tempX] = CELL_VALUE::O;
	}
	checkWin(tempY, tempX);

	_itemMatrix[_itemMatrixCount]->setPosition(tempX*(GAME_CELL_SIZE)+0.5*GAME_CELL_SIZE + _startLocationX,
		_startLocationY - (tempY*(GAME_CELL_SIZE)+0.5*GAME_CELL_SIZE));
	this->addChild(_itemMatrix[_itemMatrixCount]);
	_itemMatrixCount++;

	if (_currentPlayer == CELL_VALUE::X)
		_currentPlayer = CELL_VALUE::O;
	else _currentPlayer = CELL_VALUE::X;
}

bool MainGame::checkWin(int x, int y) {
	if (_dataBoardGame[x][y] == CELL_VALUE::UNSET)
		return false;

	//	1: Thang dung
	int counter = 1;
	for (int i = 1;i < 5;i++) {
		if (x - i < 0 || _dataBoardGame[x - i][y] != _dataBoardGame[x][y]) {
			break;
		}
		counter++;
	}
	for (int i = 1;i < 5;i++) {
		if (x + i >= GAME_MATRIX_SIZE || _dataBoardGame[x + i][y] != _dataBoardGame[x][y]) {
			break;
		}
		counter++;
	}

	if (counter > 4) {
		cocos2d::MessageBox(_dataBoardGame[x][y] + " is the WINNER", "Game over");
		return true;
	}

	//	2: Nam ngang
	counter = 1;
	for (int i = 1;i < 5;i++) {
		if (y - i < 0 || _dataBoardGame[x][y - i] != _dataBoardGame[x][y]) {
			break;
		}
		counter++;
	}
	for (int i = 1;i < 5;i++) {
		if (y + i >= GAME_MATRIX_SIZE || _dataBoardGame[x][y + i] != _dataBoardGame[x][y]) {
			break;
		}
		counter++;
	}

	if (counter > 4) {
		cocos2d::MessageBox(_dataBoardGame[x][y] + " is the WINNER", "Game over");
		return true;
	}

	//	3: Xiet sac '/'
	counter = 1;
	for (int i = 1;i < 5;i++) {
		if (x - i < 0 || y + i >= GAME_MATRIX_SIZE || _dataBoardGame[x - i][y + i] != _dataBoardGame[x][y]) {
			break;
		}
		counter++;
	}
	for (int i = 1;i < 5;i++) {
		if (x + i >= GAME_MATRIX_SIZE || y - i <0 || _dataBoardGame[x + i][y - i] != _dataBoardGame[x][y]) {
			break;
		}
		counter++;
	}

	if (counter > 4) {
		cocos2d::MessageBox(_dataBoardGame[x][y] + " is the WINNER", "Game over");
		return true;
	}
	
	//	4: Xiet huyen '\'
	counter = 1;
	for (int i = 1;i < 5;i++) {
		if (x - i < 0 || y - i < 0 ||_dataBoardGame[x - i][y - i] != _dataBoardGame[x][y]) {
			break;
		}
		counter++;
	}
	for (int i = 1;i < 5;i++) {
		if (x + i > GAME_MATRIX_SIZE || y + i > GAME_MATRIX_SIZE || _dataBoardGame[x + i][y + i] != _dataBoardGame[x][y]) {
			break;
		}
		counter++;
	}

	if (counter > 4) {
		cocos2d::MessageBox(_dataBoardGame[x][y] + " is the WINNER", "Game over");
		return true;
	}

	return false;
}


//*******************************************************
//IMPLEMENT AI
//*******************************************************
Vec2 MainGame::AIPlay(Vec2 playerLastLocation) {
	return Vec2(playerLastLocation.x, playerLastLocation.y + 1);
}