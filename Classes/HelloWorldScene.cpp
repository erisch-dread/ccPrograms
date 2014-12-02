#include "HelloWorldScene.h"
//#include <CCTMXTiledMap.h>
 
//using namespace cocos2d;
USING_NS_CC; 

CCScene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
 
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();
 
    // add layer as a child to scene
    scene->addChild(layer);
 
    // return the scene
    return scene;
}
 
// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
 
    _tileMap = TMXTiledMap::create("palletesque.tmx");
    //_tileMap->initWithTMXFile("../Resources/TileMap.tmx");
    //_tileMap->setPosition(100,100);
    //_background = _tileMap->layerNamed("Background");
    _meta = _tileMap->layerNamed("meta");
    _meta->setVisible(false);

    _misc = _tileMap->getLayer("Misc");

    this->addChild(_tileMap);
 
    TMXObjectGroup *objectGroup = _tileMap->objectGroupNamed("Objects");

    if(objectGroup == NULL) {
        CCLog("tile map has no Objects layer");
        return false;
    }

    ValueMap spawnPoint = objectGroup->objectNamed("SpawnPoint");
    //ValueMap spawnProps = spawnPoint->asValueMap();

    //int x = ((CCString)*spawnPoint->valueForKey("x")).intValue();
    //int y = ((CCString)*spawnPoint->valueForKey("y")).intValue();
    int x = spawnPoint.at("x").asInt();
    int y = spawnPoint.at("y").asInt();

    _player = CCSprite::create("Player.png");
    _player->setPosition( ccp(x, y) );

    this->addChild(_player);
    this->setViewPointCenter(_player->getPosition());

    this->setTouchEnabled(true);

    return true;
}

Point HelloWorld::tileCoordForPosition(CCPoint pos) {
            int x = pos.x / _tileMap->getTileSize().width;
            int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - pos.y) / _tileMap->getTileSize().height;

            Point newPoint = Point((float)x, (float)y);
            return newPoint;
}

void HelloWorld::setViewPointCenter(CCPoint position) {
 
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
 
    int x = MAX(position.x, winSize.width/2);
    int y = MAX(position.y, winSize.height/2);
    x = MIN(x, (_tileMap->getMapSize().width * this->_tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height/2);
    CCPoint actualPosition = ccp(x, y);
 
    CCPoint centerOfView = ccp(winSize.width/2, winSize.height/2);
    CCPoint viewPoint = ccpSub(centerOfView, actualPosition);
    this->setPosition(viewPoint);
}

void HelloWorld::setPlayerPosition(CCPoint position) {
    CCPoint tileCoord = this->tileCoordForPosition(position);
    unsigned int gid = _meta->getTileGIDAt(tileCoord);
    if(gid) {
        auto properties = _tileMap->getPropertiesForGID(gid).asValueMap();
        if(!properties.empty()) {
            auto collision = properties["Collidable"].asString();
            auto collectable = properties["Collectable"].asString();
            if("True" == collision) {
                return;
            }
            if("True" == collectable) {
                _meta->removeTileAt(tileCoord);
                _misc->removeTileAt(tileCoord);
            }
        }
    }
    _player->setPosition(position);
    this->setViewPointCenter(position);
}

void HelloWorld::onTouchesBegan(const std::vector<Touch*>& touches, cocos2d::Event *unused_event) {
for(Touch *touch : touches) {
    CCPoint touchLocation = touch->getLocationInView();
    touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
    touchLocation = this->convertToNodeSpace(touchLocation);
 
    CCPoint playerPos = _player->getPosition();
    CCPoint diff = ccpSub(touchLocation, playerPos);
 
    if ( abs(diff.x) > abs(diff.y) ) {
        if (diff.x > 0) {
            playerPos.x += _tileMap->getTileSize().width;
        } else {
            playerPos.x -= _tileMap->getTileSize().width;
        }
    } else {
        if (diff.y > 0) {
            playerPos.y += _tileMap->getTileSize().height;
        } else {
            playerPos.y -= _tileMap->getTileSize().height;
        }
    }
 
    // safety check on the bounds of the map
    if (playerPos.x <= (_tileMap->getMapSize().width * _tileMap->getTileSize().width) &&
        playerPos.y <= (_tileMap->getMapSize().height * _tileMap->getTileSize().height) &&
        playerPos.y >= 0 &&
        playerPos.x >= 0 )
    { 
        this->setPlayerPosition(playerPos);
    }
}
}
