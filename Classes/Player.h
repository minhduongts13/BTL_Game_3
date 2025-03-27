#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"
#include "Monster.h"
using namespace cocos2d;

#define TAG_MOVE_ANIMATION 100
#define TAG_ATTACK_ANIMATION 200
#define TAG_JUMP_ANIMATION 300
#define TAG_FALL_ANIMATION 400

class Fire;
class Player : public Sprite
{
public:
    static Player *createPlayer(); // Hàm tạo nhân vật

    void update(float delta);
    void onKeyPressed(EventKeyboard::KeyCode keyCode);
    void onKeyReleased(EventKeyboard::KeyCode keyCode);
    void updateDirection();
    void playAttackAnimation(); // Hàm chơi animation tấn công
    void stopMovingAnimation();
    void updateAnimationPriority();
    void playMoveAnimation(bool isMovingRight);
    void playFallAnimation(bool lastDirection);
    void onGroundContact();
    void jump();
    void playJumpAnimation(bool isFacingRight);
    void increaseSpeed();
    void heal(int amount);
    void addGold(int amount);
    void resetSpeed();
    // Biến cho nhảy
    int jumpCount = 0;
    int maxJumpCount = 2; // Cho phép double jump
    bool isOnGround = false;
    void takeDamage(int damage);
    void die();
    bool isDead = false;
    int damage = 50;
    int gold = 0;
    void setlastDirection(bool value) { lastDirection = value; }
    bool getlastDirection() { return lastDirection; }
    void setKey(bool value) { key = value; }
    bool getKey() { return key; }
    //void setHp(int value) { hp = value; }
    //int getMaxHp() { return maxHp; }
    int getHp() { return hp; }
    int getMana() { return mana; }
    int getMaxMana() { return maxMana; }
    void healMana(int amount) {
        mana += amount;
        if (mana > maxMana)
            mana = maxMana;
        if (mana <= 0) mana = 0;
    }
    void attack2();
    Fire *fire;
    void setMaxHp(int val) {
        maxHp = val;
    }
    int getMaxHp() {
        return maxHp;
    }
private:
    bool lastDirection = true; // true: phải, false: trái
    Vec2 movingDirection;
    std::map<EventKeyboard::KeyCode, bool> keyStates;
    bool isAttacking = false; // Trạng thái tấn công
    float speed = 100.0f;     // Tốc độ di chuyển cơ bản, có thể điều chỉnh
    float baseSpeed = 100.0f; // Lưu lại tốc độ ban đầu
    int hp = 4;               // Máu ban đầu
    int maxHp = 4;
    bool key = false;
    int mana = 10;               // Máu ban đầu
    int maxMana = 20;
};
class Fire : public Sprite
{

public:
    static Fire* create();

    void initAttackFrame();

    void update(float delta, Vec2 playerpos, bool lastDirection);

    void attack();

private:
    Vector<SpriteFrame*> attack2Ani;

    Vec2 playerpos;

    bool isAttack = false;

    bool isright;
};
#endif // __PLAYER_H__
