#include "Chest.h"

USING_NS_CC;

Chest* Chest::createChest(const Vec2& position) {
    Chest* chest = new (std::nothrow) Chest();
    if (chest && chest->initWithFile("Object/Chest/chest_closed.png")) { // Ảnh thùng đồ đóng
        chest->autorelease();
        chest->setPosition(position);

        // Thiết lập physics body
        auto body = PhysicsBody::createBox(chest->getContentSize(), PHYSICSBODY_MATERIAL_DEFAULT);
        body->setDynamic(false);
        body->setCategoryBitmask(0x102); // 6 là bitmask cho thùng đồ
        body->setContactTestBitmask(0xFFFFFFFF);
        chest->setPhysicsBody(body);
        chest->setAnchorPoint(Vec2(0, 0));
        return chest;
    }
    CC_SAFE_DELETE(chest);
    return nullptr;
}

Chest* Chest::createSpecialChest(const Vec2& position) {
    Chest* chest = new (std::nothrow) Chest();
    if (chest && chest->initWithFile("Thinh/treasure.png")) { // Ảnh thùng đồ đóng
        chest->autorelease();
        chest->setPosition(position);

        // Thiết lập physics body
        auto body = PhysicsBody::createBox(chest->getContentSize(), PHYSICSBODY_MATERIAL_DEFAULT);
        body->setDynamic(false);
        body->setCategoryBitmask(0x30); // 6 là bitmask cho thùng đồ
        body->setContactTestBitmask(0xFFFFFFFF);
        chest->setPhysicsBody(body);
        chest->setAnchorPoint(Vec2(0, 0));
        return chest;
    }
    CC_SAFE_DELETE(chest);
    return nullptr;
}

void Chest::openChest(std::string path) {
    if (!isOpened) {
        isOpened = true;
        this->setTexture(path); // Đổi ảnh thành thùng mở
        if (this->getPhysicsBody()) {
            this->getPhysicsBody()->removeFromWorld();
            this->removeComponent(this->getPhysicsBody());
        }
        // Delay một chút rồi xóa thùng
        this->runAction(Sequence::create(DelayTime::create(1.0f), RemoveSelf::create(), nullptr));
    }
}

Item* Item::spawnItem(const Vec2& position, ItemType type)
{
    // Danh sách các loại item
    std::vector<std::pair<std::string, std::string>> buffItems = {
        {"Object/shoe.png", "speed_boost"},
        {"Object/heart.png", "health"}
    };

    std::vector<std::pair<std::string, std::string>> currencyItems = {
        {"Object/gold.png", "gold"},
        {"Object/XP.png", "xp"}
    };

    // Chọn danh sách theo loại item
    std::vector<std::pair<std::string, std::string>>* itemList = nullptr;
    if (type == ItemType::BUFF) {
        itemList = &buffItems;
    }
    else if (type == ItemType::CURRENCY) {
        itemList = &currencyItems;
    }

    // Kiểm tra danh sách có hợp lệ không
    if (!itemList || itemList->empty()) {
        CCLOG("Không có item phù hợp!");
        return nullptr;
    }

    // Chọn ngẫu nhiên 1 item từ danh sách đã chọn
    int randomIndex = rand() % itemList->size();
    std::string texturePath = (*itemList)[randomIndex].first;
    std::string itemName = (*itemList)[randomIndex].second;

    // Tạo sprite Item
    auto item = new (std::nothrow) Item();
    if (item && item->initWithFile(texturePath))
    {
        item->autorelease();
        item->setName(itemName);
        item->setAnchorPoint(Vec2(0, 0));
        item->setPosition(position);

        auto itemBody = PhysicsBody::createBox(item->getContentSize());
        itemBody->setDynamic(true);
        itemBody->setGravityEnable(true);
        itemBody->setCategoryBitmask(0x103);
        itemBody->setContactTestBitmask(0xFFFFFFFF);

        // Gắn physics body
        item->setPhysicsBody(itemBody);

        // Đặt vận tốc bay ra ngẫu nhiên
        itemBody->setVelocity(Vec2(rand() % 100 - 50, 200));

        return item;
    }

    CC_SAFE_DELETE(item);
    return nullptr;
}
