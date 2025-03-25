#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "cocos2d.h"
#include "vector"
using namespace cocos2d;

#define TAG_WORMHOLE_ANIMATION  100
#define TAG_ATTACK_ANIMATION    200

class Monster : public Sprite {
public:
    static Monster* spawnMonster();

    void update(float delta);
    void takeDamage(int damage);
    void setbasePosition(Vec2 position);
    void setWormHole(Vec2 left, Vec2 right);
    void wormHoleAni();
    void startRepeatingWormHoleAni();
    //void updatePhysicsShapes();
    void startShapeChangeScheduler();
    void initFrame();
    void stopWormHoleAni();
    void attack(Vec2 target);
private:
    /*Sprite* monsterSprite;
    Vec2 movingDirection;
    float speed;*/
    int health = 100;
    Vec2 originalPosition; // Lưu vị trí ban đầu
    Vec2 leftHole;
    Vec2 rightHole;
    Sprite* wormholeSprite;
    Vector<SpriteFrame*>WormHoleAni;
    Vector<SpriteFrame*>AttackAni;
    std::vector<Size>frameSizes;
    std::vector<Size>AttackframeSizes;
};

#endif // __MONSTER_H__
