#include "Wormhole.h"

Wormhole* Wormhole::createWormhole(const std::string& prefix, int frameCount, float frameDelay) {
    Wormhole* wormhole = new Wormhole();
    if (wormhole && wormhole->initWormhole(prefix, frameCount, frameDelay)) {
        wormhole->autorelease();
        return wormhole;
    }
    CC_SAFE_DELETE(wormhole);
    return nullptr;
}

bool Wormhole::initWormhole(const std::string& prefix, int frameCount, float frameDelay) {
    if (!Node::init()) return false;

    // Khởi tạo sprite đầu tiên
    wormholeSprite = cocos2d::Sprite::create(prefix + "1.png");
    this->addChild(wormholeSprite);

    // Tạo animation frames
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;
    for (int i = 1; i <= frameCount; ++i) {
        std::string frameName = prefix + std::to_string(i) + ".png";
        animFrames.pushBack(cocos2d::Sprite::create(frameName)->getSpriteFrame());
    }

    // Tạo animation
    auto animation = cocos2d::Animation::createWithSpriteFrames(animFrames, frameDelay);
    auto animate = cocos2d::Animate::create(animation);

    // Tạo sequence để thực hiện callback sau animation
    auto sequence = cocos2d::Sequence::create(animate, nullptr);
    wormholeSprite->runAction(sequence);

    return true;
}

void Wormhole::playAnimation() {
    if (wormholeSprite) {
        //wormholeSprite->runAction(RepeatForever::create(wormholeSprite->getActionByTag(0)));
    }
}
