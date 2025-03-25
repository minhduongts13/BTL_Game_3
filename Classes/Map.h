#ifndef __MAP_H__
#define __MAP_H__

#include "cocos2d.h"

class MapLayer : public cocos2d::Layer
{
public:
    virtual bool init();
    void update(float dt) override;
    // Hàm kiểm tra va chạm
    bool isCollidingWithObject(cocos2d::Vec2 pos);
    bool isTouchingSpike(cocos2d::Vec2 pos, int &damage);
    // Hàm chuyển tọa độ nếu cần
    // ...

    // Macro tạo instance
    CREATE_FUNC(MapLayer);

private:
    cocos2d::TMXTiledMap* tileMap;
    cocos2d::Size visibleSize;
    float mapPixelWidth;
    float mapPixelHeight;

    cocos2d::Vec2 playerPos;
    int playerHealth = 100;

    // Đối tượng dummy nhân vật
    cocos2d::Sprite* playerSprite;

    // Danh sách các object collision từ Tiled
    cocos2d::ValueVector collisionObjects;


    // Các cờ di chuyển
    bool moveLeft = false;
    bool moveRight = false;
    bool moveUp = false;
    bool moveDown = false;

    // Tốc độ di chuyển
    float speed = 150.0f; // pixel/giây, có thể điều chỉnh
};



#endif // __MAP_H__
