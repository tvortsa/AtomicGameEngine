#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>

#include <Atomic/Resource/ResourceCache.h>

#include "ModelAOBake.h"

namespace AtomicGlow
{

ModelAOBake::ModelAOBake(Context* context) : Object(context)
{

}

ModelAOBake::~ModelAOBake()
{

}

bool ModelAOBake::LoadModel(const String& pathName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Model* model = cache->GetResource<Model>(pathName);

    if (!model)
        return false;

    modelPacker_ = new ModelPacker(context_);

    if (!modelPacker_->Unpack(model))
        return false;

    return true;
}

}

