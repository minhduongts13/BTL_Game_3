#include "Boss.h"

void Boss::setbasePosition(Vec2 position) {
    this->originalPosition = position;
}

void Boss::setWormHole(Vec2 left, Vec2 right) {
    this->leftHole = left;
    this->rightHole = right;
    CCLOG("leftHole.x: %f", this->leftHole.x);
}

void Boss::initFrame() {
    this->originalPosition = leftHole;

    for (int i = 1; i <= 14; ++i) {
        std::string frameName = "Monster/wormhole/wh" + std::to_string(i) + ".png";
        auto sprite = Sprite::create(frameName);
        if (!sprite) {
            CCLOG("Lỗi: Không tìm thấy frame %s!", frameName.c_str());
            continue;
        }
        this->WormHoleAni.pushBack(sprite->getSpriteFrame());
    }
    for (int i = 1; i <= 5; ++i) {
        std::string frameName = "Boss/attack" + std::to_string(i) + ".png";
        auto sprite = Sprite::create(frameName);
        this->AttackAni.pushBack(sprite->getSpriteFrame());
    }
}

void Boss::wormHoleAni() {
    if (WormHoleAni.empty()) {
        CCLOG("❌ Không có frame animation nào!");
        return;
    }

    this->originalPosition = this->rightHole;
    auto animation = Animation::createWithSpriteFrames(this->WormHoleAni, 0.2f);
    auto animate = Animate::create(animation);

    animate->setTag(TAG_WORMHOLE_ANIMATION);
    this->runAction(animate);
}
void Boss::stopWormHoleAni() {
    this->stopActionByTag(TAG_WORMHOLE_ANIMATION);
}

void Boss::attack(Vec2 target) {
    if (this->getActionByTag(TAG_ATTACK_ANIMATION)) return;

    this->stopActionByTag(TAG_WORMHOLE_ANIMATION);

    auto animation = Animation::createWithSpriteFrames(this->AttackAni, 0.1f);
    auto animate = Animate::create(animation);
    auto moveAction = MoveTo::create(1.0f, target);
    auto attackMove = Spawn::createWithTwoActions(animate, moveAction);

    attackMove->setTag(TAG_ATTACK_ANIMATION);

    auto finishCallback = CallFunc::create([this]() {
        this->stopActionByTag(TAG_ATTACK_ANIMATION);
        });

    auto sequence = Sequence::create(attackMove, finishCallback, nullptr);
    this->runAction(sequence);
}

void Boss::startRepeatingWormHoleAni() {
    this->schedule([this](float) {
        this->wormHoleAni();
        }, 7.0f, "WORMHOLE_ANIMATION_KEY");
}

void Boss::takeDamage(int damage) {
    CCLOG("takeDamage");
    health -= damage;

    if (health > 0) return;

    auto callback = CallFunc::create([this]() {
        this->setTexture("Monster/mos3-1.png");
        });

    auto sequence = Sequence::create(DelayTime::create(0.1f), callback, nullptr);
    this->runAction(sequence);
}

Boss* Boss::spawnMonster(Vec2 left, Vec2 right) {
    Boss* boss = new Boss();
    if (boss && boss->initWithFile("Monster/wormhole/wh1.png")) {
        boss->autorelease();
        boss->setWormHole(left, right);
        boss->setAnchorPoint(Vec2(0.0f, 0.0f));
        auto body = PhysicsBody::createBox(boss->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.0f));
        body->setCollisionBitmask(8);
        body->setContactTestBitmask(0xFFFFFFFF);
        body->setGravityEnable(false);
        body->setDynamic(false);
        boss->setPhysicsBody(body);
        boss->initFrame();
        boss->setPosition(right);
        return boss;
    }
    CC_SAFE_DELETE(boss);
    return nullptr;
}
