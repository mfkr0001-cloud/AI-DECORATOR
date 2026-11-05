#include "AIDecorateLayer.hpp"
#include "../presets/AIPresetManager.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace AutoDecoratorAI {

AIDecorateLayer* AIDecorateLayer::create(
    EditorUI* editorUI,
    LevelEditorLayer* editorLayer
) {
    auto ret = new AIDecorateLayer();

    if (ret && ret->init(editorUI, editorLayer)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool AIDecorateLayer::init(
    EditorUI* editorUI,
    LevelEditorLayer* editorLayer
) {
    if (!CCLayer::init()) {
        return false;
    }

    m_editorUI = editorUI;
    m_editorLayer = editorLayer;
    m_decorationEngine = AIDecorationEngine::get();
    m_isVisible = false;
    m_currentPreset = nullptr;

    if (!m_editorUI || !m_editorLayer || !m_decorationEngine) {
        log::error("Auto Decorator AI: initialization failed");
        return false;
    }

    m_decorationEngine->initialize();

    m_decorationEngine->setProgressCallback(
        [this](const std::string& status, float progress) {
            this->updateProgressDisplay(status, progress);
        }
    );

    auto presetManager = AIPresetManager::get();

    if (presetManager) {
        m_availablePresets = presetManager->getAllPresets();
    }

    if (!m_availablePresets.empty()) {
        m_currentPreset = &m_availablePresets[0];
    }

    createToolbarButton();
    createOverlayPanel();
    createPresetSelector();
    createSliders();
    createToggles();
    createActionButtons();
    createProgressDisplay();

    hideOverlay();

    return true;
}

// --------------------------------------------------
// TOOLBAR BUTTON
// --------------------------------------------------

void AIDecorateLayer::createToolbarButton() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto buttonSprite = ButtonSprite::create("Auto Decorate AI");

    auto button = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(AIDecorateLayer::onToggle)
    );

    if (!button) {
        return;
    }

    m_toggleButton = button;

    auto menu = CCMenu::create();
    menu->addChild(button);

    menu->setPosition(
        CCPoint(
            winSize.width / 2,
            winSize.height - 30
        )
    );

    this->addChild(menu);
}

// --------------------------------------------------
// MAIN PANEL
// --------------------------------------------------

void AIDecorateLayer::createOverlayPanel() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto panel = CCScale9Sprite::create("square02_001.png");

    if (!panel) {
        log::error("Auto Decorator AI: panel creation failed");
        return;
    }

    panel->setContentSize(CCSize(400, 500));

    panel->setPosition(
        CCPoint(
            winSize.width / 2,
            winSize.height / 2
        )
    );

    panel->setOpacity(200);

    m_overlayPanel = panel;

    this->addChild(panel);

    m_mainMenu = CCMenu::create();
    m_mainMenu->setPosition(CCPoint(0, 0));

    panel->addChild(m_mainMenu);
}

// --------------------------------------------------
// PRESET SELECTOR
// --------------------------------------------------

void AIDecorateLayer::createPresetSelector() {
    auto panelSize = m_overlayPanel->getContentSize();

    auto presetLabel = CCLabelBMFont::create(
        "AI Preset:",
        "bigFont.fnt"
    );

    presetLabel->setPosition(
        CCPoint(
            panelSize.width / 2,
            panelSize.height - 40
        )
    );

    presetLabel->setScale(0.6f);

    m_overlayPanel->addChild(presetLabel);

    m_presetLabel = CCLabelBMFont::create(
        m_currentPreset
            ? m_currentPreset->name.c_str()
            : "None",
        "goldFont.fnt"
    );

    m_presetLabel->setPosition(
        CCPoint(
            panelSize.width / 2,
            panelSize.height - 70
        )
    );

    m_presetLabel->setScale(0.5f);

    m_overlayPanel->addChild(m_presetLabel);

    m_presetMenu = CCMenu::create();

    m_presetMenu->setPosition(
        CCPoint(
            panelSize.width / 2,
            panelSize.height - 100
        )
    );

    m_overlayPanel->addChild(m_presetMenu);

    updatePresetDropdown();
}

