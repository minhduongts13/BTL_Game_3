#ifndef __BOARD_H__
#define __BOARD_H__

#include "cocos2d.h"
//#include "vector"
using namespace cocos2d;

//#define TAG_WORMHOLE_ANIMATION  900
//#define TAG_ATTACK_ANIMATION    1000

class Board : public Sprite {
public:
    static Board* create(bool isboard);
    void initFrame();
    void update(float delta, int HP);
    void setIsBoard(bool isboard);
private:
    bool isboard = true;
    Vec2 position;
    Vector<SpriteFrame*>BoardFrame;
};

#endif // __BOARD_H__
