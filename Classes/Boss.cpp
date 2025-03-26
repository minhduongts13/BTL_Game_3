#include "Boss.h"

Boss* Boss::create(Vec2 left, Vec2 right) {
    Boss* boss = new Boss();
    if (boss) {
        boss->initWithFile("Boss/att5.png");
        boss->autorelease();
        boss->setWormHole(left, right);
        auto body = PhysicsBody::createBox(boss->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.0f));
        body->setCollisionBitmask(8);
        body->setContactTestBitmask(0xfff);
        body->setGravityEnable(false);
        body->setDynamic(true);
        body->setRotationEnable(false);

        boss->setPhysicsBody(body);
        boss->initFrame();
        boss->setPosition(right);
        boss->setAnchorPoint(Vec2(0.5f, 0.5f)); // Đặt lại anchor point ở giữa
        boss->setVisible(false);
        return boss;
    }
    CC_SAFE_DELETE(boss);
    return nullptr;
}

void Boss::setWormHole(Vec2 left, Vec2 right) {
    this->leftHole  = left;
    this->rightHole = right;
    this->left  = WormHole::create(left);
    this->right = WormHole::create(right);
    CCLOG("set WormHole thanh cong");
}

void Boss::initFrame() {
    for (int i = 1; i <= 5; ++i) {
        std::string frameName = "Boss/att" + std::to_string(i) + ".png";
        auto sprite = Sprite::create(frameName);
        this->AttackAni.pushBack(sprite->getSpriteFrame());
    }
    for (int i = 1; i <= 7; ++i) {
        std::string frameName = "Boss/die" + std::to_string(i) + ".png";
        auto sprite = Sprite::create(frameName);
        //sprite->setScale(0.5f);
        this->DieAni.pushBack(sprite->getSpriteFrame());
    }
}

void Boss::attack1() {
    if (isattack || isdie) return;
    //this->stopAllActions();
    isTwo = false;
    this->stopAllActions();
    this->setRotation(0);
    bool isleft = rand() % 2; // 0 == phải , 1 == trái chọn random bên để render
    if (health <= 0) {
        //this->DieAnimation();
        return;
    }  
    isattack = true;
    this->left->start();
    this->right->start();
    // Chạy ngẫu nhiên bên
    if (isleft) {
        this->left->setFlippedX(true);
        this->setFlippedX(true);
        this->setPosition(leftHole);
    }
    else {
        this->right->setFlippedX(false);
        this->setFlippedX(false);
        this->setPosition(rightHole);
    }
        

    this->setVisible(false);
    auto delayBeforePlay = DelayTime::create(1.0f); // Đợi 1 giây trước khi play

    auto playWormhole = CallFunc::create([this, isleft]() {
        if (isleft) this->left->play();
        else this->right->play(); // Chạy Wormhole sau khi start hoàn tất
    });

    auto animation = Animation::createWithSpriteFrames(AttackAni, 0.1f);
    auto animate = Animate::create(animation);
    auto delay = DelayTime::create(0.5f);
    
    auto startAttack = CallFunc::create([this, isleft]() {
        //this->right->start();
        if (isleft) this->left->start();
        else this->right->start();
        auto lastFrame = AttackAni.back();
        this->setSpriteFrame(lastFrame);
        this->setVisible(true);

        auto moveAction = MoveTo::create(0.7f, this->target);
        auto hideAction = CallFunc::create([this, isleft]() {
            this->setVisible(false);
            //this->right->start();
            if (isleft) this->left->setVisible(true);
            else this->right->setVisible(true);
            this->setPosition(isleft ? this->leftHole : this->rightHole);
        });

        auto attackSequence = Sequence::create(moveAction, hideAction, nullptr);
        this->runAction(attackSequence);
    });

    // Chạy Wormhole -> Play Wormhole -> Attack Animation -> Tấn công
    auto sequence = Sequence::create(delayBeforePlay, playWormhole, animate, delay, startAttack, nullptr);
    this->runAction(sequence);
    isattack = false;
}

void Boss::setTarget(Vec2 tar) {
    this->target = tar;
}

int Boss::getHealth() {
    return this->health;
}

//void Boss::update(float delta, Vec2 tar) {
//    if (health > 0) {
//        this->target = tar;
//        if(isTwo)  this->setPosition(this->getPosition() + velocity);
//    } else if (!isdie && !isattack) this->DieAnimation();
//}

void Boss::update(float delta, Vec2 tar) {
    if (health > 0) {
        this->target = tar;
        if (isTwo) {
            this->setPosition(this->getPosition() + velocity);

            // Lấy kích thước màn hình
            Size screenSize = cocos2d::Director::getInstance()->getVisibleSize();
            float radius = this->getContentSize().width / 2;

            // Kiểm tra va chạm với tường
            if (this->getPositionX() - radius < 0 || this->getPositionX() + radius > screenSize.width) {
                velocity.x *= -1;
            }
            if (this->getPositionY() - radius < 0) {
                velocity.y *= -1;
                this->setPositionY(radius);  // Giữ Boss trong màn hình
            }
            if (this->getPositionY() + radius > screenSize.height) {
                velocity.y *= -1;
            }
        }
    }
    else if (!isdie && !isattack) {
        this->DieAnimation();
    }
}

