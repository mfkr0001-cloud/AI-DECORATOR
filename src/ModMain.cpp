#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

#include "ui/AIDecorateLayer.hpp"
#include "presets/AIPresetManager.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    auto presetManager = AutoDecoratorAI::AIPresetManager::get();

    if (presetManager) {
        presetManager->initialize();
    }

    log::info("Auto Decorator AI loaded successfully!");
}

class $modify(EditorUI) {
    struct Fields {
        AutoDecoratorAI::AIDecorateLayer* aiDecorateLayer = nullptr;
    };

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        m_fields->aiDecorateLayer =
            AutoDecoratorAI::AIDecorateLayer::create(this, editorLayer);

        if (m_fields->aiDecorateLayer) {
            this->addChild(m_fields->aiDecorateLayer, 1000);
        }

        return true;
    }
};
