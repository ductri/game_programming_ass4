#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "CellValue.h"
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

	//Test
	int _lineWidth = 660;
	int _lineHeight = 5;

	void markCellWith(CELL_VALUE cellValue, Vec2 locationView);
	bool checkWin(int x, int y);
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(MainGame);

	// tounch event
	virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event* event);
	virtual void onTouchEnded(Touch *touch, Event* event);
};

#endif // __HELLOWORLD_SCENE_H__
