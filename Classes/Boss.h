#ifndef __BOSS_H__
#define __BOSS_H__

#include "cocos2d.h"
#include "vector"
using namespace cocos2d;

#define TAG_WORMHOLE_ANIMATION  900
#define TAG_ATTACK_ANIMATION    1000

class WormHole;

class Boss : public Sprite {
public:
    static Boss* create(Vec2 left, Vec2 right);
    void setWormHole(Vec2 left, Vec2 right);
    void initFrame();
    void attack1();
    //void update(float delta);
    void takeDamage(int damage);
    //void setbasePosition(Vec2 position);
   
    void setTarget(Vec2 tar);
    int getHealth();

    void DieAnimation();
    void update(float delta);
    
    void attack2();
    //void reflex();
    //void stopAttackType2();

    WormHole* left;
    WormHole* right;
    //bool isType1 = true;
    bool isDead = false;
private:
    int health  = 200;
    int collisionCount  = 0;
    bool isattack   = false;
    bool isdie  = false;
    bool isTwo  = false;

    Vec2 originalPosition; // Lưu vị trí ban đầu
    Vec2 leftHole;
    Vec2 rightHole;
    Vec2 target;
    Vec2 velocity = Vec2(10, 5);
    //Sprite* wormholeSprite;
    Vector<SpriteFrame*>AttackAni;
    Vector<SpriteFrame*>DieAni;
    //Vector<SpriteFrame*>AttAni;
};

#define TAG_PLAY_WH 1100
#define TAG_START_WH 1200
class WormHole : public Sprite {
public:
    static  WormHole* create(Vec2 pos);
    void    initFrame();
    void    start();
    void    play();
    void    stop();
    Vec2    position;
private:
    Vector<SpriteFrame*>WormHoleAni;
    Vector<SpriteFrame*>startAni;
};

#endif // __BOSS_H__
