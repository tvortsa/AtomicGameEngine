
#pragma once

#include <ToolCore/Import/ModelPacker.h>

using namespace Atomic;
using namespace ToolCore;

namespace Atomic
{
    class Image;
}

namespace AtomicGlow
{

class ModelAOBake : public Object
{
    ATOMIC_OBJECT(ModelAOBake, Object)

    public:

    ModelAOBake(Context* context);
    virtual ~ModelAOBake();

    bool LoadModel(const String& pathName);

private:

    void TraceAORays(unsigned nsample, float multiply = 1.0f);

    // generate current LOD AO map
    bool GenerateLODLevelAOMap(MPLODLevel* lodLevel);

    //unsigned GetImageSize(float pixelsPerUnit, bool powerOfTwo);

    struct LMVertex
    {
        Vector3 position_;
        Vector3 normal_;
        Vector2 uv0_;
        Vector2 uv1_;
    };

    struct LMLexel
    {
        Vector3 position_;
        Vector3 normal_;
        // -1, -1 == not used
        Vector2 pixelCoord_;
        Color color_;
    };


    PODVector<LMVertex> lmVertices_;
    PODVector<LMLexel> lmLexels_;

    SharedArrayPtr<unsigned> indices_;

    unsigned numIndices_;

    SharedPtr<MPLODLevel> curLOD_;
    SharedPtr<ModelPacker> modelPacker_;

    SharedPtr<Image> lightmap_;

};

}
