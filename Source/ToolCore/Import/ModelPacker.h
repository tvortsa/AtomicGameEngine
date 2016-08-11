#pragma once

#include <Atomic/Graphics/IndexBuffer.h>
#include <Atomic/Graphics/VertexBuffer.h>
#include <Atomic/Graphics/Geometry.h>
#include <Atomic/Graphics/Model.h>

using namespace Atomic;

namespace ToolCore
{

class MPGeometry;

struct MPVertex
{
    Vector3 position_;
    Vector3 normal_;
    Vector4 tangent_;
    Vector2 uv0_;
    Vector2 uv1_;
};

class MPGeometry : public RefCounted
{
    ATOMIC_REFCOUNTED(MPGeometry)

public:

    SharedPtr<Geometry> geometry_;
    PODVector<MPVertex> vertices_;
    PODVector<VertexElement> elements_;
    SharedArrayPtr<unsigned> indices_;
    unsigned numIndices_;

};

class MPLODLevel : public RefCounted
{
    ATOMIC_REFCOUNTED(MPLODLevel)

public:

    Vector<SharedPtr<MPGeometry>> mpGeometry_;

    unsigned level_;
};

/// Model packer/unpacker from/to packed representation (destructive!)
class ModelPacker : public RefCounted
{
    ATOMIC_REFCOUNTED(ModelPacker)

public:

    bool Unpack(Model* model);

    const String& GetErrorText() const { return errorText_; }

    Vector<SharedPtr<MPLODLevel>> lodLevels_;

private:

    void SetError(const String& errorText) { errorText_ = errorText; }

    String errorText_;

    bool UnpackLODLevel(unsigned level);

    bool UnpackGeometry(MPLODLevel* level, Geometry* geometry);

    SharedPtr<Model> model_;

};

}
