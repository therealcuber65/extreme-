#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

bool isCustom15StarLevel(GJGameLevel* level) {
    return level && level->m_stars == 15 && level->m_levelID != 139;
}

void applyCustom15StarFace(GJDifficultySprite* diffSprite) {
    if (!diffSprite) return;

    // Always keep the vanilla face hidden even after feature/epic updates
    diffSprite->setOpacity(0);

    // Nudge the Feature/Epic/Legendary/Mythic ring/fire down slightly so it centers behind the demon head
    if (auto featureSprite = diffSprite->getChildByID("feature-coin")) {
        featureSprite->setPositionY(diffSprite->getContentSize().height / 2.f - 3.5f);
    }

    if (diffSprite->getChildByID("custom-15-face"_spr)) return;

    auto imgPath = Mod::get()->getResourcesDir() / "resources" / "list_demon.png";
    if (!std::filesystem::exists(imgPath)) {
        imgPath = Mod::get()->getResourcesDir() / "list_demon.png";
    }

    if (auto customSprite = CCSprite::create(imgPath.string().c_str())) {
        customSprite->setID("custom-15-face"_spr);
        customSprite->setZOrder(1); // Keep Feature/Epic/Legendary/Mythic fire behind the face

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

void addShadowedLabel(CCNode* parent, const char* text, const char* id, CCPoint pos, float scale) {
    auto shadow = CCLabelBMFont::create(text, "bigFont.fnt");
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

void applyCustom15StarLabel(LevelInfoLayer* layer) {
    if (!layer) return;

    // Always keep the vanilla demon label hidden if the level is marked as a demon
    if (auto demonLabel = layer->getChildByID("demon-label")) {
        demonLabel->setVisible(false);
    }

    if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(layer->getChildByID("difficulty-sprite"))) {
        if (diffSprite->getChildByID("custom-15-label-top"_spr)) return;

        float centerX = diffSprite->getContentSize().width / 2.f;

        addShadowedLabel(diffSprite, "LIST", "custom-15-label-top"_spr, { centerX, 7.0f }, 0.36f);
        addShadowedLabel(diffSprite, "DEMON", "custom-15-label-bottom"_spr, { centerX, -2.5f }, 0.36f);
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
    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();

        if (isCustom15StarLevel(this->m_level)) {
            if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(this->getChildByID("difficulty-sprite"))) {
                applyCustom15StarFace(diffSprite);
                applyCustom15StarLabel(this);

                diffSprite->setPositionY(diffSprite->getPositionY() + 6.f);
            }
        }
    }
};
