#include "Monster.h"

void Monster::update(float delta) {
    /*if (health>0) {
        this->setPosition(originalPosition);
    }*/
}

FlyingMonster* FlyingMonster::spawnMonster(Vec2 position) {
    FlyingMonster* monster = new FlyingMonster();
    if (monster && monster->initWithFile("Monster/mos1-1.png")) {
        monster->autorelease();

        auto body = PhysicsBody::createBox(monster->getContentSize());
        body->setCategoryBitmask(0x100);
        //body->setCollisionBitmask(0x01);
        body->setContactTestBitmask(0xFFFFFFFF);
        body->setDynamic(false);
        body->setGravityEnable(false);
        body->setRotationEnable(false);
        monster->setPhysicsBody(body);

        monster->setPatrolPositions(Vec2(position.x - 100, position.y), Vec2(position.x + 100, position.y));
        monster->startPatrol(2.0f);

        return monster;
    }
    CC_SAFE_DELETE(monster);
    return nullptr;
}

void FlyingMonster::takeDamage(int damage) {
    CCLOG("takeDamage");
    // Giảm máu
    health -= damage;
    this->setColor(Color3B::RED);
    auto revertColor = CallFunc::create([this]() { this->setColor(Color3B::WHITE); });
    this->runAction(Sequence::create(DelayTime::create(0.1f), revertColor, nullptr));
    if (health > 0) {
        // Nếu còn máu, giữ nguyên hình ảnh và vị trí
        std::string picture = lastDirection ? "Monster/mos2-1.png" : "Monster/mos2-2.png";
        this->setTexture(picture);
        return;
    }

    // Nếu health <= 0, chuyển sang hoạt ảnh "chết"
    std::string picture = lastDirection ? "Monster/mos3-1.png" : "Monster/mos3-2.png";
    this->setTexture(picture);
    isDead = true;
    this->stopAllActions();
    // 🔥 XÓA physics body nếu có
    if (this->getPhysicsBody()) {
        this->getPhysicsBody()->removeFromWorld();
        this->removeComponent(this->getPhysicsBody());
    }
    // Tạo hiệu ứng rơi: di chuyển xuống dưới 100 đơn vị trong 1 giây với easing
    auto fallAction = MoveBy::create(1.0f, Vec2(0, -100));
    auto easeFall = EaseIn::create(fallAction, 2.0f);

    // Sau vài giây, monster sẽ tự biến mất khỏi scene
    auto disappearCallback = CallFunc::create([this]() {
        this->removeFromParentAndCleanup(true);
        });

    // Chờ 2 giây rồi gọi callback (bạn có thể điều chỉnh thời gian theo ý muốn)
    auto spawn = Spawn::create(easeFall, Sequence::create(DelayTime::create(1.0f), disappearCallback, nullptr),nullptr);
    this->runAction(spawn);
}

void Monster::setPatrolPositions(Vec2 startPos, Vec2 endPos) {
    this->patrolStart = startPos;
    this->patrolEnd = endPos;
    this->setPosition(startPos);
}

void FlyingMonster::startPatrol(float duration) {
    // Callback để thay đổi texture khi di chuyển về phía end
    auto setTextureToEnd = CallFunc::create([this]() {
        lastDirection = true;
        std::string picture = lastDirection ? "Monster/mos1-1.png" : "Monster/mos1-2.png";
        this->setTexture(picture);
        
        });
    // Di chuyển từ start đến end
    auto moveToEnd = MoveTo::create(duration, patrolEnd);

    // Callback để thay đổi texture khi di chuyển về phía start
    auto setTextureToStart = CallFunc::create([this]() {
        lastDirection = false;
        std::string picture = lastDirection ? "Monster/mos1-1.png" : "Monster/mos1-2.png";
        this->setTexture(picture);
        
        });
    // Di chuyển từ end về start
    auto moveToStart = MoveTo::create(duration, patrolStart);
    
    auto patrolSequence = Sequence::create(DelayTime::create(1.0f),
        setTextureToEnd, moveToEnd, DelayTime::create(1.0f),
        setTextureToStart, moveToStart, DelayTime::create(1.0f),
        nullptr
    );
    auto repeatPatrol = RepeatForever::create(patrolSequence);
    this->runAction(repeatPatrol);
}


