#pragma once
#include "component.h"
#include "../renderer/model.h"

class MeshComponent : public Component {
public:
    Model* meshModel = nullptr;
};