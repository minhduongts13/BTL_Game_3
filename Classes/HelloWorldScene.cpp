#include "HelloWorldScene.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

Scene *HelloWorld::createScene()
{
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    auto layer = HelloWorld::create();
    layer->setPhysicWorld(scene->getPhysicsWorld());
    scene->getPhysicsWorld()->setGravity(Vec2(0, -300));
    scene->addChild(layer);
    return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char *filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if (!Scene::initWithPhysics())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // auto edgeBody = PhysicsBody::createEdgeBox(visibleSize, PHYSICSBODY_MATERIAL_DEFAULT, 3);
    // auto edgeNode = Node::create();
    // edgeNode->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    // edgeBody->setCollisionBitmask(10);
    // edgeBody->setContactTestBitmask(true);
    // edgeNode->setPhysicsBody(edgeBody);
    // addChild(edgeNode);
    //// Tạo player
    // player = Player::createPlayer();
    // player->setPosition(Vec2(200, 200));
    // this->addChild(player);

    // Khởi tạo nhạc nền
    initMusic();

    // Tạo tile map từ file TMX
    tileMap = TMXTiledMap::create("tile2/map4.tmx");
    if (tileMap == nullptr)
    {
        problemLoading("'tile2/map4.tmx'");
        return false;
    }
    tileMap->setPosition(Vec2(-200, 0));
    tileMap->setCameraMask((unsigned short)CameraFlag::DEFAULT);
    this->addChild(tileMap, 0);

    // Tạo object group từ tile map (nếu có)
    auto objectGroup = initObject(tileMap);
    if (objectGroup == nullptr)
    {
        problemLoading("objectGroup");
        return false;
    }
    // --- Kết thúc phần MAP ---

    // --- Phần nhân vật từ code đầu tiên ---
    // Giả sử bạn đã có lớp Player riêng với hàm tạo đã tích hợp physics body
    player = Player::createPlayer();
    player->setPosition(Vec2(200, 200));
    this->addChild(player);
    // Tạo camera với kích thước màn hình
    camera = Camera::createOrthographic(visibleSize.width, visibleSize.height, 1, 1000);
    camera->setPosition(player->getPosition());
    this->addChild(camera);

    // Thêm player vào gameLayer
    //tileMap->addChild(player);

    //// Khởi tạo Follow Action sau khi đã thêm player vào scene
    //Rect worldRect = Rect(0, 0, tileMap->getMapSize().width, tileMap->getMapSize().height);
    //auto followAction = Follow::create(player, worldRect);
    //tileMap->runAction(followAction);
    // Tạo monster
    /*monster1 = FlyingMonster::spawnMonster(Vec2(400, 200));
    this->addChild(monster1);*/

    /*monster2 = GroundMonster::spawnMonster(Vec2(400, 0));
    this->addChild(monster2);*/

    /*chest = Chest::createChest(Vec2(300, 35));
    this->addChild(chest);*/


    initKeyboardListener();
    //this->objectEvent(player, tileMap);
    initGameSchedule(tileMap, player, visibleSize);
    //auto listener = EventListenerKeyboard::create();
    //listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event *event)
    //{
    //    player->onKeyPressed(keyCode);
    //};
    //listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event *event)
    //{
    //    player->onKeyReleased(keyCode);
    //};

    //this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    // Thiết lập sự kiện va chạm
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::OnPhysicsContact, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    // objectEvent();

    //// Thiết lập camera (nếu cần)
    //_camera = Camera::createOrthographic(visibleSize.width, visibleSize.height, 1, 1000);
    //_camera->setCameraFlag(CameraFlag::USER1);
    //_camera->setPosition3D(Vec3(player->getPosition().x, player->getPosition().y, 500));
    // this->addChild(_camera);

    // this->scheduleUpdate();

    return true;
}
// Hàm xử lý nhạc nền (từ code thứ hai)
void HelloWorld::initMusic()
{
    AudioEngine::stopAll();
    //AudioEngine::play2d("haha/Sealed Vessel.mp3", true, 0.5f);
}
// Hàm khởi tạo object từ tile map (từ code thứ hai)
TMXObjectGroup *HelloWorld::initObject(TMXTiledMap *tileMap)
{
    auto objectGroup = tileMap->getObjectGroup("Object Layer");
    if (objectGroup == nullptr)
    {
        log("Lỗi: Không tìm thấy object group");
        return nullptr;
    }

    auto objects = objectGroup->getObjects();
    for (auto &obj : objects)
    {
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

        int categoryBitmask = 0x00;
        int collisionBitmask = 0x01;

        if (collidable == "true")
            categoryBitmask |= 0x02;
        if (fly == "true")
            categoryBitmask |= 0x02;
        if (dead == "true")
            categoryBitmask |= 0x04;
        if (poison == "true")
            categoryBitmask |= 0x04;
        if (hit == "true")
            categoryBitmask |= 0x08;
        if (open == "true")
            categoryBitmask |= 0x08;
        if (take == "true")
            categoryBitmask |= 0x08;
        if (push == "true")
            categoryBitmask |= 0x10;
        if (climb == "true")
            categoryBitmask |= 0x20;

        auto objectNode = Node::create();
        objectNode->setPosition(Vec2(x + width / 2, y + height / 2));

        auto physicsBody = PhysicsBody::createBox(Size(width, height));
        physicsBody->setDynamic(false);
        physicsBody->setCategoryBitmask(categoryBitmask);
        // physicsBody->setCollisionBitmask(collisionBitmask);
        physicsBody->setContactTestBitmask(0x01);
        objectNode->setPhysicsBody(physicsBody);
        tileMap->addChild(objectNode);
    }
    return objectGroup;
}

