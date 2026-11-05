
#pragma once

#include <Geode/Geode.hpp>
#include "../presets/AIPresetData.hpp"
#include "../engine/AIDecorationEngine.hpp"

#include <vector>

using namespace geode::prelude;

namespace AutoDecoratorAI {

class AIDecorateLayer : public cocos2d::CCLayer {
public:
    static AIDecorateLayer* create(
        EditorUI* editorUI,
        LevelEditorLayer* editorLayer
    );

    void updatePresetDropdown();

    void updateProgressDisplay(
        const std::string& status,
        float progress
    );

private:
    bool init(
        EditorUI* editorUI,
        LevelEditorLayer* editorLayer
    );

    EditorUI* m_editorUI = nullptr;
    LevelEditorLayer* m_editorLayer = nullptr;

    AIDecorationEngine* m_decorationEngine = nullptr;

    CCMenuItemSpriteExtra* m_toggleButton = nullptr;
    CCMenuItemSpriteExtra* m_closeButton = nullptr;

    CCMenu* m_mainMenu = nullptr;
    CCScale9Sprite* m_overlayPanel = nullptr;

    CCMenuItemToggler* m_edgeEmphasisToggle = nullptr;
    CCMenuItemToggler* m_flowFollowingToggle = nullptr;
    CCMenuItemToggler* m_symmetryMirroringToggle = nullptr;
    CCMenuItemToggler* m_antiClutterToggle = nullptr;

    Slider* m_densitySlider = nullptr;
    Slider* m_chaosSlider = nullptr;
    Slider* m_symmetrySlider = nullptr;
    Slider* m_detailSlider = nullptr;

    CCLabelBMFont* m_presetLabel = nullptr;
    CCMenu* m_presetMenu = nullptr;

    CCMenuItemSpriteExtra* m_analyzeButton = nullptr;
    CCMenuItemSpriteExtra* m_applyButton = nullptr;
    CCMenuItemSpriteExtra* m_savePresetButton = nullptr;
    CCMenuItemSpriteExtra* m_confirmSavePresetButton = nullptr;

    TextInput* m_presetNameInput = nullptr;

    CCLabelBMFont* m_progressLabel = nullptr;
    CCProgressTimer* m_progressBar = nullptr;

    bool m_isVisible = false;
    AIPreset* m_currentPreset = nullptr;

    std::vector<AIPreset> m_availablePresets;

    void onToggle(CCObject* sender);
    void onClose(CCObject* sender);

    void onPresetChanged(CCObject* sender);
    void onAnalyze(CCObject* sender);
    void onApply(CCObject* sender);
    void onSavePreset(CCObject* sender);
    void onConfirmSavePreset(CCObject* sender);

    void onDensityChanged(CCObject* sender);
    void onChaosChanged(CCObject* sender);
    void onSymmetryChanged(CCObject* sender);
    void onDetailChanged(CCObject* sender);

    void onEdgeEmphasisChanged(CCObject* sender);
    void onFlowFollowingChanged(CCObject* sender);
    void onSymmetryMirroringChanged(CCObject* sender);
    void onAntiClutterChanged(CCObject* sender);

    void createOverlayPanel();
    void createToolbarButton();
    void createPresetSelector();
    void createSliders();
    void createToggles();
    void createActionButtons();
    void createProgressDisplay();

    void showOverlay();
    void hideOverlay();

    std::vector<geode::Ref<GameObject>> getSelectedObjects();
};

}
