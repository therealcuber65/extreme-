#include <Geode/Geode.hpp>
#include <Geode/modify/GJDifficultySprite.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

bool isCustom15StarLevel(GJGameLevel* level) {
    return level && level->m_stars == 15 && level->m_levelID != 139;
}

void alignFeatureGlowToFace(CCNode* parent, GJDifficultySprite* diffSprite) {
    if (!parent || !diffSprite) return;

    // On LevelInfoLayer / LevelCell, the Feature/Epic/Legendary/Mythic glow is a sibling of difficulty-sprite
    // (or stored on diffSprite->m_featureCoin). Align any glow node to the face's exact center.
    CCPoint faceCenter = diffSprite->getPosition();

    if (auto coin = diffSprite->m_featureCoin) {
        coin->setPosition({ diffSprite->getContentSize().width / 2.f, diffSprite->getContentSize().height / 2.f - 4.5f });
        coin->setZOrder(-1);
    }

    for (auto* child : parent->getChildrenExt()) {
        if (!child || child == diffSprite) continue;
        auto id = child->getID();
        // Geometry Dash's feature glow sprite sits right around difficulty-sprite's X coordinate
        if (id == "feature-coin" || id == "epic-sprite" ||
            (std::abs(child->getPositionX() - faceCenter.x) < 1.f &&
             std::abs(child->getPositionY() - faceCenter.y) < 15.f &&
             child->getZOrder() < diffSprite->getZOrder())) {
            child->setPosition({ faceCenter.x, faceCenter.y - 4.5f });
        }
    }
}

void applyCustom15StarFace(GJDifficultySprite* diffSprite) {
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
            customSprite->setScale(std::min(scaleX, scaleY) * 0.96f);
        }

        customSprite->setPosition({ targetSize.width / 2.f, targetSize.height / 2.f - 1.5f });
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

void applyCustom15StarLabel(LevelInfoLayer* layer) {
    if (!layer) return;

    if (auto demonLabel = layer->getChildByID("demon-label")) {
        demonLabel->setVisible(false);
    }

    if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(layer->getChildByID("difficulty-sprite"))) {
        if (!diffSprite->getChildByID("custom-15-label-top"_spr)) {
            float centerX = diffSprite->getContentSize().width / 2.f;
            addShadowedLabel(diffSprite, "LIST", "custom-15-label-top"_spr, { centerX, 7.0f }, 0.36f);
            addShadowedLabel(diffSprite, "DEMON", "custom-15-label-bottom"_spr, { centerX, -2.5f }, 0.36f);
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
            if (auto parent = this->getParent()) {
                alignFeatureGlowToFace(parent, this);
            }
        }
    }
};

class $modify(MyLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
        if (isCustom15StarLevel(level)) {
            if (auto diffContainer = this->m_mainLayer->getChildByID("difficulty-container")) {
                if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(diffContainer->getChildByID("difficulty-sprite"))) {
                    applyCustom15StarFace(diffSprite);
                    alignFeatureGlowToFace(diffContainer, diffSprite);
                }
            }
        }
    }
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    void applyAll15StarVisuals() {
        if (!isCustom15StarLevel(this->m_level)) return;

        if (auto diffSprite = typeinfo_cast<GJDifficultySprite*>(this->getChildByID("difficulty-sprite"))) {
            applyCustom15StarFace(diffSprite);
            applyCustom15StarLabel(this);
            diffSprite->setPositionY(diffSprite->getPositionY() + 6.f);
            alignFeatureGlowToFace(this, diffSprite);
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
