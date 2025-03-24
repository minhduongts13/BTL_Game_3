#ifndef __BOSS_H__
#define __BOSS_H__

#include "cocos2d.h"
#include "vector"
using namespace cocos2d;

#define TAG_WORMHOLE_ANIMATION  900
#define TAG_ATTACK_ANIMATION    1000

class Boss : public Sprite {
public:
    static Boss* spawnMonster(Vec2 left, Vec2 right);

    //void update(float delta);
    void takeDamage(int damage);
    void setbasePosition(Vec2 position);
    void setWormHole(Vec2 left, Vec2 right);
    void wormHoleAni();
    void startRepeatingWormHoleAni();
    //void updatePhysicsShapes();
    //void startShapeChangeScheduler();
    void initFrame();
    void stopWormHoleAni();
    void attack(Vec2 target);
private:
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

#endif // __BOSS_H__
