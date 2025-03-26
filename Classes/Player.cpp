#include "Player.h"

Player* Player::createPlayer() {
    Player* player = new Player();
    if (player && player->initWithFile("Knight/knight1-1.png")) {
        player->autorelease();
        Size fixedSize(90, 140);
        auto body = PhysicsBody::createBox(fixedSize, PhysicsMaterial(1.0f, 0.0f, 1.0f), Vec2(0, 0));
        body->setCategoryBitmask(0x01);
        body->setContactTestBitmask(0xFFFFFFFF);
        body->setDynamic(true);
        body->setGravityEnable(true);
        body->setRotationEnable(false);
        player->setPhysicsBody(body);
        
        return player;
    }
    CC_SAFE_DELETE(player);
    return nullptr;
}

void Player::onKeyPressed(EventKeyboard::KeyCode keyCode) {
    keyStates[keyCode] = true; // Đánh dấu phím đang được nhấn
    updateDirection();

    // Bắt phím tấn công "D"
    if (keyCode == EventKeyboard::KeyCode::KEY_D && !this->getActionByTag(TAG_JUMP_ANIMATION) && !isAttacking) {
        if (movingDirection.x != 0 || movingDirection.y != 0) {
            this->stopActionByTag(TAG_MOVE_ANIMATION);
        }
        isAttacking = true;
        playAttackAnimation();
    }
    if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW && !this->getActionByTag(TAG_ATTACK_ANIMATION)) {
        jump();
    }

}

void Player::onKeyReleased(EventKeyboard::KeyCode keyCode) {
    keyStates[keyCode] = false; // Đánh dấu phím đã được nhả
    updateDirection();

    // Khi nhả phím D, nếu đang di chuyển thì cho animation move chạy lại
    if (keyCode == EventKeyboard::KeyCode::KEY_D) {
        isAttacking = false;
        if (movingDirection.x != 0 || movingDirection.y != 0) {
            if (!this->getActionByTag(TAG_MOVE_ANIMATION)) {
                playMoveAnimation(lastDirection);
            }
        }
    }
}

void Player::updateDirection() {
    movingDirection = Vec2::ZERO; // Reset hướng di chuyển

    if (keyStates[EventKeyboard::KeyCode::KEY_LEFT_ARROW]) {
        lastDirection = false;    // Lưu hướng cuối cùng là trái
        movingDirection.x -= 1;
    }
    if (keyStates[EventKeyboard::KeyCode::KEY_RIGHT_ARROW]) {
        lastDirection = true;     // Lưu hướng cuối cùng là phải
        movingDirection.x += 1;
    }
    if (keyStates[EventKeyboard::KeyCode::KEY_UP_ARROW]) {
        movingDirection.y += 1;
    }
    if (keyStates[EventKeyboard::KeyCode::KEY_DOWN_ARROW]) {
        movingDirection.y -= 1;
    }

    updateAnimationPriority();
    // Chuẩn hóa vector để di chuyển mượt hơn
    if (movingDirection != Vec2::ZERO) {
        movingDirection.normalize();
    }
}

void Player::updateAnimationPriority() {
    // Nếu đang attack, ưu tiên giữ attack animation
    if (this->getActionByTag(TAG_ATTACK_ANIMATION)) {
        return;
    }
    // Nếu đang nhảy, ưu tiên jump animation
    if (this->getActionByTag(TAG_JUMP_ANIMATION)) {
        return;
    }
    // Nếu đang nhảy, ưu tiên jump animation
    if (this->getActionByTag(TAG_FALL_ANIMATION)) {
        return;
    }
    // Nếu nhân vật chưa chạm đất (đang rơi) thì không chạy move animation
    if (!isOnGround) {
        return;
    }
    // Nếu có input di chuyển theo trục x, chạy move animation nếu chưa chạy
    if (fabs(movingDirection.x) > 0) {
        if (!this->getActionByTag(TAG_MOVE_ANIMATION)) {
            playMoveAnimation(lastDirection);
        }
    }
    else {
        // Nếu không có input di chuyển, dừng move animation (chuyển về idle)
        if (this->getActionByTag(TAG_MOVE_ANIMATION)) {
            stopMovingAnimation();
        }
    }
}


void Player::update(float delta) {
    if (isDead) return;
    if (movingDirection != Vec2::ZERO) {
        Vec2 newPosition = this->getPosition() + movingDirection * speed * delta;
        this->setPosition(newPosition);
    }
    if (!isOnGround &&
        !this->getActionByTag(TAG_JUMP_ANIMATION) &&
        !this->getActionByTag(TAG_ATTACK_ANIMATION) &&
        !this->getActionByTag(TAG_FALL_ANIMATION)) {
        playFallAnimation(lastDirection);
    }
}


