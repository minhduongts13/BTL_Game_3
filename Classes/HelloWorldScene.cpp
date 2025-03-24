#include "HelloWorldScene.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

Vec2 globalCharacterPos = Vec2(0, 0);

Scene* HelloWorld::createScene()
{
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    scene->getPhysicsWorld()->setGravity(Vec2(0, -980));

    auto layer = HelloWorld::create();
    layer->setPhysicWorld(scene->getPhysicsWorld());
    scene->addChild(layer);

    return scene;
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool HelloWorld::init()
{
    if ( !Scene::initWithPhysics() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    initMusic();

    auto tileMap = TMXTiledMap::create("map4.tmx");
    if (tileMap == nullptr) {
        problemLoading("'map4.tmx'");
		return false;
    }

    this->addChild(tileMap, 0);
    Size mapSize = tileMap->getContentSize();

	auto objectGroup = initObject(tileMap);
    if (objectGroup == nullptr) {
        problemLoading("objectGroup");
		return false;
    }

    // Tạo player
    auto player = Sprite::create("character.png");
    player->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

    // Các tham số của PhysicsMaterial: density, restitution, friction
    PhysicsMaterial material(0.5f, 0.5f, 0.1f);
    auto playerBody = PhysicsBody::createBox(player->getContentSize(), material);

    playerBody->setDynamic(true);
    // Đặt bitmask cho player (Category = 0x01)
    playerBody->setCategoryBitmask(0x01);
    // Cho phép nhận va chạm từ mọi thứ
    playerBody->setContactTestBitmask(0xFFFFFFFF);
    playerBody->setLinearDamping(1.0f);
    player->setPhysicsBody(playerBody);
    player->getPhysicsBody()->setRotationEnable(false);
    this->addChild(player, 1);

    initKeyboardListener();
    this->objectEvent(player, tileMap);
    initGameSchedule(tileMap, player, visibleSize);

    return true;
}

void HelloWorld::initKeyboardListener() {
    // Keyboard listener: cập nhật trạng thái phím
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            _isLeftPressed = true;
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            _isRightPressed = true;
            break;
            // Dùng phím SPACE để nhảy
        case EventKeyboard::KeyCode::KEY_SPACE:
            _isUpPressed = true;
            break;
        default:
            break;
        }
    };

    keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            _isLeftPressed = false;
            _isLeftBlocked = false;
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            _isRightPressed = false;
            _isLeftBlocked = false;
            break;
        case EventKeyboard::KeyCode::KEY_SPACE:
            _isUpPressed = false;
            break;
        default:
            break;
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void HelloWorld::initGameSchedule(TMXTiledMap* tileMap, Sprite* player, const Size& visibleSize)
{
    // Biến trạng thái kiểm tra dưới chân player
    bool isOnGround = false;

    // Schedule cập nhật game: điều chỉnh tileMap và player
    this->schedule([this, tileMap, player, visibleSize, &isOnGround](float dt) {
        // Tạo 2 tia raycast ở hai bên mép của player
        Vec2 leftStart = player->getPosition() + Vec2(-player->getContentSize().width / 2, -player->getContentSize().height / 2);
        Vec2 rightStart = player->getPosition() + Vec2(player->getContentSize().width / 2, -player->getContentSize().height / 2);

        // Tính điểm kết thúc cho ray (ở dưới collider của player)
        Vec2 leftEnd = leftStart + Vec2(0, -player->getContentSize().height / 2 - 5);
        Vec2 rightEnd = rightStart + Vec2(0, -player->getContentSize().height / 2 - 5);

        bool leftBorder = false;
        bool rightBorder = false;

        // Hàm rayCast callback
        auto rayCastFunc = [&leftBorder, &rightBorder, player](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) -> bool {
            if (info.shape->getBody() == player->getPhysicsBody()) return true; // Bỏ qua collider của chính player

            if ((info.shape->getBody()->getCategoryBitmask() & 0x02) && !info.shape->isSensor())
            {
                // Sử dụng data để xác định tia nào đang xét
                if (data == (void*)1) leftBorder = true;
                if (data == (void*)2) rightBorder = true;
                return false;
            }
            return true;
        };

        // Thực hiện raycast ở hai bên mép
        this->getScene()->getPhysicsWorld()->rayCast(rayCastFunc, leftStart, leftEnd, (void*)1);
        this->getScene()->getPhysicsWorld()->rayCast(rayCastFunc, rightStart, rightEnd, (void*)2);

        // Nếu ít nhất một tia chạm đất, player đang đứng
        isOnGround = leftBorder || rightBorder;

        if (isOnGround)
        {
            CCLOG("Raycast: isOnGround = true");
            // Nếu di chuyển trái/phải, chỉ cập nhật tileMap nếu không bị block
            if (_isLeftPressed && !_isLeftBlocked)
            {
                Vec2 tilePos = tileMap->getPosition();
                float panSpeed = 200.0f;
                tilePos.x += panSpeed * dt;
                tileMap->setPosition(tilePos);
            }
            if (_isRightPressed && !_isRightBlocked)
            {
                Vec2 tilePos = tileMap->getPosition();
                float panSpeed = 200.0f;
                tilePos.x -= panSpeed * dt;
                tileMap->setPosition(tilePos);
            }

            // Phần xử lý nhảy: nếu nhấn phím SPACE khi đang đứng
            if (_isUpPressed && isOnGround)
            {
                isOnGround = false;
                CCLOG("Raycast: isOnGround = false");
                player->getPhysicsBody()->setVelocity(Vec2(0, 400));
            }

            // Điều chỉnh vị trí player về giữa màn hình theo chiều X
            Vec2 desiredPosition = Vec2(visibleSize.width / 2, player->getPositionY());
            Vec2 currentPos = player->getPosition();
            float diffX = desiredPosition.x - currentPos.x;
            float threshold = 5.0f;

            if (fabs(diffX) > threshold) {
                float correctionSpeed = 50.0f;
                float correctionX = correctionSpeed * dt * (diffX > 0 ? 1 : -1);
                if (fabs(correctionX) > fabs(diffX)) correctionX = diffX;
                player->setPositionX(currentPos.x + correctionX);
            }
        }
        else
        {
            CCLOG("Raycast: isOnGround = false");
            // Khi không có object dưới chân, áp dụng giới hạn tốc độ rơi
            Vec2 vel = player->getPhysicsBody()->getVelocity();
            float maxFallSpeed = -600.0f;
            if (vel.y < maxFallSpeed)
            {
                player->getPhysicsBody()->setVelocity(Vec2(vel.x, maxFallSpeed));
            }
        }
    }, "update_game");
}

void HelloWorld::initMusic()
{
    AudioEngine::stopAll();
    AudioEngine::play2d("Sealed Vessel.mp3", true, 0.5f);
}

TMXObjectGroup* HelloWorld::initObject(TMXTiledMap* tileMap)
{
    auto objectGroup = tileMap->getObjectGroup("Object Layer");
	if (objectGroup == nullptr) {
		log("Lỗi: Không tìm thấy object group");
		return nullptr;
	}

    auto objects = objectGroup->getObjects();

    for (auto& obj : objects) {
        ValueMap object = obj.asValueMap();

        std::string collidable = object["collidable"].asString();
        std::string poison = object["poison"].asString();
        std::string hit = object["hit"].asString();
        std::string fly = object["fly"].asString();
        std::string open = object["open"].asString();
        std::string take = object["take"].asString();
        std::string dead = object["dead"].asString();
        std::string push = object["push"].asString();
        std::string climb = object["climb"].asString();

        float x = object["x"].asFloat();
        float y = object["y"].asFloat();
        float width = object["width"].asFloat();
        float height = object["height"].asFloat();

        // categoryBitmask = collisionBitmask = 0x01: player
        int categoryBitmask = 0x00;
        int collisionBitmask = 0x01; // Nếu mục tiêu là va chạm với player (player category = 0x01)

        if (collidable == "true") {
            categoryBitmask |= 0x02;
        }
        if (fly == "true") {
            categoryBitmask |= 0x02;
        }
        if (dead == "true") {
            categoryBitmask |= 0x04;  // Cho các đối tượng làm mất mạng nếu đứng lên
        }
        if (poison == "true") {
            categoryBitmask |= 0x04;
        }
        if (hit == "true") {
            categoryBitmask |= 0x08;  // Cho các đối tượng biến mất khi chạm
        }
        if (open == "true") {
            categoryBitmask |= 0x08;
        }
        if (take == "true") {
            categoryBitmask |= 0x08;
        }
        if (push == "true") {
			categoryBitmask |= 0x10;  // Cho các đối tượng có hiệu ứng đẩy
        }
        if (climb == "true") {
			categoryBitmask |= 0x20;  // Cho các đối tượng có hiệu ứng leo
        }

        // Tạo node cho object
        auto objectNode = Node::create();
        objectNode->setPosition(Vec2(x + width / 2, y + height / 2));

        // Tạo physics body dạng hình chữ nhật
        auto physicsBody = PhysicsBody::createBox(Size(width, height));
        physicsBody->setDynamic(false);

        // Thiết lập bitmask (ví dụ: object thuộc loại "object")
        physicsBody->setCategoryBitmask(categoryBitmask);
        physicsBody->setCollisionBitmask(collisionBitmask);
        physicsBody->setContactTestBitmask(0x01);
        objectNode->setPhysicsBody(physicsBody);
        tileMap->addChild(objectNode);
    }

    return objectGroup;
}

void HelloWorld::objectEvent(cocos2d::Sprite* player, TMXTiledMap* tileMap)
{
    // Tạo listener va chạm
    auto contactListener = EventListenerPhysicsContact::create();

    // Trong lớp HelloWorld, thêm biến thời gian:
    _lastCorrectionTime = 0.0f;
    _correctionInterval = 0.1f; // áp dụng correction mỗi 0.1 giây nếu collision vẫn tồn tại

    contactListener->onContactPreSolve = [this, tileMap, player](PhysicsContact& contact, PhysicsContactPreSolve& solve) -> bool {
        const PhysicsContactData* data = contact.getContactData();
        if (data) {
            Vec2 normal = data->normal;
            if (fabs(normal.x) > 0.1f) {
                // Lấy thời gian hiện tại
                float currentTime = Director::getInstance()->getTotalFrames() * Director::getInstance()->getAnimationInterval(); // hoặc một phương thức khác để lấy thời gian
                if (currentTime - _lastCorrectionTime >= _correctionInterval) {
                    if (normal.x < 0) {
                        tileMap->runAction(MoveBy::create(0.1f, Vec2(20, 0)));
                        CCLOG("Collision bên phải: map di chuyển sang phải");
                    }
                    else if (normal.x > 0) {
                        tileMap->runAction(MoveBy::create(0.1f, Vec2(-20, 0)));
                        CCLOG("Collision bên trái: map di chuyển sang trái");
                    }
                    _lastCorrectionTime = currentTime;
                }
            }
        }
        return true;
    };

    // Callback khi va chạm bắt đầu
    contactListener->onContactBegin = [this, player, tileMap](PhysicsContact& contact) -> bool {
        // Lấy các physics body từ đối tượng va chạm
        auto bodyA = contact.getShapeA()->getBody();
        auto bodyB = contact.getShapeB()->getBody();

        // Lấy category bitmask của hai đối tượng
        int categoryA = bodyA->getCategoryBitmask();
        int categoryB = bodyB->getCategoryBitmask();

        // Ví dụ: Log ra bitmask của 2 đối tượng
        CCLOG("Va chạm: BodyA Category = %d, BodyB Category = %d", categoryA, categoryB);

        // -----------------------------
        // Xử lý va chạm giữa player (0x01) và các object
        // -----------------------------

        // 1. Player đứng trên object cho phép đứng (bit 0x02)
        if ((categoryA == 0x01 && (categoryB & 0x02)) ||
            (categoryB == 0x01 && (categoryA & 0x02))) {
            CCLOG("Player đứng trên object cho phép đứng");

            const PhysicsContactData* contactData = contact.getContactData();
            if (contactData)
            {
                Vec2 normal = contactData->normal;
                // Kiểm tra xem va chạm có theo hướng ngang không:
                if (fabs(normal.x) > 0.1f)
                {
                    if (_isLeftPressed && normal.x > 0)
                    {
                        _isLeftBlocked = true;
                        tileMap->runAction(MoveBy::create(0.1f, Vec2(20, 0)));
                        CCLOG("Block trái: collision ngang");
                    }
                    if (_isRightPressed && normal.x < 0)
                    {
                        _isRightBlocked = true;
                        tileMap->runAction(MoveBy::create(0.1f, Vec2(-20, 0)));
                        CCLOG("Block phải: collision ngang");
                    }
                }
            }
        }

        // 2. Player va chạm với object gây hại (dead, poison, bit 0x04)
        if ((categoryA == 0x01 && (categoryB & 0x04)) ||
            (categoryB == 0x01 && (categoryA & 0x04))) {
            CCLOG("Player chạm vào object gây hại - mất mạng");
        }

        // 3. Player chạm vào object có hiệu ứng biến mất (hit, open, take, bit 0x08)
        if ((categoryA == 0x01 && (categoryB & 0x08)) ||
            (categoryB == 0x01 && (categoryA & 0x08))) {
            CCLOG("Player chạm vào object có hiệu ứng biến mất");
            // Lấy node của đối tượng va chạm, sau đó loại bỏ nó khỏi scene
            Node* collidedNode = (categoryA != 0x01) ? bodyA->getNode() : bodyB->getNode();
            if (collidedNode) {
                collidedNode->removeFromParentAndCleanup(true);
            }
        }

        // 4. Player chạm vào object đẩy (push, bit 0x10)
        // Xử lý va chạm đẩy: kiểm tra nếu player (0x01) chạm object có push (0x10)
        if ((categoryA == 0x01 && (categoryB & 0x10)) ||
            (categoryB == 0x01 && (categoryA & 0x10))) {

            CCLOG("Player chạm vào object đẩy");

            // Xác định node của object đẩy và node của player
            Node* pushNode = nullptr;
            Node* playerNode = nullptr;
            if (categoryA == 0x01) {
                playerNode = bodyA->getNode();
                pushNode = bodyB->getNode();
            }
            else {
                playerNode = bodyB->getNode();
                pushNode = bodyA->getNode();
            }

            if (pushNode && playerNode) {
                // Tính hướng đẩy: từ player đến object push
                Vec2 direction = pushNode->getPosition() - playerNode->getPosition();
                direction.normalize();

                // Xác định khoảng cách đẩy (ví dụ: 50 pixel) và thời gian di chuyển
                float pushDistance = 50.0f;
                float duration = 0.2f;

                // Tạo action MoveBy để đẩy object theo hướng đã tính
                auto moveAction = MoveBy::create(duration, direction * pushDistance);
                pushNode->runAction(moveAction);
            }
        }

        // 5. Player chạm vào object leo (climb, bit 0x20)
        if ((categoryA == 0x01 && (categoryB & 0x20)) ||
            (categoryB == 0x01 && (categoryA & 0x20))) {
            CCLOG("Player chạm vào object leo");
        }

        // 6. Player iếp xúc với một object khác (không phải player)
        if ((categoryA == 0x01 && categoryB != 0x01) ||
            (categoryB == 0x01 && categoryA != 0x01))
        {
            
        }

        return true;
    };

    // (Tùy chọn) Callback khi va chạm kết thúc
    contactListener->onContactSeparate = [this](PhysicsContact& contact) {
        if (_isLeftBlocked == true) _isLeftBlocked = false;
        else if (_isRightBlocked == true) _isRightBlocked = false;
        _lastCorrectionTime = 0.0f;
    };

    // Đăng ký listener với event dispatcher của scene
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