void AIDecorateLayer::updatePresetDropdown() {
    if (!m_presetMenu) {
        return;
    }

    m_presetMenu->removeAllChildren();

    float yOffset = 0;

    for (const auto& preset : m_availablePresets) {
        auto label = CCLabelBMFont::create(
            preset.name.c_str(),
            "chatFont.fnt"
        );

        if (!label) {
            continue;
        }

        label->setScale(0.4f);

        auto button = CCMenuItemLabel::create(
            label,
            this,
            menu_selector(AIDecorateLayer::onPresetChanged)
        );

        if (!button) {
            continue;
        }

        button->setUserObject(
            CCString::create(preset.id.c_str())
        );

        button->setPosition(
            CCPoint(0, yOffset)
        );

        m_presetMenu->addChild(button);

        yOffset -= 25;
    }
}

// --------------------------------------------------
// SLIDERS
// --------------------------------------------------

void AIDecorateLayer::createSliders() {
    auto panelSize = m_overlayPanel->getContentSize();

    float startY = panelSize.height - 200;

    auto densityLabel = CCLabelBMFont::create(
        "Density:",
        "bigFont.fnt"
    );

    densityLabel->setPosition(
        CCPoint(80, startY)
    );

    densityLabel->setScale(0.4f);

    m_overlayPanel->addChild(densityLabel);

    m_densitySlider = Slider::create(
        this,
        menu_selector(AIDecorateLayer::onDensityChanged),
        0.5f
    );

    m_densitySlider->setPosition(
        CCPoint(panelSize.width / 2 + 50, startY)
    );

    m_densitySlider->setScale(0.8f);

    m_mainMenu->addChild(m_densitySlider);

    auto chaosLabel = CCLabelBMFont::create(
        "Chaos:",
        "bigFont.fnt"
    );

    chaosLabel->setPosition(
        CCPoint(80, startY - 40)
    );

    chaosLabel->setScale(0.4f);

    m_overlayPanel->addChild(chaosLabel);

    m_chaosSlider = Slider::create(
        this,
        menu_selector(AIDecorateLayer::onChaosChanged),
        0.3f
    );

    m_chaosSlider->setPosition(
        CCPoint(panelSize.width / 2 + 50, startY - 40)
    );

    m_chaosSlider->setScale(0.8f);

    m_mainMenu->addChild(m_chaosSlider);

    auto symmetryLabel = CCLabelBMFont::create(
        "Symmetry:",
        "bigFont.fnt"
    );

    symmetryLabel->setPosition(
        CCPoint(80, startY - 80)
    );

    symmetryLabel->setScale(0.4f);

    m_overlayPanel->addChild(symmetryLabel);

    m_symmetrySlider = Slider::create(
        this,
        menu_selector(AIDecorateLayer::onSymmetryChanged),
        0.7f
    );

    m_symmetrySlider->setPosition(
        CCPoint(panelSize.width / 2 + 50, startY - 80)
    );

    m_symmetrySlider->setScale(0.8f);

    m_mainMenu->addChild(m_symmetrySlider);

    auto detailLabel = CCLabelBMFont::create(
        "Detail:",
        "bigFont.fnt"
    );

    detailLabel->setPosition(
        CCPoint(80, startY - 120)
    );

    detailLabel->setScale(0.4f);

    m_overlayPanel->addChild(detailLabel);

    m_detailSlider = Slider::create(
        this,
        menu_selector(AIDecorateLayer::onDetailChanged),
        0.4f
    );

    m_detailSlider->setPosition(
        CCPoint(panelSize.width / 2 + 50, startY - 120)
    );

    m_detailSlider->setScale(0.8f);

    m_mainMenu->addChild(m_detailSlider);
}

// --------------------------------------------------
// TOGGLES
// --------------------------------------------------

