#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "ui/AIDecorateLayer.hpp"
#include "presets/AIPresetManager.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    // Initialize preset manager
    auto presetManager = AutoDecoratorAI::AIPresetManager::get();
    presetManager->initialize();
    
    log::info("Auto Decorator AI mod loaded successfully!");
}

class $modify(AIEditorUI, EditorUI) {
    struct Fields {
        AutoDecoratorAI::AIDecorateLayer* aiDecorateLayer = nullptr;
    };

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        m_fields->aiDecorateLayer = AutoDecoratorAI::AIDecorateLayer::create(this, editorLayer);
        if (m_fields->aiDecorateLayer) {
            this->addChild(m_fields->aiDecorateLayer, 1000);
        }

        return true;
    }
};
