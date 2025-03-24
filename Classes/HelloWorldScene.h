#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Player.h"
#include "Monster.h"
#include "Chest.h"
#include "Boss.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene *createScene();

    bool OnPhysicsContact(cocos2d::PhysicsContact &contact);
    virtual bool init();
    void update(float delta);

    // a selector callback
    void menuCloseCallback(cocos2d::Ref *pSender);

    void initMusic();
    cocos2d::TMXObjectGroup *initObject(cocos2d::TMXTiledMap *tileMap);
    void initKeyboardListener();
    void initGameSchedule(TMXTiledMap* tileMap, Sprite* player, const Size& visibleSize);
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
    cocos2d::PhysicsWorld *world;
    void setPhysicWorld(cocos2d::PhysicsWorld *m_world)
    {
        world = m_world;
    }
    Player *player;
    Monster *monster1;
    Monster *monster2;
    bool isOnGround;
    Chest *chest;
    Item *item;
    Boss *boss;
    Vector<Monster *> monsters; // Khai báo biến monsters
    bool _isLeftPressed = false;
    bool _isRightPressed = false;
    bool _isUpPressed = false;
    bool _isDownPressed = false;
    bool _isLeftBlocked = false;
    bool _isRightBlocked = false;
    float _lastCorrectionTime;
    float _correctionInterval;
    bool flag = false;
    Camera* camera = nullptr;
    TMXTiledMap* tileMap=nullptr;
};

#endif // __HELLOWORLD_SCENE_H__