void Player::playAttackAnimation() {
    if (this->getActionByTag(TAG_FALL_ANIMATION)) {
        this->stopActionByTag(TAG_FALL_ANIMATION);
    }
    //if (this->getActionByTag(TAG_ATTACK_ANIMATION)) return; // Nếu đã có attack animation thì không chạy lại
    // Xóa PhysicsBody hiện tại nếu có
    if (this->getPhysicsBody()) {
        this->getPhysicsBody()->removeFromWorld();
        this->removeComponent(this->getPhysicsBody());
    }
    std::string Knightattack = (lastDirection) ? "Knight/knight2-1.png" : "Knight/knight2-2.png";
    // Đổi texture thành hình tấn công
    this->setTexture(Knightattack); 

    // Tạo hitbox tấn công mới, có kích thước dựa theo kích thước sprite hiện tại
    auto attackBody = PhysicsBody::createBox(this->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.0f));
    attackBody->setDynamic(false);
    attackBody->setGravityEnable(true);
    attackBody->setCategoryBitmask(0x80);
    attackBody->setContactTestBitmask(0xFFFFFFFF);
    attackBody->setRotationEnable(false);
    this->setPhysicsBody(attackBody);

    // Sau một khoảng thời gian ngắn, chuyển lại về trạng thái mặc định
    auto revertCallback = CallFunc::create([this]() {
        // Xóa hitbox tấn công hiện tại
        if (this->getPhysicsBody()) {
            this->getPhysicsBody()->removeFromWorld();
            this->removeComponent(this->getPhysicsBody());
        }
        // Đổi texture về trạng thái mặc định
        std::string Knightdefault = (lastDirection) ? "Knightmove/knightmove1-1.png" : "Knightmove/knightmove1-2.png";
        this->setTexture(Knightdefault);
        auto defaultBody = PhysicsBody::createBox(Size(90, 140), PhysicsMaterial(1.0f, 0.0f, 0.0f));
        defaultBody->setDynamic(true);
        defaultBody->setGravityEnable(true);
        defaultBody->setCategoryBitmask(0x01);
        defaultBody->setContactTestBitmask(0xFFFFFFFF);
        defaultBody->setRotationEnable(false);

        this->setPhysicsBody(defaultBody);
        });
    auto spamCallback = CallFunc::create([this]() {
        if (isAttacking && jumpCount == 0) {
            playAttackAnimation();
        }
        else {
            if (movingDirection.x != 0 || movingDirection.y != 0) {
                playMoveAnimation(lastDirection);
            }
        }
        });
    auto sequence = Sequence::create(DelayTime::create(0.1f), revertCallback, DelayTime::create(0.1f), spamCallback, nullptr);
    sequence->setTag(TAG_ATTACK_ANIMATION);
    this->runAction(sequence);
}


void Player::playMoveAnimation(bool isMovingRight) {
    // Nếu đang có attack animation thì không cho chạy move animation
    if (this->getActionByTag(TAG_ATTACK_ANIMATION) || this->getActionByTag(TAG_ATTACK_ANIMATION) || this->getActionByTag(TAG_FALL_ANIMATION)) {
        return;
    }

    Vector<SpriteFrame*> animFrames;
    for (int i = 1; i <= 7; ++i) {
        std::string frameName = "Knightmove/knightmove" + std::to_string(i);
        frameName += (isMovingRight ? "-1.png" : "-2.png");
        animFrames.pushBack(Sprite::create(frameName)->getSpriteFrame());
    }

    auto animation = Animation::createWithSpriteFrames(animFrames, 0.1f);
    auto animate = Animate::create(animation);
    auto moveAction = RepeatForever::create(animate);
    moveAction->setTag(TAG_MOVE_ANIMATION);
    this->runAction(moveAction);
}

void Player::stopMovingAnimation() {
    this->stopActionByTag(TAG_MOVE_ANIMATION);
    std::string frameName = "Knightmove/knightmove1";
    frameName += (lastDirection ? "-1.png" : "-2.png");
    this->setTexture(frameName);
}


void Player::jump() {
    if (this->getActionByTag(TAG_FALL_ANIMATION)) {
        this->stopActionByTag(TAG_FALL_ANIMATION);
    }
    if (jumpCount < maxJumpCount) {
        isOnGround = false;
        jumpCount++;
        playJumpAnimation(lastDirection);
        float jumpVelocity = 300.0f; // Điều chỉnh giá trị này cho phù hợp
        Vec2 currentVelocity = this->getPhysicsBody()->getVelocity();
        currentVelocity.y = jumpVelocity;
        this->getPhysicsBody()->setVelocity(currentVelocity);
    }
}

