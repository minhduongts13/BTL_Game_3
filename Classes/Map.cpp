#include "Map.h"
USING_NS_CC;

bool MapLayer::init() {
    if (!Layer::init()) return false;

    // Tạo tileMap
    tileMap = TMXTiledMap::create("tile2/map4.tmx");
    this->addChild(tileMap);

    // Lấy kích thước màn hình và map
    visibleSize = Director::getInstance()->getVisibleSize();
    auto mapSize = tileMap->getMapSize();
    auto tileSize = tileMap->getTileSize();
    mapPixelWidth  = mapSize.width  * tileSize.width;
    mapPixelHeight = mapSize.height * tileSize.height;
    
    // Lấy Object Layer để lấy đối tượng collision
    auto objectGroup = tileMap->getObjectGroup("Object Layer");
    if (objectGroup) {
        collisionObjects = objectGroup->getObjects();
        CCLOG("load được object layer");
    }

    // Tạo dummy sprite cho nhân vật
    playerSprite = Sprite::create("zombom2.jpg"); // Đảm bảo file dummy.png có trong Resources
    if (playerSprite) {
        // Đặt nhân vật ở giữa màn hình (có thể điều chỉnh)
        playerSprite->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
        this->addChild(playerSprite);
        playerPos = playerSprite->getPosition();
        // Lấy kích thước sprite
        auto spriteSize = playerSprite->getContentSize();

        // Giả sử anchor point mặc định (0.5, 0.5)
        // => center của sprite nằm tại playerPos
        // => bounding box = (pos.x - width/2, pos.y - height/2, width, height)
        Rect playerRect(
            playerPos.x - spriteSize.width / 2,
            playerPos.y - spriteSize.height / 2,
            spriteSize.width,
            spriteSize.height
        );

    } else {
        CCLOG("Không load được dummy.png");
    }

    // Lắng nghe sự kiện bàn phím
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        switch (keyCode) {
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            case EventKeyboard::KeyCode::KEY_A:
                moveLeft = true;
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            case EventKeyboard::KeyCode::KEY_D:
                moveRight = true;
                break;
            case EventKeyboard::KeyCode::KEY_UP_ARROW:
            case EventKeyboard::KeyCode::KEY_W:
                moveUp = true;
                break;
            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            case EventKeyboard::KeyCode::KEY_S:
                moveDown = true;
                break;
            default:
                break;
        }
    };
    listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        switch (keyCode) {
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            case EventKeyboard::KeyCode::KEY_A:
                moveLeft = false;
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            case EventKeyboard::KeyCode::KEY_D:
                moveRight = false;
                break;
            case EventKeyboard::KeyCode::KEY_UP_ARROW:
            case EventKeyboard::KeyCode::KEY_W:
                moveUp = false;
                break;
            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            case EventKeyboard::KeyCode::KEY_S:
                moveDown = false;
                break;
            default:
                break;
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    this->scheduleUpdate();
    return true;
}

void MapLayer::update(float dt) {
    // Tính vector di chuyển dựa trên phím
    Vec2 velocity;
    if (moveLeft)  velocity.x -= 10;
    if (moveRight) velocity.x += 10;
    if (moveUp)    velocity.y += 10;
    if (moveDown)  velocity.y -= 10;

    if (velocity.lengthSquared() > 0) {
        velocity.normalize();
    }

    // Tính quãng đường di chuyển
    Vec2 moveOffset = velocity * speed * dt;

    // Tính vị trí mới dựa trên di chuyển
    Vec2 newPos = playerPos + moveOffset;
    int damage = 0;
    if (isTouchingSpike(newPos, damage)) {
        // Giảm máu và có thể thêm hiệu ứng, âm thanh...
        playerHealth -= damage;
        CCLOG("Nhân vật bị spike, trừ máu: %d, health còn lại: %d", damage, playerHealth);
        // Nếu spike chỉ gây sát thương mà không chặn di chuyển, bạn vẫn cho phép di chuyển:
        // playerPos = newPos;
    }
    else if (!isCollidingWithObject(newPos)) {
        // Nếu không va chạm, cập nhật vị trí
        playerPos = newPos;
    }
    // Cập nhật vị trí của sprite nhân vật
    playerSprite->setPosition(playerPos);
    // CCLOG("Player position: %f, %f", playerSprite->getPosition().x, playerSprite->getPosition().y);
    // Di chuyển tileMap theo hướng camera (center nhân vật)
    Vec2 desiredPos = Vec2(visibleSize.width/2 - playerPos.x,
                           visibleSize.height/2 - playerPos.y);
    desiredPos.x = std::min(desiredPos.x, 0.0f);
    desiredPos.x = std::max(desiredPos.x, visibleSize.width - mapPixelWidth);
    desiredPos.y = std::min(desiredPos.y, 0.0f);
    desiredPos.y = std::max(desiredPos.y, visibleSize.height - mapPixelHeight);
    tileMap->setPosition(desiredPos);
}

bool MapLayer::isCollidingWithObject(Vec2 pos) {
    for (const auto& object : collisionObjects) {
        ValueMap obj = object.asValueMap();
        float x = obj["x"].asFloat();
        float y = obj["y"].asFloat();
        float width = obj["width"].asFloat();
        float height = obj["height"].asFloat();
        CCLOG("Collison object: %f, %f", x, y);
        // CCLOG("Check collision");
        Rect rect(x, y, width, height);
        // Chuyển đổi tọa độ y
        rect.origin.y = mapPixelHeight - (y + height);
        
        if (rect.containsPoint(pos)) {
            CCLOG("Collide");
            return true;
        }
    }
    if (pos.x < 0 || pos.x > mapPixelWidth || pos.y < 0 || pos.y > mapPixelHeight){
        return true;
    }
    return false;
}

bool MapLayer::isTouchingSpike(Vec2 pos, int &damage) {
    for (const auto& object : collisionObjects) {
        ValueMap obj = object.asValueMap();
        // Kiểm tra property spike
        if (obj.find("spike") != obj.end() && obj["spike"].asBool()) {
            float x = obj["x"].asFloat();
            float y = obj["y"].asFloat();
            float width = obj["width"].asFloat();
            float height = obj["height"].asFloat();
            
            // CCLOG("Check spike");
            Rect rect(x, y, width, height);
            rect.origin.y = mapPixelHeight - (y + height);

            if (rect.containsPoint(pos)) {
                if (obj.find("damage") != obj.end())
                    damage = obj["damage"].asInt();
                else
                    damage = 10;
                return true;
            }
        }
    }
    return false;
}
