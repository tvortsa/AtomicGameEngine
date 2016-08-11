
#pragma once

#include "ModelPacker.h"

using namespace Atomic;

namespace Thekla
{

struct Atlas_Output_Mesh;
struct Atlas_Input_Mesh;

}

namespace ToolCore
{

class ModelPacker;

class MeshLightmapUVGen
{

public:

    struct Settings
    {
        bool genChartID_;

        Settings()
        {
            genChartID_ = false;
        }

    };

    MeshLightmapUVGen(Model* model, const Settings& settings);

    bool Generate();

private:

    void WriteLightmapUVCoords();

    struct LMVertex
    {
        MPGeometry* geometry_;
        unsigned geometryIdx_;
        unsigned originalVertex_;
    };

    SharedPtr<Model> model_;
    SharedPtr<ModelPacker> modelPacker_;

    SharedPtr<MPLODLevel> curLOD_;

    SharedArrayPtr<LMVertex> lmVertices_;

    Settings settings_;

    Thekla::Atlas_Output_Mesh* tOutputMesh_;
    Thekla::Atlas_Input_Mesh* tInputMesh_;

};

}
