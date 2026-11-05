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

    auto pool = matjson::Value::array();
    for (const auto& obj : decorationPool) {
        matjson::Value item;
        item["objectID"] = obj.objectID;
        item["weight"] = obj.weight;
        item["category"] = obj.category;
        item["requiresGlow"] = obj.requiresGlow;
        item["isDetail"] = obj.isDetail;
        pool.push(item);
    }
    json["decorationPool"] = pool;

    matjson::Value weights;
    for (const auto& [key, value] : styleWeights) weights[key] = value;
    json["styleWeights"] = weights;
    return json;
}

AIPreset AIPreset::fromJson(const matjson::Value& json) {
    AIPreset preset;
    preset.id = json["id"].asString().unwrapOr("");
    preset.name = json["name"].asString().unwrapOr("Unnamed");
    preset.description = json["description"].asString().unwrapOr("");
    preset.styleBias = styleBiasFromString(json["styleBias"].asString().unwrapOr("CleanModern"));

    if (json.contains("densityCurve")) {
        const auto& dc = json["densityCurve"];
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

    if (json.contains("decorationPool")) {
        auto arrayResult = json["decorationPool"].asArray();
        if (arrayResult) {
            for (const auto& item : *arrayResult) {
                DecorationObject obj{};
                obj.objectID = item["objectID"].asInt().unwrapOr(100);
                obj.weight = static_cast<float>(item["weight"].asDouble().unwrapOr(1.0));
                obj.category = item["category"].asString().unwrapOr("fill");
                obj.requiresGlow = item["requiresGlow"].asBool().unwrapOr(false);
                obj.isDetail = item["isDetail"].asBool().unwrapOr(false);
                preset.decorationPool.push_back(obj);
            }
        }
    }

    if (json.contains("styleWeights") && json["styleWeights"].isObject()) {
        for (const auto& [key, value] : json["styleWeights"]) {
            preset.styleWeights[key] = static_cast<float>(value.asDouble().unwrapOr(0.0));
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
    if (str == "NeonGlow") return StyleBias::NeonGlow;
    if (str == "DemonExtreme") return StyleBias::DemonExtreme;
    if (str == "RetroPixel") return StyleBias::RetroPixel;
    if (str == "Architectural") return StyleBias::Architectural;
    return StyleBias::CleanModern;
}

}
