// MenuScene.cpp
#include "MenuScene.h"
#include "HelloWorldScene.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool MenuScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("haha/menu_background.png");
    if (background == nullptr)
    {
        problemLoading("'haha/HelloWorld.png'haha/");
    }
    else
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(background, 0);
    }

    //AudioEngine::play2d("haha/Greenpath.mp3", true, 0.5f);

    auto titleImage = Sprite::create("haha/menu_name.png");
    if (titleImage == nullptr)
    {
        problemLoading("'haha/menu_name.png'");
    }
    else
    {
        titleImage->setScale(0.5f);
        titleImage->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + titleImage->getContentSize().height - 100));
        this->addChild(titleImage, 1);
    }

    auto iconImage = Sprite::create("haha/menu_icon.png");
    if (iconImage == nullptr)
    {
        problemLoading("'haha/menu_icon.png'");
    }
    else
    {
        iconImage->setScale(0.5f);
        iconImage->setPosition(Vec2(origin.x + visibleSize.width / 4, origin.y + visibleSize.height / 4));
        this->addChild(iconImage, 1);
    }

    auto iconLeft = Sprite::create("haha/main_menu_pointer_left.png");
    auto iconRight = Sprite::create("haha/main_menu_pointer_right.png");
    if (iconLeft && iconRight)
    {
        iconLeft->setScale(0.3f);
        iconRight->setScale(0.3f);
        iconLeft->setVisible(false);
        iconRight->setVisible(false);
        this->addChild(iconLeft, 2);
        this->addChild(iconRight, 2);
    }

    Vector<MenuItem*> menuItems;
    auto createMenuItem = [&](const std::string& text, const Vec2& position, const ccMenuCallback& callback)
    {
        auto item = MenuItemLabel::create(Label::createWithTTF(text, "fonts/Marker Felt.ttf", 24), callback);
        item->setPosition(position);
        menuItems.pushBack(item);
        return item;
    };

    auto startItem = createMenuItem("Start Game", Vec2(visibleSize.width / 2, visibleSize.height / 2 - 50), CC_CALLBACK_1(MenuScene::menuStartCallback, this));
    auto optionItem = createMenuItem("Options", Vec2(visibleSize.width / 2, visibleSize.height / 2 - 100), CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    auto aboutItem = createMenuItem("About", Vec2(visibleSize.width / 2, visibleSize.height / 2 - 150), CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    auto closeItem = createMenuItem("Quit Game", Vec2(visibleSize.width / 2, visibleSize.height / 2 - 200), CC_CALLBACK_1(MenuScene::menuCloseCallback, this));

    auto menu = Menu::create(startItem, optionItem, aboutItem, closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = [menuItems, iconLeft, iconRight](Event* event)
    {
        auto mouseEvent = dynamic_cast<EventMouse*>(event);
        Vec2 mousePos = mouseEvent->getLocationInView();

        bool hovered = false;
        for (auto item : menuItems)
        {
            Vec2 itemPos = item->getParent()->convertToWorldSpace(item->getPosition());
            Rect itemRect = item->getBoundingBox();
            itemRect.origin = itemPos - itemRect.size / 2;

            if (itemRect.containsPoint(mousePos))
            {
                iconLeft->setPosition(item->getPosition() - Vec2(itemRect.size.width / 2 + 25, 0));
                iconRight->setPosition(item->getPosition() + Vec2(itemRect.size.width / 2 + 25, 0));
                iconLeft->setVisible(true);
                iconRight->setVisible(true);
                hovered = true;
                break;
            }
        }

        if (!hovered)
        {
            iconLeft->setVisible(false);
            iconRight->setVisible(false);
        }
    };

    // Đăng ký listener chung cho cả Scene
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    return true;
}

void MenuScene::menuStartCallback(Ref* pSender)
{
    auto scene = HelloWorld::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene));
}

void MenuScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}
