#include "Monster.h"

void Monster::setbasePosition(Vec2 position) {
    this->originalPosition = position;
}

void Monster::setWormHole(Vec2 left, Vec2 right) {
    this->leftHole = left;
    this->rightHole = right;
    //CCLOG(std::str(this->leftHole.x));
    CCLOG("leftHole.x: %f", this->leftHole.x);
}

void Monster::initFrame() {
    this->originalPosition = leftHole;
    auto body = this->getPhysicsBody();
    if (!body) {
        body = PhysicsBody::create();
        this->setPhysicsBody(body);
    }

    for (int i = 1; i <= 14; ++i) {
        std::string frameName = "Monster/wormhole/wh" + std::to_string(i) + ".png";

        auto sprite = Sprite::create(frameName);
        //sprite->setAnchorPoint(Vec2(0, 0));
        if (!sprite) {
            CCLOG("Lỗi: Không tìm thấy frame %s!", frameName.c_str());
            continue;
        }

        //sprite->setScale(0.1f);
        this->WormHoleAni.pushBack(sprite->getSpriteFrame()); //pushBack(sprite);
        //auto frameSize = spriteFrame->getRect().size; // Dùng getRect().size để lấy kích thước chính xác
        auto frameSize = sprite->getContentSize();
        this->frameSizes.push_back(frameSize);
    }

    for (int i = 1; i <= 5; ++i) {
        std::string frameName = "Boss/attack" + std::to_string(i) + ".png";
        //frameName += (  "-1.png" : "-2.png");
        auto sprite = Sprite::create(frameName);

        this->AttackAni.pushBack(sprite->getSpriteFrame());

        auto frameSize = sprite->getContentSize();
        this->AttackframeSizes.push_back(frameSize);
    }
    if (this->AttackAni.empty()) {
        CCLOG("AttackAni rỗng");
        return;
    }

    if (WormHoleAni.empty()) {
        CCLOG("Lỗi: Không có frame animation nào được tải!");
    }
}

void Monster::wormHoleAni() {
    this->originalPosition = this->rightHole;
    if (WormHoleAni.empty() || frameSizes.empty()) {
        CCLOG("Không có frame hoặc kích thước frame!");
        return;
    }

    int frameCount = WormHoleAni.size();
    auto body = this->getPhysicsBody();

    Vector<FiniteTimeAction*> actions;

    for (int i = 0; i < frameCount; ++i) {
        auto frame      = WormHoleAni.at(i);
        auto frameSize  = frameSizes.at(i); // Lấy kích thước frame tương ứng

        auto updateFrameAndShape = CallFunc::create([this, frame, frameSize, body, i]() {
            this->setSpriteFrame(frame);
            //this->setPosition(originalPosition);
            this->setPosition(originalPosition - Vec2(frameSize.width / 2, frameSize.height / 2));

            if (body && frameSize.width > 0 && frameSize.height > 0) {
                body->removeAllShapes(); // Xóa shape cũ trước khi thêm mới
                auto newShape = PhysicsShapeBox::create(frameSize);
                body->addShape(newShape);
                //body->setPositionOffset(Vec2(80, 30));
                body->setPositionOffset(Vec2(frameSize.width * 0.5f, frameSize.height * 0.5f));

            }
            });

        auto delay = DelayTime::create(0.2f);
        actions.pushBack(updateFrameAndShape);
        actions.pushBack(delay);
    }

    auto sequence = Sequence::create(actions);

    /*float moveDuration = frameCount * 0.3f;
    auto moveAction = MoveTo::create(moveDuration, Vec2(0,0));
    auto spawn = Spawn::create(sequence, moveAction, nullptr);
    spawn->setTag(TAG_WORMHOLE_ANIMATION);
    this->stopActionByTag(TAG_WORMHOLE_ANIMATION);
    this->runAction(spawn);*/
    sequence->setTag(TAG_WORMHOLE_ANIMATION);
    this->runAction(sequence);
    //this->attack(Vec2(0, 0));
    //auto loopAnimation = RepeatForever::create(sequence);
    //loopAnimation->setTag(TAG_WORMHOLE_ANIMATION); // Đặt tag để có thể dừng

    // Dừng animation cũ trước khi chạy animation mới
    //this->stopActionByTag(TAG_WORMHOLE_ANIMATION);
    //this->runAction(loopAnimation);
}

void Monster::stopWormHoleAni() {
    this->stopActionByTag(TAG_WORMHOLE_ANIMATION);
}

