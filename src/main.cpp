#include <Geode/Geode.hpp>
#include <Geode/modify/GJDifficultySprite.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

bool isCustom15StarLevel(GJGameLevel* level) {
    return level && level->m_stars == 15 && level->m_levelID != 139;
}

void applyCustom15StarFace(GJDifficultySprite* diffSprite, float scaleMult, float yOffset) {
    if (!diffSprite) return;

    diffSprite->setOpacity(0);

    if (diffSprite->getChildByID("custom-15-face"_spr)) return;

    auto imgPath = Mod::get()->getResourcesDir() / "resources" / "list_demon.png";
    if (!std::filesystem::exists(imgPath)) {
        imgPath = Mod::get()->getResourcesDir() / "list_demon.png";
    }

    if (auto customSprite = CCSprite::create(imgPath.string().c_str())) {
        customSprite->setID("custom-15-face"_spr);
        customSprite->setZOrder(1);

        auto targetSize = diffSprite->getContentSize();
        auto imgSize = customSprite->getContentSize();
        if (imgSize.width > 0 && imgSize.height > 0) {
            float scaleX = targetSize.width / imgSize.width;
            float scaleY = targetSize.height / imgSize.height;
            customSprite->setScale(std::min(scaleX, scaleY) * scaleMult);
        }

        customSprite->setPosition({ targetSize.width / 2.f, targetSize.height / 2.f + yOffset });
        diffSprite->addChild(customSprite);
    }
}

void addShadowedLabel(CCNode* parent, const char* text, const char* id, CCPoint pos, float scale) {
    auto shadow = CCLabelBMFont::create(text, "bigFont.fnt");
    shadow->setID("custom-15-shadow"_spr);
    shadow->setScale(scale);
    shadow->setColor({ 0, 0, 0 });
    shadow->setOpacity(75);
    shadow->setZOrder(9);
    shadow->setPosition({ pos.x + 1.0f, pos.y - 1.0f });
    parent->addChild(shadow);

    auto label = CCLabelBMFont::create(text, "bigFont.fnt");
    label->setID(id);
    label->setScale(scale);
    label->setZOrder(10);
    label->setPosition(pos);
    parent->addChild(label);
}

void applyCustom15StarCellLabel(CCNode* diffContainer, GJDifficultySprite* diffSprite) {
    if (!diffContainer || !diffSprite) return;

    if (auto demonLabel = diffContainer->getChildByID("demon-label")) {
        demonLabel->setVisible(false);
    }

    if (!diffSprite->getChildByID("custom-15-cell-label"_spr)) {
        float centerX = diffSprite->getContentSize().width / 2.f;
        addShadowedLabel(diffSprite, "DEMON", "custom-15-cell-label"_spr, { centerX, 3.0f }, 0.34f);
    }
}

void applyCustom15StarLabel(LevelInfoLayer* layer) {
    if (!layer) return;

    if (auto demonLabel = layer->getChildByID("demon-label")) {
        demonLabel->setVisible(false);
    }

    if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(layer->getChildByID("difficulty-sprite"))) {
        if (!diffSprite->getChildByID("custom-15-label-top"_spr)) {
            float centerX = diffSprite->getContentSize().width / 2.f;
            addShadowedLabel(diffSprite, "LIST", "custom-15-label-top"_spr, { centerX, 9.5f }, 0.36f);
            addShadowedLabel(diffSprite, "DEMON", "custom-15-label-bottom"_spr, { centerX, 0.0f }, 0.36f);
        }
    }
}

class $modify(MyGJDifficultySprite, GJDifficultySprite) {
    void updateDifficultyFrame(int difficulty, GJDifficultyName name) {
        GJDifficultySprite::updateDifficultyFrame(difficulty, name);
        if (this->getChildByID("custom-15-face"_spr)) {
            this->setOpacity(0);
        }
    }

    void updateFeatureState(GJFeatureState state) {
        GJDifficultySprite::updateFeatureState(state);
        if (this->getChildByID("custom-15-face"_spr)) {
            this->setOpacity(0);
        }
    }
};

class $modify(MyLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
        if (isCustom15StarLevel(level)) {
            if (auto diffContainer = this->m_mainLayer->getChildByID("difficulty-container")) {
                if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(diffContainer->getChildByID("difficulty-sprite"))) {
                    // Use 1.0f scale in LevelCell so it fills the Featured ring just like Incandescence
                    applyCustom15StarFace(diffSprite, 1.0f, 5.5f);
                    applyCustom15StarCellLabel(diffContainer, diffSprite);
                }
            }
        }
    }
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    void applyAll15StarVisuals() {
        if (!isCustom15StarLevel(this->m_level)) return;

        if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(this->getChildByID("difficulty-sprite"))) {
            // Keep 0.85f scale and +7.5f offset on LevelInfoLayer
            applyCustom15StarFace(diffSprite, 0.85f, 7.5f);
            applyCustom15StarLabel(this);
            diffSprite->setPositionY(diffSprite->getPositionY() + 3.f);
        }
    }

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        this->applyAll15StarVisuals();
        return true;
    }

    void levelDownloadFinished(GJGameLevel* level) {
        LevelInfoLayer::levelDownloadFinished(level);
        this->applyAll15StarVisuals();
    }

    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();
        this->applyAll15StarVisuals();
    }
};