void AIDecorateLayer::createToggles() {
    auto panelSize = m_overlayPanel->getContentSize();

    float startY = panelSize.height - 280;

    auto edgeSprite =
        CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    auto edgeUncheckSprite =
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

    m_edgeEmphasisToggle = CCMenuItemToggler::create(
        edgeSprite,
        edgeUncheckSprite,
        this,
        menu_selector(AIDecorateLayer::onEdgeEmphasisChanged)
    );

    m_edgeEmphasisToggle->setPosition(
        CCPoint(100, startY)
    );

    m_edgeEmphasisToggle->toggle(
        m_currentPreset
            ? m_currentPreset->edgeEmphasis
            : true
    );

    m_mainMenu->addChild(m_edgeEmphasisToggle);

    auto edgeLabel = CCLabelBMFont::create(
        "Edge Emphasis",
        "bigFont.fnt"
    );

    edgeLabel->setPosition(
        CCPoint(180, startY)
    );

    edgeLabel->setScale(0.35f);
    edgeLabel->setAnchorPoint(CCPoint(0, 0.5f));

    m_overlayPanel->addChild(edgeLabel);

    auto flowSprite =
        CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    auto flowUncheckSprite =
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

    m_flowFollowingToggle = CCMenuItemToggler::create(
        flowSprite,
        flowUncheckSprite,
        this,
        menu_selector(AIDecorateLayer::onFlowFollowingChanged)
    );

    m_flowFollowingToggle->setPosition(
        CCPoint(100, startY - 35)
    );

    m_flowFollowingToggle->toggle(
        m_currentPreset
            ? m_currentPreset->flowFollowing
            : true
    );

    m_mainMenu->addChild(m_flowFollowingToggle);

    auto flowLabel = CCLabelBMFont::create(
        "Flow Following",
        "bigFont.fnt"
    );

    flowLabel->setPosition(
        CCPoint(180, startY - 35)
    );

    flowLabel->setScale(0.35f);
    flowLabel->setAnchorPoint(CCPoint(0, 0.5f));

    m_overlayPanel->addChild(flowLabel);

    auto symSprite =
        CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    auto symUncheckSprite =
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

    m_symmetryMirroringToggle = CCMenuItemToggler::create(
        symSprite,
        symUncheckSprite,
        this,
        menu_selector(AIDecorateLayer::onSymmetryMirroringChanged)
    );

    m_symmetryMirroringToggle->setPosition(
        CCPoint(100, startY - 70)
    );

    m_symmetryMirroringToggle->toggle(
        m_currentPreset
            ? m_currentPreset->symmetryMirroring
            : true
    );

    m_mainMenu->addChild(m_symmetryMirroringToggle);

    auto symLabel = CCLabelBMFont::create(
        "Symmetry Mirroring",
        "bigFont.fnt"
    );

    symLabel->setPosition(
        CCPoint(180, startY - 70)
    );

    symLabel->setScale(0.35f);
    symLabel->setAnchorPoint(CCPoint(0, 0.5f));

    m_overlayPanel->addChild(symLabel);

    auto clutterSprite =
        CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    auto clutterUncheckSprite =
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

    m_antiClutterToggle = CCMenuItemToggler::create(
        clutterSprite,
        clutterUncheckSprite,
        this,
        menu_selector(AIDecorateLayer::onAntiClutterChanged)
    );

    m_antiClutterToggle->setPosition(
        CCPoint(100, startY - 105)
    );

    m_antiClutterToggle->toggle(
        m_currentPreset
            ? m_currentPreset->antiClutterMode
            : false
    );

    m_mainMenu->addChild(m_antiClutterToggle);

    auto clutterLabel = CCLabelBMFont::create(
        "Anti-Clutter",
        "bigFont.fnt"
    );

    clutterLabel->setPosition(
        CCPoint(180, startY - 105)
    );

    clutterLabel->setScale(0.35f);
    clutterLabel->setAnchorPoint(CCPoint(0, 0.5f));

    m_overlayPanel->addChild(clutterLabel);
}

// --------------------------------------------------
// ACTION BUTTONS
// --------------------------------------------------

void AIDecorateLayer::createActionButtons() {
    auto panelSize = m_overlayPanel->getContentSize();

    float startY = 80;

    auto analyzeSprite = ButtonSprite::create(
        "Analyze Selection"
    );

    m_analyzeButton = CCMenuItemSpriteExtra::create(
        analyzeSprite,
        this,
        menu_selector(AIDecorateLayer::onAnalyze)
    );

    m_analyzeButton->setPosition(
        CCPoint(panelSize.width / 2, startY + 60)
    );

    m_mainMenu->addChild(m_analyzeButton);

    auto applySprite = ButtonSprite::create(
        "Apply AI Decoration"
    );

    m_applyButton = CCMenuItemSpriteExtra::create(
        applySprite,
        this,
        menu_selector(AIDecorateLayer::onApply)
    );

    m_applyButton->setPosition(
        CCPoint(panelSize.width / 2, startY + 20)
    );

    m_mainMenu->addChild(m_applyButton);

    auto saveSprite = ButtonSprite::create(
        "Save AI Preset"
    );

    m_savePresetButton = CCMenuItemSpriteExtra::create(
        saveSprite,
        this,
        menu_selector(AIDecorateLayer::onSavePreset)
    );

    m_savePresetButton->setPosition(
        CCPoint(panelSize.width / 2, startY - 20)
    );

    m_mainMenu->addChild(m_savePresetButton);
}

