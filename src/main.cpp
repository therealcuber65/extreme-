#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

void applyCustom15StarFace(GJDifficultySprite* diffSprite) {
    if (!diffSprite) return;
    if (auto customFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("6z11wx8mjdrmr0d1zabs637akc_result_0.png"_spr)) {
        diffSprite->setDisplayFrame(customFrame);
    }
}

class $modify(MyLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
        if (level && level->m_stars == 15) {
            if (auto diffContainer = this->m_mainLayer->getChildByID("difficulty-container")) {
                if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(diffContainer->getChildByID("difficulty-sprite"))) {
                    applyCustom15StarFace(diffSprite);
                }
            }
        }
    }
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        if (level && level->m_stars == 15) {
            if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(this->getChildByID("difficulty-sprite"))) {
                applyCustom15StarFace(diffSprite);
            }
        }
        return true;
    }

    void levelDownloadFinished(GJGameLevel* level) {
        LevelInfoLayer::levelDownloadFinished(level);
        if (level && level->m_stars == 15) {
            if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(this->getChildByID("difficulty-sprite"))) {
                applyCustom15StarFace(diffSprite);
            }
        }
    }
};