bool HelloWorld::OnPhysicsContact(cocos2d::PhysicsContact &contact)
{
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();

    // Lấy category bitmask của hai đối tượng
    int categoryA = bodyA->getCategoryBitmask();
    int categoryB = bodyB->getCategoryBitmask();
    CCLOG("A: %i, B: %i", categoryA, categoryB);
    if ((categoryA == 0x100 && categoryB == 0x01) || (categoryA == 0x01 && categoryB == 0x100) || (categoryA == 0x101 && categoryB == 0x01) || (categoryA == 0x01 && categoryB == 0x101))
    {
        CCLOG("Player đã va chạm với Monster!");
        // Xử lý logic khi va chạm, ví dụ trừ máu player hoặc xóa monster
        if (player)
        {
            if (categoryA == 0x01)
            {
                player->takeDamage(((Monster *)bodyB->getNode())->damage);
            }
            if (categoryB == 0x01)
            {
                player->takeDamage(((Monster *)bodyA->getNode())->damage);
            }
        }
        return true;
    }

    if ((categoryA == 0x100 && categoryB == 0x80) || (categoryA == 0x80 && categoryB == 0x100))
    {
        CCLOG("Player đã va chạm với Monster!");
        // Xử lý logic khi va chạm, ví dụ trừ máu player hoặc xóa monster
        if (monster1)
            monster1->takeDamage(player->damage);
        return true;
    }

    if ((categoryA == 0x101 && categoryB == 0x80) || (categoryA == 0x80 && categoryB == 0x101))
    {
        CCLOG("Player đã va chạm với Monster!");
        // Xử lý logic khi va chạm, ví dụ trừ máu player hoặc xóa monster
        if (monster2)
            monster2->takeDamage(player->damage);
        if (monster2 && monster2->isDead)
        {
            this->scheduleOnce([&](float dt)
                               {
                monster2 = GroundMonster::spawnMonster(Vec2(400, 0));
                this->addChild(monster2); }, 1.5f, "check_groundmonster");
            // muốn tạo mới phải thêm scheduleOnce này để đợi sau khi khung body bị xóa tránh lỗi
        }
        return true;
    }
    if ((categoryA == 0x01 && categoryB == 0x02) || (categoryB == 0x01 && categoryA == 0x02) || (categoryA == 0x80 && categoryB == 0x02) || (categoryB == 0x80 && categoryA == 0x02))
    {
        /*CCLOG("Player đã va chạm với Monster!");
        player->jumpCount = 0;*/
        CCLOG("Player đứng trên object cho phép đứng");
        player->onGroundContact();
        return true;
    }

    if ((categoryA == 0x102 && categoryB == 0x80) || (categoryA == 0x80 && categoryB == 0x102))
    { // Player va vào Chest
        CCLOG("Player đã đập thùng!");
        // Chest* chest = nullptr;
        if (categoryA == 0x102)
            chest = (Chest *)bodyA->getNode();
        else if (categoryB == 0x102)
            chest = (Chest *)bodyB->getNode();

        if (chest)
        {
            chest->openChest(); // Mở chest (đổi ảnh, spawn vật phẩm bên trong, ...)
            Vec2 tempPosition = chest->getPosition();
            // Nếu cần tạo chest mới sau 1.5 giây
            this->scheduleOnce([tempPosition, this](float dt)
                               {
                // Spawn item ngay tại vị trí chest
                item = Item::spawnItem(tempPosition);
                this->addChild(item); }, 0.05f, "spawn_item");

            this->scheduleOnce([this](float dt)
                               {
                chest = Chest::createChest(Vec2(300, 35));
                this->addChild(chest); }, 1.5f, "spawn_chest");
        }
        return true;
    }

    if ((categoryA == 0x103 && categoryB == 0x01) || (categoryA == 0x01 && categoryB == 0x103))
    {
        CCLOG("Player picked up an item!");
        Node *item = (categoryA == 0x103) ? bodyA->getNode() : bodyB->getNode();
        if (item)
        {
            std::string itemName = item->getName();
            if (itemName == "speed_boost")
            {
                CCLOG("Player got speed boost!");
                player->increaseSpeed();
            }
            else if (itemName == "health")
            {
                CCLOG("Player got health!");
                player->heal(20);
            }
            else if (itemName == "gold")
            {
                CCLOG("Player got gold!");
                player->addGold(10);
            }
            item->runAction(Sequence::create(FadeOut::create(0.01f), RemoveSelf::create(), nullptr));
        }
        return true;
    }
    if ((categoryA == 0x02 && categoryB == 0x103) || (categoryA == 0x103 && categoryB == 0x02))
    {
        return true;
    }

    return false;
}