// --------------------------------------------------
// PROGRESS DISPLAY
// --------------------------------------------------

void AIDecorateLayer::createProgressDisplay() {
    auto panelSize = m_overlayPanel->getContentSize();

    m_progressLabel = CCLabelBMFont::create(
        "",
        "bigFont.fnt"
    );

    m_progressLabel->setPosition(
        CCPoint(panelSize.width / 2, 30)
    );

    m_progressLabel->setScale(0.4f);
    m_progressLabel->setVisible(false);

    m_overlayPanel->addChild(m_progressLabel);

    auto progressSprite =
        CCSprite::createWithSpriteFrameName("barbg.png");

    if (!progressSprite) {
        return;
    }

    m_progressBar = CCProgressTimer::create(progressSprite);

    m_progressBar->setType(kCCProgressTimerTypeBar);

    m_progressBar->setBarChangeRate(
        CCPoint(1, 0)
    );

    m_progressBar->setMidpoint(
        CCPoint(0, 0)
    );

    m_progressBar->setPosition(
        CCPoint(panelSize.width / 2, 15)
    );

    m_progressBar->setScaleX(0.8f);
    m_progressBar->setScaleY(0.5f);
    m_progressBar->setPercentage(0);
    m_progressBar->setVisible(false);

    m_overlayPanel->addChild(m_progressBar);
}

void AIDecorateLayer::updateProgressDisplay(
    const std::string& status,
    float progress
) {
    if (m_progressLabel) {
        m_progressLabel->setString(status.c_str());
        m_progressLabel->setVisible(true);
    }

    if (m_progressBar) {
        m_progressBar->setPercentage(
            std::max(0.f, std::min(progress * 100.f, 100.f))
        );

        m_progressBar->setVisible(true);
    }
}

// --------------------------------------------------
// SHOW / HIDE PANEL
// --------------------------------------------------

void AIDecorateLayer::onToggle(CCObject* sender) {
    if (m_isVisible) {
        hideOverlay();
    } else {
        showOverlay();
    }

    m_isVisible = !m_isVisible;
}

void AIDecorateLayer::showOverlay() {
    if (!m_overlayPanel) {
        return;
    }

    m_overlayPanel->stopAllActions();
    m_overlayPanel->setVisible(true);
    m_overlayPanel->setOpacity(200);

    m_overlayPanel->runAction(
        CCFadeIn::create(0.2f)
    );
}

void AIDecorateLayer::hideOverlay() {
    if (!m_overlayPanel) {
        return;
    }

    m_overlayPanel->stopAllActions();
    m_overlayPanel->setVisible(false);
    m_overlayPanel->setOpacity(200);
}

// --------------------------------------------------
// PRESET CHANGED
// --------------------------------------------------

void AIDecorateLayer::onPresetChanged(CCObject* sender) {
    auto button = static_cast<CCMenuItemLabel*>(sender);

    auto userObject = button
        ? button->getUserObject()
        : nullptr;

    auto presetString = typeinfo_cast<CCString*>(userObject);

    if (!presetString) {
        return;
    }

    auto presetManager = AIPresetManager::get();

    if (!presetManager) {
        return;
    }

    m_currentPreset = presetManager->getPresetById(
        presetString->getCString()
    );

    if (!m_currentPreset) {
        return;
    }

    m_presetLabel->setString(
        m_currentPreset->name.c_str()
    );

    m_edgeEmphasisToggle->toggle(
        m_currentPreset->edgeEmphasis
    );

    m_flowFollowingToggle->toggle(
        m_currentPreset->flowFollowing
    );

    m_symmetryMirroringToggle->toggle(
        m_currentPreset->symmetryMirroring
    );

    m_antiClutterToggle->toggle(
        m_currentPreset->antiClutterMode
    );

    m_chaosSlider->setValue(
        m_currentPreset->chaosTolerance
    );

    m_symmetrySlider->setValue(
        m_currentPreset->symmetryPreference
    );

    m_detailSlider->setValue(
        m_currentPreset->microDetailLevel
    );
}

