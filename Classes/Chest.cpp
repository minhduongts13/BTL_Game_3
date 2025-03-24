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

void Chest::openChest() {
    if (!isOpened) {
        isOpened = true;
        this->setTexture("Object/Chest/chest_opened.png"); // Đổi ảnh thành thùng mở
        if (this->getPhysicsBody()) {
            this->getPhysicsBody()->removeFromWorld();
            this->removeComponent(this->getPhysicsBody());
        }
        // Delay một chút rồi xóa thùng
        this->runAction(Sequence::create(DelayTime::create(1.0f), RemoveSelf::create(), nullptr));
    }
}

Item* Item::spawnItem(const Vec2& position)
{
    // Danh sách texture và tên item tương ứng
    std::vector<std::pair<std::string, std::string>> itemData = {
        {"Object/shoe.png",  "speed_boost"},
        {"Object/heart.png", "health"},
        {"Object/gold.png",  "gold"}
    };

    // Chọn ngẫu nhiên
    int randomIndex = rand() % itemData.size();
    std::string texturePath = itemData[randomIndex].first;
    std::string itemName = itemData[randomIndex].second;

    // Tạo sprite Item
    auto item = new (std::nothrow) Item();
    if (item && item->initWithFile(texturePath))
    {
        item->autorelease();
        item->setName(itemName);
        item->setAnchorPoint(Vec2(0, 0));
        item->setPosition(position);
        auto itemBody = PhysicsBody::createBox(item->getContentSize());
        itemBody->setDynamic(true);              // Để item chịu tác động vật lý (gravity, velocity)
        itemBody->setGravityEnable(true);        // Bật trọng lực
        itemBody->setCategoryBitmask(0x103);        // Tự quy ước bitmask cho item
        itemBody->setContactTestBitmask(0xFFFFFFFF);   // Cho phép gọi callback khi va chạm

        // Gắn physics body cho sprite
        item->setPhysicsBody(itemBody);

        // (Tuỳ ý) Đặt vận tốc ban đầu cho item để nó "bay" ra
        itemBody->setVelocity(Vec2(rand() % 100 - 50, 200));

        return item; // Trả về item đã autorelease
    }

    CC_SAFE_DELETE(item);
    return nullptr;
}