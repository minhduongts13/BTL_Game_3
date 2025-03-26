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

bool HelloWorld::init()
{
    if (!Layer::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Khởi tạo nhạc nền
    initMusic();

    // Tạo tile map từ file TMX
    tileMap = TMXTiledMap::create("tile2/map4.tmx");
    if (tileMap == nullptr)
    {
        problemLoading("'tile2/map4.tmx'");
        return false;
    }
    tileMap->setPosition(Vec2(-6000, -1800));
    tileMap->setCameraMask((unsigned short)CameraFlag::DEFAULT);
    this->addChild(tileMap, 0);

    // Tạo object group từ tile map (nếu có)
    auto objectGroup = initObject(tileMap);
    if (objectGroup == nullptr)
    {
        problemLoading("objectGroup");
        return false;
    }

    auto rock1 = Sprite::create("Thinh/rock.png"); // Tạo object từ hình ảnh
    if (rock1 == nullptr)
    {
        problemLoading("'Thinh/rock.png'");
        return false;
    }
    rock1->setPosition(Vec2(10000, 2800)); // Đặt vị trí ban đầu

    // Tạo PhysicsBody với kích thước dựa trên object
    auto body = PhysicsBody::createBox(rock1->getContentSize(), PhysicsMaterial(1.0f, 0.5f, 0.3f));
    body->setDynamic(true);  // Cho phép di chuyển
    body->setGravityEnable(true); // Nếu không muốn bị ảnh hưởng bởi trọng lực
    body->setCategoryBitmask(0x10);
    body->setCollisionBitmask(0x01 | 0x02 | 0x04); // Cài đặt va chạm
    body->setContactTestBitmask(0x01);

    rock1->setPhysicsBody(body); // Gán PhysicsBody vào object

    tileMap->addChild(rock1, 12345); // Thêm vào scene

    auto rock2 = Sprite::create("Thinh/rock.png"); // Tạo object từ hình ảnh
    if (rock2 == nullptr)
    {
        problemLoading("'Thinh/rock.png'");
        return false;
    }
    rock2->setPosition(Vec2(11300, 2800)); // Đặt vị trí ban đầu

    // Tạo PhysicsBody với kích thước dựa trên object
    body = PhysicsBody::createBox(rock2->getContentSize(), PhysicsMaterial(1.0f, 0.5f, 0.3f));
    body->setDynamic(true);  // Cho phép di chuyển
    body->setGravityEnable(true); // Nếu không muốn bị ảnh hưởng bởi trọng lực
    body->setCategoryBitmask(0x10);
    body->setCollisionBitmask(0x01 | 0x02 | 0x04); // Cài đặt va chạm
    body->setContactTestBitmask(0x01);

    rock2->setPhysicsBody(body); // Gán PhysicsBody vào object

    tileMap->addChild(rock2, 12345); // Thêm vào scene

    auto key = Sprite::create("Thinh/key.png"); // Tạo object từ hình ảnh
    if (key == nullptr)
    {
        problemLoading("'Thinh/key.png'");
        return false;
    }
    key->setPosition(Vec2(6100, 2800)); // Đặt vị trí ban đầu

    // Tạo PhysicsBody với kích thước dựa trên object
    body = PhysicsBody::createBox(key->getContentSize(), PhysicsMaterial(1.0f, 0.5f, 0.3f));
    body->setDynamic(true);  // Cho phép di chuyển
    body->setGravityEnable(true); // Nếu không muốn bị ảnh hưởng bởi trọng lực
    body->setCategoryBitmask(0x08);
    body->setCollisionBitmask(0x01 | 0x02); // Cài đặt va chạm
    body->setContactTestBitmask(0x01);

    key->setPhysicsBody(body); // Gán PhysicsBody vào object

    tileMap->addChild(key, 12345); // Thêm vào scene

    chest = Chest::createSpecialChest(Vec2(11700, 1800));
    if (chest == nullptr) {
        problemLoading("'Thinh/treasure.png'");
        return false;
    }
    tileMap->addChild(chest, 12345);

    // --- Kết thúc phần MAP ---

    // --- Phần nhân vật từ code đầu tiên ---
    // Giả sử bạn đã có lớp Player riêng với hàm tạo đã tích hợp physics body
    player = Player::createPlayer();
    player->setPosition(Vec2(200, 200));
    this->addChild(player);
    
    // Tạo monster
    /*monster1 = FlyingMonster::spawnMonster(Vec2(400, 200));
    this->addChild(monster1);*/

    monster2 = GroundMonster::spawnMonster(Vec2(2000, 0));
    tileMap->addChild(monster2,9999);//thay vì để nó làm con scene chính thì để làm con của tilemap.


    /*chest = Chest::createChest(Vec2(300, 35));
    this->addChild(chest);*/

   

    // Lưu ý: bạn có thể muốn trừ bớt kích thước màn hình nếu muốn camera dừng ở rìa map

    
    // Tạo Follow action
    auto followAction = Follow::create(player, Rect::ZERO);
    this->runAction(followAction);



    initKeyboardListener();
    initGameSchedule(tileMap, player, visibleSize);
    

    // Thiết lập sự kiện va chạm
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::OnPhysicsContact, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

     this->scheduleUpdate();

    return true;
}

// Hàm xử lý nhạc nền (từ code thứ hai)
void HelloWorld::initMusic()
{
    AudioEngine::stopAll();
    AudioEngine::play2d("haha/Sealed Vessel.mp3", true, 0.5f);
}

// Hàm khởi tạo object từ tile map (từ code thứ hai)
TMXObjectGroup *HelloWorld::initObject(TMXTiledMap *tileMap)
{
    // Sau khi khởi tạo tileMap trong init()
    //pushContainer = Node::create();
    //pushContainer->setPosition(Vec2::ZERO); // Vị trí ban đầu
    //this->addChild(pushContainer, 2);
        
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
        std::string spike = object["spike"].asString();
        std::string hit = object["hit"].asString();
        std::string fly = object["fly"].asString();
        std::string take = object["take"].asString();
        std::string dead = object["dead"].asString();
        std::string push = object["push"].asString();
        std::string climb = object["climb"].asString();

        float x = object["x"].asFloat();
        float y = object["y"].asFloat();
        float width = object["width"].asFloat();
        float height = object["height"].asFloat();

        auto objectNode = Node::create();
        objectNode->setPosition(Vec2(x + width / 2, y + height / 2));

        auto physicsBody = PhysicsBody::createBox(Size(width, height));
        physicsBody->setDynamic(false);

        /*auto pushContainer = Node::create();
        this->addChild(pushContainer);*/

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
        if (spike == "true")
            categoryBitmask |= 0x04;
        if (hit == "true")
            categoryBitmask |= 0x08;
        /*if (push == "true") {
            physicsBody->setDynamic(true);
            categoryBitmask |= 0x10;
            pushContainer->addChild(objectNode);
        }
        else {
            tileMap->addChild(objectNode);
        }*/
        if (climb == "true")
            categoryBitmask |= 0x20;

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
    // Lấy vị trí của player theo world space
    Vec2 playerWorldPos = player->getPosition();

    // Chuyển đổi sang hệ tọa độ của tileMap
    Vec2 rockSpawnPos = tileMap->convertToNodeSpace(playerWorldPos);

    CCLOG("positionX: % f, positionY : % f", rockSpawnPos.x, rockSpawnPos.y);
    
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

    if ((categoryA == 0x04 && categoryB == 0x01) || (categoryA == 0x01 && categoryB == 0x04))
    {
        CCLOG("Player đã dính bẫy!");
        // Xử lý logic khi va chạm, ví dụ trừ máu player hoặc xóa monster
        if (player)
        {
            int dame = 1;
            player->takeDamage(dame);
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
        this->isOnGround = true;
        return true;
    }

    if ((categoryA == 0x01 && categoryB == 0x10) || (categoryB == 0x01 && categoryA == 0x10))
    {
        CCLOG("Player đập đá");
        // Áp dụng impulse cho object (nếu object là dynamic)
        float impulseStrength = 500.0f;
        Vec2 impulse = Vec2(-impulseStrength, 0);
        if (categoryB == 0x01) bodyA->applyImpulse(impulse);
        else if (categoryA == 0x01) bodyB->applyImpulse(impulse);

        player->onGroundContact();
        this->isOnGround = true;

        return true;
    }

    if ((categoryA == 0x08 && categoryB == 0x01) || (categoryA == 0x01 && categoryB == 0x08))
    {
        CCLOG("Player lấy chìa khóa");
        player->setKey(true);
        Node* rockNode = (categoryA == 0x08) ? bodyA->getNode() : bodyB->getNode();
        if (rockNode)
        {
            rockNode->runAction(Sequence::create(FadeOut::create(0.1f), RemoveSelf::create(), nullptr));
        }
        return true;
    }

    if ((categoryA == 0x30 && categoryB == 0x01) || (categoryA == 0x01 && categoryB == 0x30))
    { // Player va vào Chest
        CCLOG("Player đã mở rương!");
        
        if (chest && player->getKey())
        {
            chest->openChest("Thinh/chest.png"); // Mở chest (đổi ảnh, spawn vật phẩm bên trong, ...)
            player->setHp(player->getHp() + 3);
        }
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
            chest->openChest("Object/Chest/chest_opened.png"); // Mở chest (đổi ảnh, spawn vật phẩm bên trong, ...)
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
    if (player->isDead) return;
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

void HelloWorld::initGameSchedule(TMXTiledMap* tileMap, Player* player, const Size& visibleSize)
{
    if (!player->isDead) {
        this->schedule([this, tileMap, player, visibleSize](float dt) {
            if (isOnGround)
            {
                /*CCLOG("Raycast: isOnGround = true");*/
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
                if (_isUpPressed)
                {
                    isOnGround = false;
                    /*CCLOG("Raycast: isOnGround = false");*/
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

            }
            else
            {
                /*CCLOG("Raycast: isOnGround = false");*/
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
                //if (player->getPositionY() < visibleSize.height*0.1) {
                //    Vec2 tilePos = tileMap->getPosition();
                //    float playerVelocityY = player->getPhysicsBody()->getVelocity().y; // Lấy tốc độ rơi hiện tại
                //    float panSpeed = -playerVelocityY * dt; // Điều chỉnh theo tốc độ rơi
                //    tilePos.y -= panSpeed;
                //    tileMap->setPosition(tilePos);
                //}


            }
        }, "update_game");
    }
}

void HelloWorld::update(float delta)
{
    if (!player->isDead) {
        player->update(delta);
        /*CCLOG("Mạng Player: %d", player->getHp());*/
    }
    else {
        AudioEngine::stopAll();
        auto menuScene = MenuScene::create();
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, menuScene));
    }
}

void HelloWorld::menuCloseCallback(Ref *pSender)
{
    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    // EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