GroundMonster* GroundMonster::spawnMonster(Vec2 position) {
    GroundMonster* monster = new GroundMonster();
    if (monster && monster->initWithFile("Monster/ground1-1.png")) {
        monster->autorelease();
        monster->setAnchorPoint(Vec2(0,0));
        auto body = PhysicsBody::createBox(monster->getContentSize());
        body->setCategoryBitmask(0x101);  // Khác với FlyingMonster
        //body->setCollisionBitmask(0x01);
        body->setContactTestBitmask(0xFFFFFFFF);
        body->setDynamic(false);
        body->setGravityEnable(false);
        body->setRotationEnable(false);
        monster->setPhysicsBody(body);

        monster->setPatrolPositions(Vec2(position.x - 100, position.y), Vec2(position.x + 100, position.y));
        monster->startPatrol(2.0f);
        
        return monster;
    }
    CC_SAFE_DELETE(monster);
    return nullptr;
}


void GroundMonster::startPatrol(float duration) {
    // Hàm khởi chạy animation di chuyển
    auto startAnimation = CallFunc::create([this]() {
        Vector<SpriteFrame*> frames;
        for (int i = 2; i <= 5; i++) {
            std::string frameName = "Monster/ground" + std::to_string(i);
            std::string frame = frameName + (lastDirection ? "-2.png" : "-1.png");
            frames.pushBack(Sprite::create(frame)->getSpriteFrame());
        }
        auto animation = Animation::createWithSpriteFrames(frames, 0.1f);
        auto animateAction = RepeatForever::create(Animate::create(animation));
        animateAction->setTag(100); // Gán tag cho action này
        this->runAction(animateAction);
        });

    // Hàm dừng animation và chuyển về ảnh mặc định
    auto stopAnimation = CallFunc::create([this]() {
        this->stopActionByTag(100);
        // Chuyển về ảnh mặc định dựa trên lastDirection
        if (lastDirection) {
            this->setTexture("Monster/ground1-2.png");
        }
        else {
            this->setTexture("Monster/ground1-1.png");
        }
        });

    // Di chuyển từ vị trí hiện tại đến patrolEnd
    auto setDirTrue = CallFunc::create([this]() {
        lastDirection = true;
        });
    auto moveToEnd = MoveTo::create(duration, patrolEnd);
    auto delayAtEnd = DelayTime::create(1.0f);
    auto seqToEnd = Sequence::create(setDirTrue, startAnimation, moveToEnd, stopAnimation, delayAtEnd, nullptr);

    // Di chuyển từ vị trí hiện tại đến patrolStart
    auto setDirFalse = CallFunc::create([this]() {
        lastDirection = false;
        });
    auto moveToStart = MoveTo::create(duration, patrolStart);
    auto delayAtStart = DelayTime::create(1.0f);
    auto seqToStart = Sequence::create(setDirFalse, startAnimation, moveToStart, stopAnimation, delayAtStart, nullptr);

    // Lặp lại vô hạn hai hành trình trên
    auto patrolSequence = Sequence::create(seqToEnd, seqToStart, nullptr);
    auto repeatPatrol = RepeatForever::create(patrolSequence);
    this->runAction(repeatPatrol);
}


void GroundMonster::takeDamage(int damage) {
    // Giảm máu
    health -= damage;
    this->setColor(Color3B::RED);
    auto revertColor = CallFunc::create([this]() { this->setColor(Color3B::WHITE); });
    this->runAction(Sequence::create(DelayTime::create(0.1f), revertColor, nullptr));
    if (health > 0) {
        return;  // Nếu còn máu, không làm gì thêm
    }

    // Nếu máu dưới 0, chuyển sang trạng thái chết
    std::string picture = lastDirection ? "Monster/grounddie-2.png" : "Monster/grounddie-1.png";
    this->setTexture(picture);
    isDead = true;

    // Dừng tất cả các hành động
    this->stopAllActions();
    // 🔥 XÓA physics body nếu có
    if (this->getPhysicsBody()) {
        this->getPhysicsBody()->removeFromWorld();
        this->removeComponent(this->getPhysicsBody());
    }
    // Cho ảnh mờ dần trong 1.5 giây, sau đó xoá khỏi scene
    auto fadeOut = FadeOut::create(1.0f);
    auto removeCallback = CallFunc::create([this]() {
        this->removeFromParentAndCleanup(true);
        });

    auto sequence = Sequence::create(fadeOut, removeCallback, nullptr);
    this->runAction(sequence);
}
