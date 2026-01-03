#pragma once
#include "component.h"
#include <string>
#include <angelscript.h>
#include "../core/script_system.h"

class ScriptComponent : public Component {
public:
    std::string scriptPath;
    std::string className;
    asIScriptObject* scriptObject = nullptr;
    asIScriptModule* module = nullptr;

    ~ScriptComponent() {
        if (scriptObject) {
            scriptObject->Release();
            scriptObject = nullptr;
        }
    }

    void LoadScript(const std::string& path, const std::string& cls) {
        scriptPath = path;
        className = cls;

        if (scriptObject) {
            scriptObject->Release();
            scriptObject = nullptr;
        }

        std::string moduleName = "Module_" + std::to_string((size_t)this);
        module = ScriptSystem::Instance().LoadScript(moduleName, scriptPath);

        if (module) {
            scriptObject = ScriptSystem::Instance().CreateScriptObject(module, className);
            if (scriptObject) {
                SetScriptOwner();
            }
        }
    }

    void onInit() override {
        if (scriptObject) {
            ScriptSystem::Instance().CallMethod(scriptObject, "OnInit");
        }
    }

    void onUpdate(float delta) override {
        if (scriptObject) {
            ScriptSystem::Instance().CallMethodWithFloat(scriptObject, "OnUpdate", delta);
        }
    }

    void CallCustomMethod(const std::string& methodName) {
        if (scriptObject) {
            ScriptSystem::Instance().CallMethod(scriptObject, methodName);
        }
    }

private:
    void SetScriptOwner() {
        if (!scriptObject || !owner) return;

        asITypeInfo* type = scriptObject->GetObjectType();
        asIScriptFunction* func = type->GetMethodByName("SetOwner");
        if (!func) return;

        asIScriptContext* ctx = ScriptSystem::Instance().GetEngine()->CreateContext();
        ctx->Prepare(func);
        ctx->SetObject(scriptObject);
        ctx->SetArgAddress(0, owner);
        ctx->Execute();
        ctx->Release();
    }
};