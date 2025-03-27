#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "cocos2d.h"
using namespace cocos2d;

class Monster : public Sprite {
public:
    void update(float delta);
    virtual void takeDamage(int damage) {};
    bool isDead = false;
    void setPatrolPositions(Vec2 startPos, Vec2 endPos);
    virtual void startPatrol(float duration) {};
    Vec2 patrolEnd = Vec2(0, 0);
    Vec2 patrolStart = Vec2(0, 0);
    int damage = 0;
protected:
    bool lastDirection = true;
    int health = 100;
    Vec2 originalPosition; // Lưu vị trí ban đầu
    
};


class FlyingMonster : public Monster {
public:
    static FlyingMonster* spawnMonster(Vec2 position);

    virtual void takeDamage(int damage) override;
    virtual void startPatrol(float duration) override;
};



class GroundMonster : public Monster {
public:
    static GroundMonster* spawnMonster(Vec2 position);
    virtual void startPatrol(float duration) override;
    virtual void takeDamage(int damage) override;
};


#endif // __MONSTER_H__
