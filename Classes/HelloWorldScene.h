#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    void initMusic();
    cocos2d::TMXObjectGroup* initObject(cocos2d::TMXTiledMap* tileMap);
    void objectEvent(cocos2d::Sprite* player, cocos2d::TMXTiledMap* tileMap);
    void initKeyboardListener();
    void initGameSchedule(cocos2d::TMXTiledMap* tileMap, cocos2d::Sprite* player, const Size& visibleSize);

    cocos2d::PhysicsWorld* world;
    void setPhysicWorld(cocos2d::PhysicsWorld* m_world) {
        world = m_world;
    }
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
    bool _isLeftPressed = false;
    bool _isRightPressed = false;
    bool _isUpPressed = false;
    bool _isDownPressed = false;
    bool _isLeftBlocked = false;
    bool _isRightBlocked = false;
    float _lastCorrectionTime;
    float _correctionInterval;
};

#endif // __HELLOWORLD_SCENE_H__
