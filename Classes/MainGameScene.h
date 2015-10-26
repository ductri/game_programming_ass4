#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "EnumValue.h"
using namespace cocos2d;
class MainGame : public cocos2d::Layer
{
private:
	const int GAME_MATRIX_SIZE = 50;
	const int GAME_CELL_SIZE = 30; //pixel
	Size _visibleSize;
	Sprite** _lineMatrix;

	CCSpriteBatchNode* _itemBatch;
	Sprite** _itemMatrix;
	int _itemMatrixCount=0;

	int _lineMatrixLenght;
	CELL_VALUE **_dataBoardGame;
	bool playerX = true; //Luot choi
	
	Vec2 _touchBeganLocation;
	Vec2 _lineOldPos;
	Vec2 _line2OldPos;
	Vec2 _currentTouchLocation;
	
	float _startLocationX;
	float _startLocationY;

	CELL_VALUE _currentPlayer = CELL_VALUE::X;
	bool _isMoving = false;

	GAME_MODE _gameMode = GAME_MODE::SINGLE;

	int _lineWidth = 506;
	int _lineHeight = 5;


	//*********************************
	/* FUNCTION                       */
	//*********************************
	Vec2 markCellWith(Vec2 locationView);
	bool checkWin(int x, int y);
	virtual bool init();
	void menuCloseCallback(cocos2d::Ref* pSender);
	CREATE_FUNC(MainGame);

	// tounch event
	virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event* event);
	virtual void onTouchEnded(Touch *touch, Event* event);
public:
	static cocos2d::Scene* createScene();
	/*
	For playing turn of AI.
	*	* cellValue: X or O
	*	* locationBoardX: x in _matrix _dataBoardGame
	*	* locationBoardY: y in _matrix_dataBoardGame
	*/
	void markCellWith(int locationBoardX, int locationBoardY);
	Vec2 AIPlay(Vec2 playerLastLocation);
};

#endif // __HELLOWORLD_SCENE_H__
