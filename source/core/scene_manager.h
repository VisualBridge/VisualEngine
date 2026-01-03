#pragma once
#include <vector>
#include <memory>
#include <string>
#include "../core/entity.h"
#include "../components/component.h"
#include "../components/mesh_component.h"
#include "../components/script_component.h"

class SceneManager {
public:
    static SceneManager& Instance() {
        static SceneManager instance;
        return instance;
    }

    void SetActiveScene(std::vector<std::shared_ptr<Entity>>* scene) {
        activeScene = scene;
        currentScenePath = "";
    }

    void SetScenePath(const std::string& path) {
        currentScenePath = path;
    }

    std::string GetScenePath() const {
        return currentScenePath;
    }

    std::vector<std::shared_ptr<Entity>>* GetActiveScene() {
        return activeScene;
    }

    void SaveSceneState() {
        savedScene.clear();
        if (!activeScene) return;

        for (auto& entity : *activeScene) {
            savedScene.push_back(CloneEntity(entity));
        }
    }

    void RestoreSceneState() {
        if (!activeScene || savedScene.empty()) return;

        activeScene->clear();
        for (auto& entity : savedScene) {
            activeScene->push_back(CloneEntity(entity));
        }
        savedScene.clear();
    }

private:
    SceneManager() : activeScene(nullptr) {}

    std::shared_ptr<Entity> CloneEntity(std::shared_ptr<Entity> original) {
        auto clone = std::make_shared<Entity>();
        clone->Name = original->Name;

        auto origTransform = original->getComponent<TransformComponent>();
        if (origTransform) {
            auto cloneTransform = clone->getComponent<TransformComponent>();
            cloneTransform->translation = origTransform->translation;
            cloneTransform->rotation = origTransform->rotation;
            cloneTransform->scale = origTransform->scale;
        }

        auto origMesh = original->getComponent<MeshComponent>();
        if (origMesh) {
            auto cloneMesh = clone->addComponent<MeshComponent>();
            cloneMesh->meshModel = origMesh->meshModel;
        }

        auto origScript = original->getComponent<ScriptComponent>();
        if (origScript) {
            auto cloneScript = clone->addComponent<ScriptComponent>();
            cloneScript->scriptPath = origScript->scriptPath;
            cloneScript->className = origScript->className;
            if (!origScript->scriptPath.empty() && !origScript->className.empty()) {
                cloneScript->LoadScript(origScript->scriptPath, origScript->className);
            }
        }

        return clone;
    }

    std::vector<std::shared_ptr<Entity>>* activeScene;
    std::vector<std::shared_ptr<Entity>> savedScene;
    std::string currentScenePath;
};