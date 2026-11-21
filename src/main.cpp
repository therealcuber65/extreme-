#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

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

    // Hide vanilla demon label if the level also has a demon flag
    if (auto demonLabel = layer->getChildByID("demon-label")) {
        demonLabel->setVisible(false);
    }

    // Prevent adding duplicate labels on refresh
    if (layer->getChildByID("custom-15-label-top"_spr)) return;

    if (auto diffSprite = layer->getChildByID("difficulty-sprite")) {
        float centerX = diffSprite->getPositionX();
        float baseY = diffSprite->getPositionY();

        auto topLabel = CCLabelBMFont::create("LIST", "bigFont.fnt");
        topLabel->setID("custom-15-label-top"_spr);
        topLabel->setScale(0.32f);
        topLabel->setPosition({ centerX, baseY - 21.5f });
        layer->addChild(topLabel);

        auto bottomLabel = CCLabelBMFont::create("DEMON", "bigFont.fnt");
        bottomLabel->setID("custom-15-label-bottom"_spr);
        bottomLabel->setScale(0.32f);
        bottomLabel->setPosition({ centerX, baseY - 31.0f });
        layer->addChild(bottomLabel);
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
