#pragma once

#include <Atomic/Graphics/IndexBuffer.h>
#include <Atomic/Graphics/VertexBuffer.h>
#include <Atomic/Graphics/Geometry.h>
#include <Atomic/Graphics/Model.h>

using namespace Atomic;

namespace ToolCore
{

/// Model packer/unpacker from/to packed representation
class ModelPacker
{

public:

    bool Unpack(Model* model);

private:

    SharedPtr<Model> model_;


};

}
