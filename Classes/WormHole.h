#ifndef __WORMHOLE_H__
#define __WORMHOLE_H__

#include "cocos2d.h"

class Wormhole : public cocos2d::Node {
public:
    static Wormhole* createWormhole(const std::string& prefix, int frameCount, float frameDelay);
    void playAnimation();  // Chạy animation wormhole

private:
    bool initWormhole(const std::string& prefix, int frameCount, float frameDelay);
    cocos2d::Sprite* wormholeSprite;
};

#endif // __WORMHOLE_H__
