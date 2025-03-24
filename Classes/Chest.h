#ifndef __CHEST_H__
#define __CHEST_H__

#include "cocos2d.h"

class Chest : public cocos2d::Sprite {
public:
    static Chest* createChest(const cocos2d::Vec2& position);

    void openChest(); // Hàm xử lý khi mở thùng

private:
    bool isOpened = false; // Trạng thái thùng
};
class Item : public cocos2d::Sprite
{
public:
    /**
     * @brief Tạo và trả về một item (Sprite) tại vị trí 'position'
     *        với loại (texture) ngẫu nhiên trong danh sách.
     *        Đồng thời gán physics body để item có thể tương tác vật lý.
     *
     * @param position Vị trí spawn item (trong hệ toạ độ của node cha nơi add item).
     * @return Con trỏ Item* đã được autorelease (dùng addChild(item) để thêm vào scene/layer).
     */
    static Item* spawnItem(const cocos2d::Vec2& position);
};
#endif // __CHEST_H__
