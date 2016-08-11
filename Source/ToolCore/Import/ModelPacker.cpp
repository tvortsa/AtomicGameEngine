
#include <Atomic/Graphics/IndexBuffer.h>
#include <Atomic/Graphics/VertexBuffer.h>

#include "ModelPacker.h"


namespace ToolCore
{

bool ModelPacker::Unpack(Model* model)
{

    model_ = model;

    unsigned maxLOD = 0;

    for (unsigned i = 0; i < model_->GetNumGeometries(); i++)
    {
        unsigned numLOD = model_->GetNumGeometryLodLevels(i);

        if (numLOD > maxLOD)
            maxLOD = numLOD;
    }

    if (!maxLOD)
    {
        SetError("No LOD in model");
        return false;
    }

    for (unsigned i = 0; i < maxLOD; i++)
    {
        if (!UnpackLODLevel(i))
            return false;
    }

    return true;
}

bool ModelPacker::UnpackLODLevel(unsigned level)
{
    PODVector<Geometry*> lodGeo;

    for (unsigned i = 0; i < model_->GetNumGeometries(); i++)
    {
        Geometry * geo = model_->GetGeometry(i, level);

        if (geo)
            lodGeo.Push(geo);
    }

    if (!level && !lodGeo.Size())
    {
        SetError("No geometry in LOD 0 for model");
        return false;
    }

    if (!lodGeo.Size())
        return true;

    SharedPtr<MPLODLevel> lodLevel (new MPLODLevel());

    for (unsigned i = 0; i < lodGeo.Size(); i++)
    {
        if (!UnpackGeometry(lodLevel, lodGeo[i]))
            return false;
    }

    lodLevel->level_ = level;

    lodLevels_.Push(lodLevel);

    return true;

}

bool ModelPacker::UnpackGeometry(MPLODLevel *level, Geometry* geometry)
{
    SharedPtr<MPGeometry> mpGeo(new MPGeometry());

    // We only support vertex buffer operations on vertex buffer 0
    // TODO: should it be an error if > 1 vertex buffer in geo since we might be destructive to index buffers?

    const unsigned char* indexData = 0;
    unsigned indexSize = 0;

    unsigned vertexSize = 0;
    const unsigned char* vertexData = 0;

    const PODVector<VertexElement>* elements = 0;

    geometry->GetRawData(vertexData, vertexSize, indexData, indexSize, elements);

    if (!indexData || !indexSize || !vertexData || !vertexSize || !elements)
    {
        SetError("ModelPacker::UnpackGeometry - Failed to get raw data for geometry");
        return false;
    }

    // VERTEX DATA

    mpGeo->elements_ = *elements;

    const unsigned char* positionData = 0;
    const unsigned char* normalData = 0;
    const unsigned char* tangentData = 0;
    const unsigned char* uv0Data = 0;
    const unsigned char* uv1Data = 0;

    unsigned vertexStart = geometry->GetVertexStart();
    unsigned vertexCount = geometry->GetVertexCount();

    vertexData += vertexStart * vertexSize;

    for (unsigned i = 0; i < elements->Size(); i++)
    {
        VertexElement element = elements->At(i);

        if (element.type_ == TYPE_VECTOR3 && element.semantic_ == SEM_POSITION)
        {
            positionData = vertexData + element.offset_;
        }
        else if (element.type_ == TYPE_VECTOR3 && element.semantic_ == SEM_NORMAL)
        {
            normalData = vertexData + element.offset_;
        }
        else if (element.type_ == TYPE_VECTOR4 && element.semantic_ == SEM_TANGENT)
        {
            tangentData = vertexData + element.offset_;
        }
        else if (element.type_ == TYPE_VECTOR2 && element.semantic_ == SEM_TEXCOORD)
        {
            if (!uv0Data)
            {
                uv0Data = vertexData + element.offset_;
            }
            else
            {
                uv1Data = vertexData + element.offset_;
            }
        }
    }

    if (!positionData)
    {
        SetError("Geometry has no position data");
        return false;
    }

    mpGeo->vertices_.Resize(vertexCount);

    MPVertex* v = &mpGeo->vertices_[0];

    for (unsigned i = 0; i < vertexCount; i++, v++)
    {
        float* fp = (float *) positionData;

        v->position_.x_ = fp[0];
        v->position_.y_ = fp[1];
        v->position_.z_ = fp[2];

        positionData += vertexSize;

        if (normalData)
        {
            fp = (float *) normalData;

            v->normal_.x_ = fp[0];
            v->normal_.y_ = fp[1];
            v->normal_.z_ = fp[2];

            normalData += vertexSize;
        }

        if (tangentData)
        {
            fp = (float *) tangentData;

            v->tangent_.x_ = fp[0];
            v->tangent_.y_ = fp[1];
            v->tangent_.z_ = fp[2];
            v->tangent_.w_ = fp[3];

            tangentData += vertexSize;
        }

        if (uv0Data)
        {
            fp = (float *) uv0Data;

            v->uv0_.x_ = fp[0];
            v->uv0_.y_ = fp[1];

            uv0Data += vertexSize;
        }

        if (uv1Data)
        {
            fp = (float *) uv1Data;

            v->uv1_.x_ = fp[0];
            v->uv1_.y_ = fp[1];

            uv1Data += vertexSize;
        }

    }

    // INDICES

    unsigned indexStart = geometry->GetIndexStart();
    unsigned indexCount = geometry->GetIndexCount();

    // source indices converted to unsigned value
    PODVector<unsigned> geoIndices;

    if (indexSize == sizeof(unsigned short))
    {
        // 16-bit indices
        const unsigned short* indices = ((const unsigned short*)indexData) + indexStart;
        const unsigned short* indicesEnd = indices + indexCount;

        while (indices < indicesEnd)
        {
            unsigned idx = (unsigned) *indices++;

            if (idx >= vertexStart && idx < (vertexStart + vertexCount))
            {
                geoIndices.Push(idx - vertexStart);
            }
        }
    }
    else
    {
        // 32-bit indices
        const unsigned* indices = ((const unsigned*)indexData) + indexStart;
        const unsigned* indicesEnd = indices + indexCount;

        while (indices < indicesEnd)
        {
            unsigned idx = *indices++;

            if (idx >= vertexStart && idx < (vertexStart + vertexCount))
            {
                geoIndices.Push(idx - vertexStart);
            }
        }

    }

    mpGeo->indices_ = new unsigned[geoIndices.Size()];
    mpGeo->numIndices_ = geoIndices.Size();
    memcpy(&mpGeo->indices_[0], &geoIndices[0], sizeof(unsigned) * geoIndices.Size());

    level->mpGeometry_.Push(mpGeo);

    return true;
}

}
