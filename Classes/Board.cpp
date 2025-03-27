#include "Board.h"

Board* Board::create(bool isboard) {
    Board* board = new Board();
    board->setIsBoard(isboard);
    if (board) { // Thay bằng hình mặc định
        board->autorelease();
        if (isboard) {
            board->initWithFile("Board/board_0.png");
            board->setAnchorPoint(Vec2(0.0f, 0.0f));
            board->setScale(1.0f);
            Size visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 origin = Director::getInstance()->getVisibleOrigin();
            board->setPosition(Vec2(0, visibleSize.height - board->getContentSize().height / 1.5 - 70));  // Góc trên
            //board->position = board->getPosition();
        } else {
            board->initWithFile("Board/li0.png");
            board->setAnchorPoint(Vec2(0.0f, 0.0f));
            board->setScale(2.0f);
            Size visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 origin = Director::getInstance()->getVisibleOrigin();
            board->setPosition(Vec2(150, visibleSize.height - board->getContentSize().height/2 - 135));  // Góc trên
        }
        board->position = board->getPosition();
        board->initFrame();
        return board;
    }
    CC_SAFE_DELETE(board);
    return nullptr;
}

void Board::setIsBoard(bool isboard) {
    this->isboard = isboard;
}

void Board::initFrame() {
    // Load các frame animation của Board (nếu có)
    if (this->isboard) {
        for (int i = 0; i <= 17; ++i) { 
            std::string frameName = "Board/board_" + std::to_string(i) + ".png";
            auto sprite = Sprite::create(frameName);
            sprite->setScale(0.6f);
            this->BoardFrame.pushBack(sprite->getSpriteFrame());
        }
    }
    else {
        for (int i = 0; i <= 10; ++i) {
            std::string frameName = "Board/li" + std::to_string(i) + ".png";
            auto sprite = Sprite::create(frameName);
            this->BoardFrame.pushBack(sprite->getSpriteFrame());
        }
    }
}

void Board::update(float delta, int HP) {
    // Cập nhật trạng thái board dựa vào HP
    if (isboard) {
        if (HP <= 17 && HP >= 0) {
            this->setSpriteFrame(BoardFrame.at(HP));
        }
        else {
            //CCLOG("Mana kỳ quá òi!");
        }
    } else {
        if (HP <= 10 && HP >= 0) {
            this->setSpriteFrame(BoardFrame.at(HP));
        }
        else {
            //CCLOG("HP kỳ quá òi!");
        }
    }
}
