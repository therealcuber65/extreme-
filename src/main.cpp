#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

// Change the difficulty name here if you want a different title
constexpr const char* CUSTOM_DIFFICULTY_NAME = "List Demon";

bool isCustom15StarLevel(GJGameLevel* level) {
    return level && level->m_stars == 15 && level->m_levelID != 139;
}

void applyCustom15StarFace(GJDifficultySprite* diffSprite) {
    if (!diffSprite) return;

    if (diffSprite->getChildByID("custom-15-face"_spr)) return;

    auto imgPath = Mod::get()->getResourcesDir() / "resources" / "list_demon.png";
    if (!std::filesystem::exists(imgPath)) {
        imgPath = Mod::get()->getResourcesDir() / "list_demon.png";
    }

    if (auto customSprite = CCSprite::create(imgPath.string().c_str())) {
        customSprite->setID("custom-15-face"_spr);

        diffSprite->setOpacity(0);

        auto targetSize = diffSprite->getContentSize();
        auto imgSize = customSprite->getContentSize();
        if (imgSize.width > 0 && imgSize.height > 0) {
            float scaleX = targetSize.width / imgSize.width;
            float scaleY = targetSize.height / imgSize.height;
            customSprite->setScale(std::min(scaleX, scaleY));
        }

        customSprite->setPosition(targetSize / 2.f);
        diffSprite->addChild(customSprite);
    }
}

void applyCustom15StarLabel(LevelInfoLayer* layer) {
    if (!layer) return;

    if (auto diffLabel = typeinfo_cast<CCLabelBMFont*>(layer->getChildByID("difficulty-label"))) {
        diffLabel->setString(CUSTOM_DIFFICULTY_NAME);
        diffLabel->setVisible(true);
        diffLabel->limitLabelWidth(55.f, 0.4f, 0.1f);
    }
}

class $modify(MyLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
        if (isCustom15StarLevel(level)) {
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
        if (isCustom15StarLevel(level)) {
            if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(this->getChildByID("difficulty-sprite"))) {
                applyCustom15StarFace(diffSprite);
            }
            applyCustom15StarLabel(this);
        }
        return true;
    }

    void levelDownloadFinished(GJGameLevel* level) {
        LevelInfoLayer::levelDownloadFinished(level);
        if (isCustom15StarLevel(level)) {
            if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(this->getChildByID("difficulty-sprite"))) {
                applyCustom15StarFace(diffSprite);
            }
            applyCustom15StarLabel(this);
        }
    }
};