void Boss::attack2() {
    if (isdie || isattack) return;
    isTwo = true;
    isattack = true;
    this->stopAllActions();
    this->right->start();
    this->left->start();
    this->setVisible(true);
    Vec2 newPos = (rand() % 2) ? rightHole : leftHole;
    this->setPosition(newPos);
    this->getPhysicsBody()->setDynamic(false);
    this->setSpriteFrame(Sprite::create("Boss/boss2.png")->getSpriteFrame());

    auto rotateAction = RotateBy::create(1.0f, 360); // Xoay 360 độ trong 1 giây
    auto repeatRotate = RepeatForever::create(rotateAction);

    this->stopAllActions();  // Dừng các action cũ để tránh lỗi
    this->runAction(repeatRotate);  // Chạy xoay mãi mãi
    isattack = false;
    //isTwo = false;
}

void Boss::DieAnimation() {
    isdie = true;
    if (this->DieAni.empty()) {
        CCLOG("Không có frame animation nào cho die!");
        return;
    }
    this->stopAllActions();
    this->setVisible(true);
    this->setRotation(0);
    this->setPosition(target + Vec2(100, 0));

    // Tạo animation từ danh sách frame
    auto animation = Animation::createWithSpriteFrames(this->DieAni, 0.1f); // 0.1s mỗi frame
    auto animate = Animate::create(animation);
    this->setVisible(true);
    auto lastFrame = DieAni.back();
    this->setSpriteFrame(lastFrame);
    auto delay = DelayTime::create(2.0f);
    // Ẩn Boss sau khi animation kết thúc
    auto hideBoss = CallFunc::create([this]() {
        this->left->stop();
        this->right->stop();
        this->getPhysicsBody()->setContactTestBitmask(0);
        this->getPhysicsBody()->setCollisionBitmask(0);
        this->getPhysicsBody()->setDynamic(false);
        this->removeComponent(this->getPhysicsBody());
        this->setVisible(false);
    });

    // Chạy animation và sau đó ẩn Boss
    auto sequence = Sequence::create(animate,delay,hideBoss, nullptr);
    this->runAction(sequence);
}

void Boss::takeDamage(int damage) {
    CCLOG("takeDamage");
    health -= damage;

    if (health > 0) return;

    //auto callback = CallFunc::create([this]() {
    //    this->setTexture("Monster/mos3-1.png");
    //    });

    //auto sequence = Sequence::create(DelayTime::create(0.1f), callback, nullptr);
    //this->runAction(sequence);
    //this->stopAttackWithWormHole();
    //this->DieAnimation();
}

WormHole* WormHole::create(Vec2 pos) {
    WormHole* whole = new WormHole();
    if (whole) {
        whole->autorelease();
        whole->setScale(0.5f);
        whole->initFrame();
        whole->setAnchorPoint(Vec2(0.5f, 0.5f));
        whole->setPosition(pos);
        whole->setSpriteFrame(Sprite::create("WormHole/wh14.png")->getSpriteFrame());
        whole->setVisible(false);
        whole->position = pos;
        return whole;
    }
    CC_SAFE_DELETE(whole);
    return nullptr;
}

void WormHole::initFrame() {
    for (int i = 1; i <= 14; ++i) {
        std::string frameName = "WormHole/wh" + std::to_string(i) + ".png";
        auto sprite = Sprite::create(frameName);
        //sprite->setScale(0.5f);
        if(i <= 10) this->startAni.pushBack(sprite->getSpriteFrame());
        else        this->WormHoleAni.pushBack(sprite->getSpriteFrame());
    }
}

void WormHole::start() {
    this->stop();
    this->setVisible(true);
    auto animation  = Animation::createWithSpriteFrames(startAni, 0.1f);
    auto animate    = Animate::create(animation);

    // Giữ frame cuối và xoay liên tục
    auto setLastFrame = CallFunc::create([this]() {
        this->setSpriteFrame(startAni.at(9)); // Frame thứ 10 (index = 9)
        auto rotateAction = RepeatForever::create(RotateBy::create(0.7f, 360));
        rotateAction->setTag(TAG_START_WH);
        this->runAction(rotateAction); // Xoay liên tục
    });

    // Chạy animation rồi xoay mãi
    auto sequence = Sequence::create(animate, setLastFrame, nullptr);
    sequence->setTag(TAG_START_WH);
    this->runAction(sequence);
    //this->setVisible(false);
}

void WormHole::play() {
    //this->stopActionByTag(TAG_START_WH);
    this->stop();
    this->setVisible(true);
    this->setRotation(0);
    auto animation  = Animation::createWithSpriteFrames(WormHoleAni, 0.2f);
    auto animate    = Animate::create(animation);
    animate->setTag(TAG_PLAY_WH);
    this->runAction(animate);
}

void WormHole::stop() {
    this->stopAllActions();
    this->stopActionByTag(TAG_PLAY_WH);
    this->stopActionByTag(TAG_START_WH);
    this->setVisible(false);
}