void Monster::attack(Vec2 target) {
    // Nếu đang có attack animation thì không cho chạy move animation
    
    if (this->getActionByTag(TAG_ATTACK_ANIMATION)) {
        return;
    }
    this->stopActionByTag(TAG_WORMHOLE_ANIMATION);

    auto animation  = Animation::createWithSpriteFrames(this->AttackAni, 0.1f);
    auto animate    = Animate::create(animation);

    auto moveAction = MoveTo::create(1.0f, target);
    auto attackMove = Spawn::createWithTwoActions(animate, moveAction);

    // Đặt tag để kiểm soát animation
    attackMove->setTag(TAG_ATTACK_ANIMATION);
    Size finalFrameSize = this->AttackframeSizes.back();
    // Cập nhật body vật lý với kích thước frame cuối
    auto body = this->getPhysicsBody();
    if (body && finalFrameSize.width > 0 && finalFrameSize.height > 0) {
        body->removeAllShapes(); // Xóa shape cũ trước khi thêm mới
        auto newShape = PhysicsShapeBox::create(finalFrameSize);
        body->addShape(newShape);
        body->setPositionOffset(Vec2(finalFrameSize.width * 0.5f, finalFrameSize.height * 0.5f));
    }

    ////Vẽ tâm body bằng DrawNode
    //auto drawNode = DrawNode::create();
    //Vec2 center = this->getPosition() + body->getPositionOffset(); // Tâm body
    //drawNode->drawDot(center, 5, Color4F::RED); // Vẽ chấm đỏ
    //this->getParent()->addChild(drawNode, 10);  // Thêm vào scene để hiển thị
    // **🔴 Tạo DrawNode để vẽ tâm frame**
    //auto drawNode = DrawNode::create();
    //this->getParent()->addChild(drawNode, 100);  // Đảm bảo vẽ trên cùng (Z-index cao)

    //auto updateFrameCenter = CallFunc::create([this, drawNode]() {
    //    Vec2 frameCenter = this->getPosition() + Vec2(this->getContentSize().width / 2, this->getContentSize().height / 2);
    //    drawNode->clear(); // Xóa chấm cũ
    //    drawNode->drawDot(frameCenter, 5, Color4F::RED); // Vẽ chấm đỏ tại tâm frame
    //    CCLOG("Frame Center: (%.2f, %.2f)", frameCenter.x, frameCenter.y); // Debug vị trí
    //    });

    // Khi hoàn thành, có thể gọi callback để dừng animation hoặc tiếp tục hành động khác
    auto finishCallback = CallFunc::create([this]() {
        this->stopActionByTag(TAG_ATTACK_ANIMATION);
        });

    // Tạo chuỗi hành động: Attack + Move → Callback kết thúc
    auto sequence = Sequence::create(attackMove, finishCallback, nullptr);

    // Chạy hành động
    this->runAction(sequence);
}

void Monster::startRepeatingWormHoleAni() {
    this->schedule([this](float) {
        this->wormHoleAni();
        }, 7.0f, "WORMHOLE_ANIMATION_KEY");

}

void Monster::update(float delta) {
    if (health>0) {
        //this->setPosition(originalPosition);
    }
    //this->updatePhysicsShapes();
}

void Monster::startShapeChangeScheduler() {
    this->schedule([this](float dt) {
        this->startRepeatingWormHoleAni(); // Gọi hàm thay đổi shape
        log("PhysicsShape đã được cập nhật!");
        }, 10.0f, "ShapeChangeScheduler"); // Cứ 10 giây gọi lại
}

void Monster::takeDamage(int damage) {
    CCLOG("takeDamage");
    // Callback sau khi animation xong
    // Giảm máu
    health -= damage;

    if (health > 0) {
        // Nếu còn máu, giữ nguyên hình ảnh và vị trí
        return;
    }
    auto callback = CallFunc::create([this]() {
        this->setTexture("Monster/mos2.png");
        // Cập nhật lại PhysicsBody mặc định
        if (this->getPhysicsBody()) {
            this->getPhysicsBody()->removeFromWorld();
            this->removeComponent(this->getPhysicsBody());
        }

        // Hitbox mặc định của sprite idle
        auto defaultSize = this->getContentSize();
        auto defaultBody = PhysicsBody::createBox(defaultSize, PhysicsMaterial(1.0f, 0.0f, 0.0f));
        defaultBody->setCollisionBitmask(2);
        defaultBody->setContactTestBitmask(true);
        defaultBody->setGravityEnable(true);
        this->setPhysicsBody(defaultBody);       
        });
    auto sequence = Sequence::create(DelayTime::create(0.1f), callback, nullptr);
    this->runAction(sequence);
}

Monster* Monster::spawnMonster() {
    Monster* monster = new Monster();
    if (monster && monster->initWithFile("Monster/mos1.png")) {
        monster->autorelease();
        //monster->setAnchorPoint(Vec2(0.5f, 0.5f));
        // Kích hoạt PhysicsBody cho Monster
        auto body = PhysicsBody::createBox(monster->getContentSize());
        body->setCollisionBitmask(2);
        body->setContactTestBitmask(true); // Cho phép bắt sự kiện va chạm
        body->setGravityEnable(false);// ko chịu tác động trọng lực
        //body->setDynamic(true); // Quái vật không bị trọng lực ảnh hưởng
        monster->setPhysicsBody(body);
        //monster->wormHoleAni();
        monster->initFrame();
        return monster;
    }
    CC_SAFE_DELETE(monster);
    return nullptr;
}


