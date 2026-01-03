#pragma once
#include <new>
#include <angelscript.h>
#include <angelscript/scriptstdstring/scriptstdstring.h>
#include <angelscript/scriptbuilder/scriptbuilder.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include "entity.h"
#include "../components/transform_component.h"

inline void MessageCallback(const asSMessageInfo* msg, void* param) {
    const char* type = "ERR ";
    if (msg->type == asMSGTYPE_WARNING) type = "WARN";
    else if (msg->type == asMSGTYPE_INFORMATION) type = "INFO";

    std::cout << msg->section << " (" << msg->row << ", " << msg->col << ") : "
        << type << " : " << msg->message << std::endl;
}

class ScriptSystem {
public:
    static ScriptSystem& Instance() {
        static ScriptSystem instance;
        return instance;
    }

    bool Initialize() {
        engine = asCreateScriptEngine();
        if (!engine) {
            std::cerr << "Failed to create script engine" << std::endl;
            return false;
        }

        engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        RegisterStdString(engine);
        RegisterEngineAPI();

        std::cout << "AngelScript initialized successfully" << std::endl;
        return true;
    }

    void Shutdown() {
        if (engine) {
            engine->ShutDownAndRelease();
            engine = nullptr;
        }
    }

    asIScriptEngine* GetEngine() { return engine; }

    asIScriptModule* LoadScript(const std::string& moduleName, const std::string& scriptPath) {
        CScriptBuilder builder;
        int r = builder.StartNewModule(engine, moduleName.c_str());
        if (r < 0) {
            std::cerr << "Failed to start new module: " << moduleName << std::endl;
            return nullptr;
        }

        r = builder.AddSectionFromFile(scriptPath.c_str());
        if (r < 0) {
            std::cerr << "Failed to load script file: " << scriptPath << std::endl;
            return nullptr;
        }

        r = builder.BuildModule();
        if (r < 0) {
            std::cerr << "Failed to build module: " << moduleName << std::endl;
            return nullptr;
        }

        std::cout << "Script loaded: " << scriptPath << std::endl;
        return engine->GetModule(moduleName.c_str());
    }

    asIScriptObject* CreateScriptObject(asIScriptModule* module, const std::string& className) {
        asITypeInfo* type = module->GetTypeInfoByName(className.c_str());
        if (!type) {
            std::cerr << "Script class not found: " << className << std::endl;
            return nullptr;
        }

        asIScriptFunction* factory = type->GetFactoryByDecl((className + " @" + className + "()").c_str());
        if (!factory) {
            std::cerr << "Factory function not found for: " << className << std::endl;
            return nullptr;
        }

        asIScriptContext* ctx = engine->CreateContext();
        ctx->Prepare(factory);
        int r = ctx->Execute();

        asIScriptObject* obj = nullptr;
        if (r == asEXECUTION_FINISHED) {
            obj = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
            if (obj) obj->AddRef();
        }

        ctx->Release();
        return obj;
    }

    void CallMethod(asIScriptObject* obj, const std::string& methodName) {
        if (!obj) return;

        asITypeInfo* type = obj->GetObjectType();
        asIScriptFunction* func = type->GetMethodByName(methodName.c_str());
        if (!func) return;

        asIScriptContext* ctx = engine->CreateContext();
        ctx->Prepare(func);
        ctx->SetObject(obj);
        ctx->Execute();
        ctx->Release();
    }

    void CallMethodWithFloat(asIScriptObject* obj, const std::string& methodName, float value) {
        if (!obj) return;

        asITypeInfo* type = obj->GetObjectType();
        asIScriptFunction* func = type->GetMethodByName(methodName.c_str());
        if (!func) return;

        asIScriptContext* ctx = engine->CreateContext();
        ctx->Prepare(func);
        ctx->SetObject(obj);
        ctx->SetArgFloat(0, value);
        ctx->Execute();
        ctx->Release();
    }

private:
    ScriptSystem() : engine(nullptr) {}

    void RegisterEngineAPI() {
        engine->RegisterObjectType("vec3", sizeof(glm::vec3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK);
        engine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x));
        engine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y));
        engine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z));
        engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Vec3Constructor), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(Vec3ConstructorFloats), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectType("Entity", 0, asOBJ_REF | asOBJ_NOCOUNT);
        engine->RegisterObjectMethod("Entity", "TransformComponent@ getTransform()", asFUNCTION(GetTransformComponent), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectProperty("Entity", "string Name", asOFFSET(Entity, Name));
        engine->RegisterObjectType("TransformComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
        engine->RegisterObjectProperty("TransformComponent", "vec3 translation", asOFFSET(TransformComponent, translation));
        engine->RegisterObjectProperty("TransformComponent", "vec3 rotation", asOFFSET(TransformComponent, rotation));
        engine->RegisterObjectProperty("TransformComponent", "vec3 scale", asOFFSET(TransformComponent, scale));
        engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(ScriptPrint), asCALL_CDECL);
    }

    static void Vec3Constructor(glm::vec3* self) {
        new(self) glm::vec3(0.0f, 0.0f, 0.0f);
    }

    static void Vec3ConstructorFloats(float x, float y, float z, glm::vec3* self) {
        new(self) glm::vec3(x, y, z);
    }

    static TransformComponent* GetTransformComponent(Entity* entity) {
        auto comp = entity->getComponent<TransformComponent>();
        return comp.get();
    }

    static void ScriptPrint(const std::string& msg) {
        std::cout << "[Script] " << msg << std::endl;
    }

    asIScriptEngine* engine;
};