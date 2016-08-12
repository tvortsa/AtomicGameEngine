
#pragma once

#include <ToolCore/Import/ModelPacker.h>

using namespace Atomic;
using namespace ToolCore;

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

    SharedPtr<ModelPacker> modelPacker_;

};

}
