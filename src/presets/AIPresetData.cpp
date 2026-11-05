#include "AIPresetData.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace AutoDecoratorAI {

matjson::Value AIPreset::toJson() const {
    matjson::Value json;
    
    json["id"] = id;
    json["name"] = name;
    json["description"] = description;
    json["styleBias"] = styleBiasToString(styleBias);
    
    json["densityCurve"]["baseDensity"] = densityCurve.baseDensity;
    json["densityCurve"]["edgeMultiplier"] = densityCurve.edgeMultiplier;
    json["densityCurve"]["cornerMultiplier"] = densityCurve.cornerMultiplier;
    json["densityCurve"]["centerMultiplier"] = densityCurve.centerMultiplier;
    json["densityCurve"]["flowMultiplier"] = densityCurve.flowMultiplier;
    
    json["symmetryPreference"] = symmetryPreference;
    json["chaosTolerance"] = chaosTolerance;
    json["clusteringThreshold"] = clusteringThreshold;
    json["microDetailLevel"] = microDetailLevel;
    
    json["edgeEmphasis"] = edgeEmphasis;
    json["flowFollowing"] = flowFollowing;
    json["symmetryMirroring"] = symmetryMirroring;
    json["antiClutterMode"] = antiClutterMode;
    
    std::vector<matjson::Value> decorationPoolJson;
    for (const auto& obj : decorationPool) {
        matjson::Value objJson;
        objJson["objectID"] = obj.objectID;
        objJson["weight"] = obj.weight;
        objJson["category"] = obj.category;
        objJson["requiresGlow"] = obj.requiresGlow;
        objJson["isDetail"] = obj.isDetail;
        decorationPoolJson.push_back(objJson);
    }
    json["decorationPool"] = matjson::Value(decorationPoolJson);
    
    matjson::Value styleWeightsJson;
    for (const auto& pair : styleWeights) {
        styleWeightsJson[pair.first] = pair.second;
    }
    json["styleWeights"] = styleWeightsJson;
    
    return json;
}

AIPreset AIPreset::fromJson(const matjson::Value& json) {
    AIPreset preset;

    preset.id = json["id"].asString().unwrapOr("");
    preset.name = json["name"].asString().unwrapOr("Imported Preset");
    preset.description = json["description"].asString().unwrapOr("");
    preset.styleBias = styleBiasFromString(json["styleBias"].asString().unwrapOr("CleanModern"));

    if (json.contains("densityCurve")) {
        const auto dc = json["densityCurve"];
        preset.densityCurve.baseDensity = static_cast<float>(dc["baseDensity"].asDouble().unwrapOr(0.5));
        preset.densityCurve.edgeMultiplier = static_cast<float>(dc["edgeMultiplier"].asDouble().unwrapOr(1.5));
        preset.densityCurve.cornerMultiplier = static_cast<float>(dc["cornerMultiplier"].asDouble().unwrapOr(2.0));
        preset.densityCurve.centerMultiplier = static_cast<float>(dc["centerMultiplier"].asDouble().unwrapOr(0.8));
        preset.densityCurve.flowMultiplier = static_cast<float>(dc["flowMultiplier"].asDouble().unwrapOr(1.2));
    }

    preset.symmetryPreference = static_cast<float>(json["symmetryPreference"].asDouble().unwrapOr(0.7));
    preset.chaosTolerance = static_cast<float>(json["chaosTolerance"].asDouble().unwrapOr(0.3));
    preset.clusteringThreshold = static_cast<float>(json["clusteringThreshold"].asDouble().unwrapOr(0.5));
    preset.microDetailLevel = static_cast<float>(json["microDetailLevel"].asDouble().unwrapOr(0.4));

    preset.edgeEmphasis = json["edgeEmphasis"].asBool().unwrapOr(true);
    preset.flowFollowing = json["flowFollowing"].asBool().unwrapOr(true);
    preset.symmetryMirroring = json["symmetryMirroring"].asBool().unwrapOr(true);
    preset.antiClutterMode = json["antiClutterMode"].asBool().unwrapOr(false);

    if (auto array = json["decorationPool"].asArray(); array) {
        for (const auto& objJson : *array) {
            DecorationObject obj{};
            obj.objectID = objJson["objectID"].asInt().unwrapOr(0);
            obj.weight = static_cast<float>(objJson["weight"].asDouble().unwrapOr(1.0));
            obj.category = objJson["category"].asString().unwrapOr("detail");
            obj.requiresGlow = objJson["requiresGlow"].asBool().unwrapOr(false);
            obj.isDetail = objJson["isDetail"].asBool().unwrapOr(false);
            preset.decorationPool.push_back(std::move(obj));
        }
    }

    if (auto object = json["styleWeights"].asObject(); object) {
        for (const auto& pair : *object) {
            preset.styleWeights[pair.first] = static_cast<float>(pair.second.asDouble().unwrapOr(0.0));
        }
    }

    return preset;
}

std::string AIPreset::styleBiasToString(StyleBias bias) {
    switch (bias) {
        case StyleBias::CleanModern: return "CleanModern";
        case StyleBias::NeonGlow: return "NeonGlow";
        case StyleBias::DemonExtreme: return "DemonExtreme";
        case StyleBias::RetroPixel: return "RetroPixel";
        case StyleBias::Architectural: return "Architectural";
        default: return "CleanModern";
    }
}

StyleBias AIPreset::styleBiasFromString(const std::string& str) {
    if (str == "CleanModern") return StyleBias::CleanModern;
    if (str == "NeonGlow") return StyleBias::NeonGlow;
    if (str == "DemonExtreme") return StyleBias::DemonExtreme;
    if (str == "RetroPixel") return StyleBias::RetroPixel;
    if (str == "Architectural") return StyleBias::Architectural;
    return StyleBias::CleanModern;
}

}