// Hàm chạy animation nhảy
void Player::playJumpAnimation(bool isFacingRight) {
    if (jumpCount > 1 && this->getActionByTag(TAG_JUMP_ANIMATION)) {
        this->stopActionByTag(TAG_JUMP_ANIMATION);
    }
    else if (jumpCount == 1 && this->getActionByTag(TAG_JUMP_ANIMATION)) {
        return;
    }
    if (this->getActionByTag(TAG_JUMP_ANIMATION)) return;

    Vector<SpriteFrame*> jumpFrames;
    for (int i = 1; i <= 7; ++i) {
        std::string frameName = "Knightmove/up/" + std::to_string(i) + (isFacingRight ? "-1.png" : "-2.png");
        auto frame = Sprite::create(frameName)->getSpriteFrame();
        jumpFrames.pushBack(frame);
    }

    auto jumpAnim = Animation::createWithSpriteFrames(jumpFrames, 0.1f);
    auto jumpAnimate = Animate::create(jumpAnim);
    jumpAnimate->setTag(TAG_JUMP_ANIMATION);
    this->runAction(jumpAnimate);
}

void Player::playFallAnimation(bool isFacingRight) {
    if (this->getActionByTag(TAG_FALL_ANIMATION)) return;
    if (this->getActionByTag(TAG_MOVE_ANIMATION)) {
        this->stopActionByTag(TAG_MOVE_ANIMATION);
    }
    Vector<SpriteFrame*> fallFrames;
    for (int i = 1; i <= 7; ++i) {
        std::string frameName = "Knightmove/down/" + std::to_string(i);
        frameName += (isFacingRight ? "-1.png" : "-2.png");
        fallFrames.pushBack(Sprite::create(frameName)->getSpriteFrame());
    }
    for (int i = 6; i >= 1; --i) {
        std::string frameName = "Knightmove/down/" + std::to_string(i);
        frameName += (isFacingRight ? "-1.png" : "-2.png");
        fallFrames.pushBack(Sprite::create(frameName)->getSpriteFrame());
    }

    auto fallAnim = Animation::createWithSpriteFrames(fallFrames, 0.1f);
    auto fallAnimate = Animate::create(fallAnim);

    auto sequence = Sequence::create(fallAnimate, nullptr);
    sequence->setTag(TAG_FALL_ANIMATION);
    this->runAction(sequence);
}

void Player::onGroundContact() {
    jumpCount = 0;
    isOnGround = true;
    // Nếu đang chạy animation rơi, dừng nó ngay
    if (this->getActionByTag(TAG_FALL_ANIMATION)) {
        //CCLOG("Player đứng trên object cho phép đứng");
        this->stopActionByTag(TAG_FALL_ANIMATION); 
        std::string idleTexture = (lastDirection ? "Knightmove/knightmove1-1.png" : "Knightmove/knightmove1-2.png");
        this->setTexture(idleTexture);
        if (movingDirection.x != 0) {
            if (!this->getActionByTag(TAG_MOVE_ANIMATION)) {
                playMoveAnimation(lastDirection);
            }
        }
    }
}

void Player::takeDamage(int damage) {
    if (hp <= 0) return; // Nếu đã chết thì không nhận sát thương nữa

    hp -= damage;
    CCLOG("Player nhận %d sát thương, còn lại %d máu", damage, hp);

    // Hiệu ứng bị đánh (có thể đổi màu, rung, hoặc flash)
    this->setColor(Color3B::RED);
    auto revertColor = CallFunc::create([this]() { this->setColor(Color3B::WHITE); });
    this->runAction(Sequence::create(DelayTime::create(0.2f), revertColor, nullptr));

    if (hp <= 0) {
        die();
    }
}

void Player::die() {
    CCLOG("Player đã chết!");
    this->stopAllActions();
    isDead = true; // Đánh dấu Player đã chết
    // Xóa physics body để tránh va chạm tiếp theo
    if (this->getPhysicsBody()) {
        this->getPhysicsBody()->removeFromWorld();
        this->removeComponent(this->getPhysicsBody());
    }

    // Animation chết
    std::string deathTexture = "Knight/die1.png";
    this->setTexture(deathTexture);
    // Xoay nhân vật 90 độ sang phải
    this->setRotation(lastDirection?90:270);
    // Delay một chút rồi ẩn nhân vật
    auto fadeOut = FadeOut::create(1.0f);
    auto removeSelf = CallFunc::create([this]() { this->removeFromParentAndCleanup(true); });
    this->runAction(Sequence::create(fadeOut, removeSelf, nullptr));
}

void Player::increaseSpeed() {
    CCLOG("Player speed increased!");
    speed += 50.0f;
    // Reset sau 5 giây
    this->scheduleOnce([this](float dt) { this->resetSpeed(); }, 15.0f, "reset_speed");
}

void Player::resetSpeed() {
    speed = baseSpeed;
    CCLOG("Player speed reset to %f", speed);
}

void Player::heal(int amount) {
    hp += amount;
    if (hp > maxHp) hp = maxHp;
    CCLOG("Player healed, hp: %d", hp);
}

void Player::addGold(int amount) {
    gold += amount;
    CCLOG("Player gold: %d", gold);
}