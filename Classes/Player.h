#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"
#include "Monster.h"
using namespace cocos2d;


#define TAG_MOVE_ANIMATION 100
#define TAG_ATTACK_ANIMATION 200
#define TAG_JUMP_ANIMATION 300
#define TAG_FALL_ANIMATION 400


class Player : public Sprite {
public:
    static Player* createPlayer(); // Hàm tạo nhân vật

    void update(float delta);
    void onKeyPressed(EventKeyboard::KeyCode keyCode);
    void onKeyReleased(EventKeyboard::KeyCode keyCode);
    void updateDirection();
    void playAttackAnimation();           // Hàm chơi animation tấn công
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
    void setHp(int value) { hp = value; }
    int getHp() { return hp; }
private:
    bool lastDirection = true;  // true: phải, false: trái
    Vec2 movingDirection;
    std::map<EventKeyboard::KeyCode, bool> keyStates;
    bool isAttacking = false;             // Trạng thái tấn công
    float speed = 300.0f;   // Tốc độ di chuyển cơ bản, có thể điều chỉnh
    float baseSpeed = 300.0f; // Lưu lại tốc độ ban đầu
    int hp = 4; // Máu ban đầu
    int maxHp = 4;
    bool key = false;
};

#endif // __PLAYER_H__