// --------------------------------------------------
// ANALYZE SELECTION
// --------------------------------------------------

void AIDecorateLayer::onAnalyze(CCObject* sender) {
    auto selectedObjects = getSelectedObjects();

    if (selectedObjects.empty()) {
        FLAlertLayer::create(
            "No Selection",
            "Please select some objects in the editor first.",
            "OK"
        )->show();

        return;
    }

    auto analyzer = GeometryAnalyzer::get();

    auto clusters = analyzer->analyzeSelection(
        selectedObjects
    );

    auto patternDetector = PatternDetector::get();

    auto patterns = patternDetector->detectPatterns(
        selectedObjects
    );

    std::string result = "Analysis Complete:\n\n";

    result += "Clusters found: ";
    result += std::to_string(clusters.size());
    result += "\n";

    result += "Symmetry: ";

    switch (patterns.symmetryType) {
        case SymmetryType::Horizontal:
            result += "Horizontal";
            break;

        case SymmetryType::Vertical:
            result += "Vertical";
            break;

        case SymmetryType::Radial:
            result += "Radial";
            break;

        case SymmetryType::Bilateral:
            result += "Bilateral";
            break;

        default:
            result += "None";
            break;
    }

    result += " (";
    result += std::to_string(
        static_cast<int>(patterns.symmetryStrength * 100)
    );
    result += "%)\n";

    result += "Repetition: ";
    result += patterns.hasRepetition ? "Yes" : "No";
    result += "\n";

    result += "Density Imbalance: ";
    result += std::to_string(
        static_cast<int>(patterns.densityImbalance * 100)
    );
    result += "%\n";

    FLAlertLayer::create(
        "Geometry Analysis",
        result,
        "OK"
    )->show();
}

// --------------------------------------------------
// APPLY AI DECORATION
// --------------------------------------------------

void AIDecorateLayer::onApply(CCObject* sender) {
    auto selectedObjects = getSelectedObjects();

    if (selectedObjects.empty()) {
        FLAlertLayer::create(
            "No Selection",
            "Please select some objects in the editor first.",
            "OK"
        )->show();

        return;
    }

    if (!m_currentPreset) {
        FLAlertLayer::create(
            "No Preset",
            "Please select an AI preset first.",
            "OK"
        )->show();

        return;
    }

    AIPreset modifiedPreset = *m_currentPreset;

    modifiedPreset.edgeEmphasis =
        m_edgeEmphasisToggle->isToggled();

    modifiedPreset.flowFollowing =
        m_flowFollowingToggle->isToggled();

    modifiedPreset.symmetryMirroring =
        m_symmetryMirroringToggle->isToggled();

    modifiedPreset.antiClutterMode =
        m_antiClutterToggle->isToggled();

    auto placements = m_decorationEngine->generateDecorations(
        selectedObjects,
        modifiedPreset,
        m_densitySlider->getValue(),
        m_chaosSlider->getValue(),
        m_symmetrySlider->getValue(),
        m_detailSlider->getValue()
    );

    if (placements.empty()) {
        FLAlertLayer::create(
            "No Decorations",
            "No decorations were generated. Try adjusting the parameters.",
            "OK"
        )->show();

        return;
    }

    m_decorationEngine->applyDecorationsToLevel(
        placements,
        m_editorUI
    );

    if (m_progressLabel) {
        m_progressLabel->setVisible(false);
    }

    if (m_progressBar) {
        m_progressBar->setVisible(false);
    }

    FLAlertLayer::create(
        "Success",
        "Applied " + std::to_string(placements.size()) +
        " decorations!",
        "OK"
    )->show();
}

// --------------------------------------------------
// SAVE PRESET
// --------------------------------------------------

