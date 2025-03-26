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

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    auto layer = HelloWorld::create();
    layer->setPhysicWorld(scene->getPhysicsWorld());
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
    if (!Scene::initWithPhysics())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    auto edgeBody = PhysicsBody::createEdgeBox(visibleSize, PHYSICSBODY_MATERIAL_DEFAULT, 3);
    auto edgeNode = Node::create();
    edgeNode->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    edgeNode->setPhysicsBody(edgeBody);
    addChild(edgeNode);
    // Tạo player
    player = Player::createPlayer();
    player->setPosition(Vec2(200, 0));
    this->addChild(player);
    this->addChild(player->fire);

    auto left   = Vec2(0.2f * visibleSize.width, 0.78f * visibleSize.height);
    auto right  = Vec2(visibleSize.width - (0.2f * visibleSize.width), 0.78f * visibleSize.height);
    boss    = Boss::create(left, right);
    this->addChild(boss);
    //boss->attack1();
    
    this->addChild(boss->right);
    this->addChild(boss->left);
    //boss->startAutoAttackWithWormHole(5.0f);
    //boss->attack2();
    //monster->startRepeatingWormHoleAni();
    //player->attack2(boss->getPosition());

    this->board = Board::create(true);
    this->live = Board::create(false);
    this->addChild(board);
    this->addChild(live);

  /*  this->hole = WormHole::create(right);
    this->addChild(hole);
    this->hole->start();*/

    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        player->onKeyPressed(keyCode);
        if (keyCode == EventKeyboard::KeyCode::KEY_SPACE) {
            CCLOG("Nhấn Space attack kiểu 2");
            //boss->removeFromParentAndCleanup(true);
            //boss = nullptr;
            //hole->stop();
            player->attack2();
            //boss->right->stop();
            //boss->left->stop();
        }        
        if (keyCode == EventKeyboard::KeyCode::KEY_E) {
            //CCLOG("Nhấn Space attack kiểu 2");
       /*     boss->right->start();
            boss->left->start();*/
            boss->attack1();
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_W) {
            //CCLOG("Nhấn Space attack kiểu 2");
            //hole->start();
            /*boss->right->play();
            boss->left->play();*/
            //boss->attack1();
            boss->attack2();
        }
        HP++;
        Mana++;
        };
    listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        player->onKeyReleased(keyCode);
        //hole->start();
        };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

   
    // Thiết lập sự kiện va chạm
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::OnPhysicsContact, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);


    this->scheduleUpdate();

    return true;
}


bool HelloWorld::OnPhysicsContact(cocos2d::PhysicsContact& contact) {
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();

    int maskA = bodyA->getCollisionBitmask();
    int maskB = bodyB->getCollisionBitmask();
    CCLOG("có va chạm");
    // Kiểm tra va chạm giữa Player (bitmask 1) và Monster (bitmask 2)
    if ((maskA == 3 && maskB == 2) || (maskA == 2 && maskB == 3)) {
        CCLOG("Player đã va chạm với Monster!");
        // Xử lý logic khi va chạm, ví dụ trừ máu player hoặc xóa monster
        boss->takeDamage(20);
        return true;
    }
    if ((maskA == 1 && maskB == 2) || (maskA == 2 && maskB == 1)) {
        CCLOG("Player đã va chạm với Monster!2");
        // Xử lý logic khi va chạm, ví dụ trừ máu player hoặc xóa monster
        boss->takeDamage(20);
        return true;
    }

    if ((maskA == 1 && maskB == 8) || (maskA == 8 && maskB == 1)) {
        CCLOG("Player đã va chạm với Monster!3");
        // Xử lý logic khi va chạm, ví dụ trừ máu player hoặc xóa monster
        boss->takeDamage(20);
        return true;
    }

    return false;
}

void HelloWorld::update(float delta) {
    player->update(delta);
    //if (boss) {
    //    if (boss->getHealth() < 0) {
    //        boss->DieAnimation();
    //        boss->removeFromParentAndCleanup(true);
    //        boss = nullptr;
    //    } else boss->update(delta, player->getPosition());
    //}
    HP = HP < maxHP ? HP : maxHP;
    Mana = Mana < maxMana ? Mana : maxMana;
    boss->update(delta, player->getPosition());
    live->update(delta,this->HP);
    //
    //int temp = (Mana / maxMana)*18;
    int index = static_cast<int>((static_cast<float>(Mana)/maxMana) * 17);
    //CCLOG("temp: %d", index);
    board->update(delta, index);
    //if(boss) boss->setTarget(player->getPosition());
    //monster->update(delta);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