void HelloWorld::initKeyboardListener() {
    // Keyboard listener: cập nhật trạng thái phím
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        player->onKeyPressed(keyCode);
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            _isLeftPressed = true;
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            _isRightPressed = true;
            break;
            // Dùng phím SPACE để nhảy
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
            _isUpPressed = true;
            break;
        default:
            break;
        }
        };

    keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        player->onKeyReleased(keyCode);
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
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
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
        //isOnGround = leftBorder || rightBorder;
        isOnGround = true;
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
                //isOnGround = false;
                CCLOG("Raycast: isOnGround = false");
                /*Vec2 tilePos = tileMap->getPosition();
                float panSpeed = 200.0f;
                tilePos.y -= panSpeed * dt;
                tileMap->setPosition(tilePos);*/
                //player->getPhysicsBody()->setVelocity(Vec2(0, 200));
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
            //// Lấy kích thước màn hình
            //Size visibleSize = Director::getInstance()->getVisibleSize();

            //// Chuyển đổi vị trí của player sang hệ tọa độ thế giới (nếu player không thuộc gameLayer trực tiếp)
            //Vec2 playerWorldPos = player->getParent()->convertToWorldSpace(player->getPosition());

            //// Kiểm tra nếu player vượt qua 1/2 màn hình theo chiều y
            //if (playerWorldPos.y > visibleSize.height / 2 && !flag) {
            //    // Tính độ dịch chuyển: 1/4 chiều cao màn hình
            //    float shiftY = visibleSize.height / 4;

            //    // Giả sử gameLayer là node cha chứa tất cả các đối tượng cần di chuyển
            //    Vec2 currentPos = tileMap->getPosition();
            //    tileMap->setPosition(Vec2(currentPos.x, currentPos.y - shiftY));
            //    playerWorldPos.y -= 180;
            //    flag = true;
            //}
            //else if (playerWorldPos.y < visibleSize.height / 2 && flag) {
            //    // Khi player quay xuống dưới ngưỡng, reset flag để cho phép dịch chuyển tiếp theo
            //    flag = false;
            //}
            //CCLOG("playerWorldPos.y :%f, tileMap.y: %f, flag: %d", playerWorldPos.y, tileMap->getPositionY(), flag);

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

void HelloWorld::update(float delta)
{
    /*if (player && _camera)
    {
        Vec3 camPos = _camera->getPosition3D();
        camPos.x = player->getPosition().x;
        camPos.y = player->getPosition().y;
        _camera->setPosition3D(camPos);
    }*/
    // Tính bán kính màn hình
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float halfWidth = visibleSize.width / 2;
    float halfHeight = visibleSize.height / 2;

    // Lấy vị trí player làm tham chiếu
    Vec2 targetPos = player->getPosition();

    // Giới hạn vị trí theo biên của map
    float clampedX = clampf(targetPos.x, halfWidth, tileMap->getMapSize().width - halfWidth);
    float clampedY = clampf(targetPos.y, halfHeight, tileMap->getMapSize().height - halfHeight);

    camera->setPosition(Vec2(clampedX, clampedY));

    if (!player->isDead)
        player->update(delta);
}

void HelloWorld::menuCloseCallback(Ref *pSender)
{
    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    // EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