void AIDecorateLayer::onSavePreset(CCObject* sender) {
    if (!m_overlayPanel || !m_mainMenu) {
        return;
    }

    if (m_presetNameInput) {
        m_presetNameInput->removeFromParent();
        m_presetNameInput = nullptr;
    }

    if (m_confirmSavePresetButton) {
        m_confirmSavePresetButton->removeFromParent();
        m_confirmSavePresetButton = nullptr;
    }

    m_presetNameInput = TextInput::create(
        220.f,
        "Enter preset name"
    );

    if (!m_presetNameInput) {
        return;
    }

    auto panelSize = m_overlayPanel->getContentSize();

    m_presetNameInput->setPosition(
        CCPoint(
            panelSize.width / 2,
            panelSize.height / 2 + 15.f
        )
    );

    m_overlayPanel->addChild(
        m_presetNameInput,
        1000
    );

    auto saveSprite = ButtonSprite::create(
        "Save preset"
    );

    m_confirmSavePresetButton = CCMenuItemSpriteExtra::create(
        saveSprite,
        this,
        menu_selector(AIDecorateLayer::onConfirmSavePreset)
    );

    m_confirmSavePresetButton->setPosition(
        CCPoint(
            panelSize.width / 2,
            panelSize.height / 2 - 35.f
        )
    );

    m_mainMenu->addChild(
        m_confirmSavePresetButton,
        1001
    );

    m_presetNameInput->focus();
}

void AIDecorateLayer::onConfirmSavePreset(CCObject* sender) {
    if (!m_presetNameInput) {
        return;
    }

    auto name = m_presetNameInput->getString();

    if (name.empty()) {
        FLAlertLayer::create(
            "Preset name",
            "Enter a name before saving.",
            "OK"
        )->show();

        return;
    }

    auto presetManager = AIPresetManager::get();

    if (!presetManager) {
        return;
    }

    auto newPreset = presetManager->createCustomPreset(
        name,
        "Custom user preset"
    );

    if (m_currentPreset) {
        newPreset.styleBias =
            m_currentPreset->styleBias;

        newPreset.decorationPool =
            m_currentPreset->decorationPool;

        newPreset.styleWeights =
            m_currentPreset->styleWeights;
    }

    newPreset.edgeEmphasis =
        m_edgeEmphasisToggle->isToggled();

    newPreset.flowFollowing =
        m_flowFollowingToggle->isToggled();

    newPreset.symmetryMirroring =
        m_symmetryMirroringToggle->isToggled();

    newPreset.antiClutterMode =
        m_antiClutterToggle->isToggled();

    newPreset.chaosTolerance =
        m_chaosSlider->getValue();

    newPreset.symmetryPreference =
        m_symmetrySlider->getValue();

    newPreset.microDetailLevel =
        m_detailSlider->getValue();

    presetManager->addPreset(newPreset);
    presetManager->saveCustomPresets();

    m_presetNameInput->defocus();
    m_presetNameInput->removeFromParent();

    m_presetNameInput = nullptr;

    if (m_confirmSavePresetButton) {
        m_confirmSavePresetButton->removeFromParent();
        m_confirmSavePresetButton = nullptr;
    }

    m_availablePresets = presetManager->getAllPresets();

    updatePresetDropdown();

    FLAlertLayer::create(
        "Success",
        "Preset saved successfully!",
        "OK"
    )->show();
}

// --------------------------------------------------
// SLIDER CALLBACKS
// --------------------------------------------------

void AIDecorateLayer::onDensityChanged(CCObject* sender) {
}

void AIDecorateLayer::onChaosChanged(CCObject* sender) {
}

void AIDecorateLayer::onSymmetryChanged(CCObject* sender) {
}

void AIDecorateLayer::onDetailChanged(CCObject* sender) {
}

// --------------------------------------------------
// TOGGLE CALLBACKS
// --------------------------------------------------

void AIDecorateLayer::onEdgeEmphasisChanged(CCObject* sender) {
}

void AIDecorateLayer::onFlowFollowingChanged(CCObject* sender) {
}

void AIDecorateLayer::onSymmetryMirroringChanged(CCObject* sender) {
}

void AIDecorateLayer::onAntiClutterChanged(CCObject* sender) {
}

// --------------------------------------------------
// SELECTED OBJECTS
// --------------------------------------------------

std::vector<geode::Ref<GameObject>>
AIDecorateLayer::getSelectedObjects() {
    std::vector<geode::Ref<GameObject>> selectedObjects;

    if (!m_editorUI) {
        return selectedObjects;
    }

    auto selected = m_editorUI->getSelectedObjects();

    if (!selected) {
        return selectedObjects;
    }

    for (auto* object : CCArrayExt<GameObject*>(selected)) {
        if (object) {
            selectedObjects.emplace_back(object);
        }
    }

    return selectedObjects;
}

}
