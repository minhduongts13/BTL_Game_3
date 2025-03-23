/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

 #include "HelloWorldScene.h"
 #include "audio/include/AudioEngine.h"
 
 USING_NS_CC;
 
 Vec2 globalCharacterPos = Vec2(0, 0);
 
 Scene* HelloWorld::createScene()
 {
     // Tạo scene có physics
     auto scene = Scene::createWithPhysics();
    //  // (Tùy chọn) Tùy chỉnh trọng lực nếu cần, mặc định là (0, -980)
    //  scene->getPhysicsWorld()->setGravity(Vec2(0, -980));
 
    //  auto layer = HelloWorld::create();
    //  scene->addChild(layer);
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    auto layer = HelloWorld::create();
    layer->setPhysicWorld(scene->getPhysicsWorld());
    scene->getPhysicsWorld()->setGravity(Vec2(0, -300));
    scene->addChild(layer);
     return scene;
 }
 
 // Print useful error message instead of segfaulting when files are not there.
 static void problemLoading(const char* filename)
 {
     printf("Error while loading: %s\n", filename);
     printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
 }
 
 // on "init" you need to initialize your instance
 bool HelloWorld::init()
 {
     //////////////////////////////
     // 1. super init first
     if ( !Scene::initWithPhysics() )
     {
         return false;
     }
 
     auto visibleSize = Director::getInstance()->getVisibleSize();
     Vec2 origin = Director::getInstance()->getVisibleOrigin();
 
     initMusic();
 
     auto tileMap = TMXTiledMap::create("tile2/map4.tmx");
     if (tileMap == nullptr) {
         problemLoading("'tile2/map4.tmx'");
         return false;
     }
     /*tileMap->setAnchorPoint(Vec2(0.5, 0.5));*/
     /*tileMap->setPosition(visibleSize / 2);*/
 
     this->addChild(tileMap, 0);
     Size mapSize = tileMap->getContentSize();
 
     auto objectGroup = initObject(tileMap);
     if (objectGroup == nullptr) {
         problemLoading("objectGroup");
         return false;
     }
 
     // Tạo player
     auto player = Sprite::create("zombom2.jpg");
     // Ban đầu đặt player ở giữa màn hình
     player->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
 
     // Tạo physics body cho player
     auto playerBody = PhysicsBody::createBox(player->getContentSize());
     playerBody->setDynamic(true);
     // Đặt bitmask cho player (Category = 0x01)
     playerBody->setCategoryBitmask(0x01);
     // Cho phép nhận va chạm từ mọi thứ
     playerBody->setContactTestBitmask(0xFFFFFFFF);
     player->setPhysicsBody(playerBody);
     this->addChild(player, 1);
 
     // Tạo sensor nhỏ cho chân player với offset trực tiếp
     Size footSize = Size(player->getContentSize().width * 0.8f, 5);
     Vec2 offset(0, -player->getContentSize().height / 2);
 
     // Tạo physics body cho sensor với offset được truyền vào
     auto footSensor = PhysicsBody::createBox(footSize, PHYSICSBODY_MATERIAL_DEFAULT, offset);
     footSensor->setDynamic(false);
     footSensor->setGravityEnable(false);
     footSensor->setContactTestBitmask(0xFFFFFFFF);
 
     // Thêm sensor vào physics body của player
     playerBody->addShape(footSensor->getShape(0));
 
     // Giả sử playerBody đã được tạo cho player
     // Sensor trái:
     auto leftSensor = PhysicsBody::createBox(
         Size(5, player->getContentSize().height * 0.8f),
         PHYSICSBODY_MATERIAL_DEFAULT,
         Vec2(-player->getContentSize().width / 2, 0)
     );
     leftSensor->setDynamic(false);
     leftSensor->setGravityEnable(false);
     leftSensor->setContactTestBitmask(0xFFFFFFFF);
     leftSensor->getShape(0)->setSensor(true);
     playerBody->addShape(leftSensor->getShape(0));
 
     // Sensor phải:
     auto rightSensor = PhysicsBody::createBox(
         Size(5, player->getContentSize().height * 0.8f),
         PHYSICSBODY_MATERIAL_DEFAULT,
         Vec2(player->getContentSize().width / 2, 0)
     );
     rightSensor->setDynamic(false);
     rightSensor->setGravityEnable(false);
     rightSensor->setContactTestBitmask(0xFFFFFFFF);
     rightSensor->getShape(0)->setSensor(true);
     playerBody->addShape(rightSensor->getShape(0));
 
     // Các biến trạng thái (nên là biến thành viên của lớp)
     _isLeftPressed = false;
     _isRightPressed = false;
     _isUpPressed = false;
     _isDownPressed = false;
     // isOnGround sẽ được cập nhật qua contact listener
     this->isOnGround = false;
     this->isTouchingLeft = false;
     this->isTouchingRight = false;
 
     // Keyboard listener: cập nhật trạng thái phím
     auto keyboardListener = EventListenerKeyboard::create();
     keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
         switch (keyCode)
         {
         case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
             if (!this->isTouchingLeft) _isLeftPressed = true;
             break;
         case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
             if (!this->isTouchingRight) _isRightPressed = true;
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
             break;
         case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
             _isRightPressed = false;
             break;
         case EventKeyboard::KeyCode::KEY_SPACE:
             _isUpPressed = false;
             break;
         default:
             break;
         }
     };
 
     _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
 
     this->objectEvent(player);
 
     // Schedule cập nhật game: điều chỉnh tileMap và player
     this->schedule([this, tileMap, player, visibleSize](float dt) {
         // Nếu player có object hỗ trợ (isOnGround == true), cho phép di chuyển trái, phải, nhảy
         if (isOnGround)
         {
             // Nếu di chuyển trái/phải, player giữ nguyên x, tileMap di chuyển theo ngược lại
             if (_isLeftPressed || _isRightPressed)
             {
                 Vec2 tilePos = tileMap->getPosition();
                 float panSpeed = 200.0f;
                 float panStep = panSpeed * dt;
                 if (_isLeftPressed)
                     tilePos.x += panStep;
                 if (_isRightPressed)
                     tilePos.x -= panStep;
                 // Giới hạn tileMap nếu cần (bạn có thể dùng clampf)
                 tileMap->setPosition(tilePos);
                 // Giữ cho player nằm ở giữa màn hình theo trục x
                 Vec2 playerPos = player->getPosition();
                 playerPos.x = visibleSize.width / 2;
                 player->setPosition(playerPos);
             }
             // Nếu nhảy, chỉ cập nhật vị trí của player
             if (_isUpPressed)
             {
                 // Nếu đang đứng, thực hiện nhảy
                 if (isOnGround)
                 {
                     // Đặt trạng thái để không nhảy liên tục
                     isOnGround = false;
                     // Thiết lập vận tốc nhảy (giá trị có thể điều chỉnh)
                     player->getPhysicsBody()->setVelocity(Vec2(0, 400));
                 }
             }
         }
         else
         {
             // Nếu player không có object hỗ trợ, player sẽ rơi (với trọng lực)
             // Trong hầu hết trường hợp, hệ thống physics tự cập nhật vận tốc rơi do trọng lực,
             // nhưng bạn có thể theo dõi hoặc điều chỉnh thêm nếu cần.
             // Ví dụ: nếu player rơi quá nhanh, có thể giới hạn vận tốc.
             Vec2 vel = player->getPhysicsBody()->getVelocity();
             float maxFallSpeed = -600.0f;
             if (vel.y < maxFallSpeed)
             {
                 player->getPhysicsBody()->setVelocity(Vec2(vel.x, maxFallSpeed));
             }
         }
     }, "update_game");
 
 
     /*float scaleX = visibleSize.width / mapSize.width;
     float scaleY = visibleSize.height / mapSize.height;
 
     tileMap->setScale(scaleX, scaleY);*/
 
     /////////////////////////////
     // 2. add a menu item with "X" image, which is clicked to quit the program
     //    you may modify it.
 
     // add a "close" icon to exit the progress. it's an autorelease object
     /*auto closeItem = MenuItemImage::create(
                                            "CloseNormal.png",
                                            "CloseSelected.png",
                                            CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
 
     if (closeItem == nullptr ||
         closeItem->getContentSize().width <= 0 ||
         closeItem->getContentSize().height <= 0)
     {
         problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
     }
     else
     {
         float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
         float y = origin.y + closeItem->getContentSize().height/2;
         closeItem->setPosition(Vec2(x,y));
     }*/
 
     // create menu, it's an autorelease object
     /*auto menu = Menu::create(closeItem, NULL);
     menu->setPosition(Vec2::ZERO);
     this->addChild(menu, 1);*/
 
     /////////////////////////////
     // 3. add your codes below...
 
     // add a label shows "Hello World"
     // create and initialize a label
 
     //auto label = Label::createWithTTF("Hello World, I'm C++", "fonts/Marker Felt.ttf", 24);
     //if (label == nullptr)
     //{
     //    problemLoading("'fonts/Marker Felt.ttf'");
     //}
     //else
     //{
     //    // position the label on the center of the screen
     //    label->setPosition(Vec2(origin.x + visibleSize.width/2,
     //                            origin.y + visibleSize.height - label->getContentSize().height));
 
     //    // add the label as a child to this layer
     //    this->addChild(label, 1);
     //}
     return true;
 }
 
 void HelloWorld::initMusic()
 {
     AudioEngine::stopAll();
     AudioEngine::play2d("haha/Sealed Vessel.mp3", true, 0.5f);
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
 
         /*CCLOG("tileMap pos after: (%f\)", object["collision"].asString());*/
 
         float x = object["x"].asFloat();
         float y = object["y"].asFloat();
         float width = object["width"].asFloat();
         float height = object["height"].asFloat();
 
         // Nếu hệ tọa độ cần chuyển đổi, thực hiện chuyển đổi ở đây
         // float cocosY = tileMapHeight - y;
 
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
         physicsBody->setDynamic(false);  // Đặt là static nếu object không di chuyển
 
         // Thiết lập bitmask (ví dụ: object thuộc loại "object")
         physicsBody->setCategoryBitmask(categoryBitmask);
         physicsBody->setCollisionBitmask(collisionBitmask);
         physicsBody->setContactTestBitmask(0x01);
         objectNode->setPhysicsBody(physicsBody);
         this->addChild(objectNode);
     }
 
     return objectGroup;
 }
 
 void HelloWorld::objectEvent(cocos2d::Sprite* player)
 {
     // Tạo listener va chạm
     auto contactListener = EventListenerPhysicsContact::create();
 
     // Callback khi va chạm bắt đầu
     contactListener->onContactBegin = [this, player](PhysicsContact& contact) -> bool {
         // Lấy các physics body từ đối tượng va chạm
         auto bodyA = contact.getShapeA()->getBody();
         auto bodyB = contact.getShapeB()->getBody();
 
         // Lấy category bitmask của hai đối tượng
         int categoryA = bodyA->getCategoryBitmask();
         int categoryB = bodyB->getCategoryBitmask();
 
         // Kiểm tra xem sensor nào đã kích hoạt
         Vec2 sensorPosA = bodyA->getNode()->getPosition();
         Vec2 sensorPosB = bodyB->getNode()->getPosition();
 
         // Ví dụ: Log ra bitmask của 2 đối tượng
         CCLOG("Va chạm: BodyA Category = %d, BodyB Category = %d", categoryA, categoryB);
 
         // -----------------------------
         // Xử lý va chạm giữa player (0x01) và các object
         // -----------------------------
 
         // 1. Player đứng trên object cho phép đứng (bit 0x02)
         if ((categoryA == 0x01 && (categoryB & 0x02)) ||
             (categoryB == 0x01 && (categoryA & 0x02))) {
             CCLOG("Player đứng trên object cho phép đứng");
             /*
             // Nếu sensor bên trái chạm vào object collidable (với category khác player)
             if (bodyA->getCategoryBitmask() != 0x01 && sensorPosA.x < player->getPosition().x) {
                 isTouchingLeft = true;
             }
             if (bodyB->getCategoryBitmask() != 0x01 && sensorPosB.x < player->getPosition().x) {
                 isTouchingLeft = true;
             }
             // Tương tự, nếu sensor bên phải chạm:
             if (bodyA->getCategoryBitmask() != 0x01 && sensorPosA.x > player->getPosition().x) {
                 isTouchingRight = true;
             }
             if (bodyB->getCategoryBitmask() != 0x01 && sensorPosB.x > player->getPosition().x) {
                 isTouchingRight = true;
             }
             */
             this->isOnGround = true;
         }
 
         // 2. Player va chạm với object gây hại (dead, poison, bit 0x04)
         if ((categoryA == 0x01 && (categoryB & 0x04)) ||
             (categoryB == 0x01 && (categoryA & 0x04))) {
             CCLOG("Player chạm vào object gây hại - mất mạng");
             // Xử lý mất mạng, ví dụ: gọi hàm xử lý chết
             // this->handlePlayerDeath();
             this->isOnGround = true;
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
                 this->isOnGround = true;
             }
         }
 
         // 5. Player chạm vào object leo (climb, bit 0x20)
         if ((categoryA == 0x01 && (categoryB & 0x20)) ||
             (categoryB == 0x01 && (categoryA & 0x20))) {
             CCLOG("Player chạm vào object leo");
             // Xử lý logic cho phép player leo lên object, ví dụ: bật cờ cho phép di chuyển theo trục y
             // this->canClimb = true;  // canClimb là biến thành viên của lớp
             this->isOnGround = true;
         }
 
         // 6. Player iếp xúc với một object khác (không phải player)
         if ((categoryA == 0x01 && categoryB != 0x01) ||
             (categoryB == 0x01 && categoryA != 0x01))
         {
             this->isOnGround = true;
         }
 
         return true;
     };
 
     // (Tùy chọn) Callback khi va chạm kết thúc
     contactListener->onContactSeparate = [this](PhysicsContact& contact) {
         // Có thể xử lý logic khi va chạm kết thúc, ví dụ: hạ cờ isOnGround hoặc canClimb
         auto bodyA = contact.getShapeA()->getBody();
         auto bodyB = contact.getShapeB()->getBody();
         int categoryA = bodyA->getCategoryBitmask();
         int categoryB = bodyB->getCategoryBitmask();
 
          this->isOnGround = false;
 
         // Ví dụ: nếu player không còn tiếp xúc với đối tượng cho phép đứng
         if ((categoryA == 0x01 && (categoryB & 0x02)) ||
             (categoryB == 0x01 && (categoryA & 0x02))) {
             CCLOG("Player rời khỏi object cho phép đứng");
             this->isTouchingLeft = false;
             this->isTouchingRight = false;
         }
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
 