
#pragma once

#include <Atomic/Graphics/Model.h>

namespace Atomic
{
    class Geometry;
}

using namespace Atomic;

namespace ToolCore
{

class MeshLightmapUVGen
{

public:

    struct Settings
    {

    };

    MeshLightmapUVGen(Model* model, const Settings& settings);

    bool Generate();

private:

    class LightmapMesh
    {


private:

        Geometry* geometry_;

        // in global UV
        unsigned vertexStart_;
        unsigned vertexCount_;
    };

    SharedPtr<Model> model_;

    PODVector<LightmapMesh*> lightmapMeshes_;

    Settings setting_;

};

}
