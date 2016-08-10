
#include "MeshLightmapUVGen.h"

#include <Atomic/Graphics/IndexBuffer.h>
#include <Atomic/Graphics/VertexBuffer.h>
#include <Atomic/Graphics/Geometry.h>

namespace  ToolCore
{


MeshLightmapUVGen::MeshLightmapUVGen(Model* model, const Settings& settings) :
    model_(model),
    setting_(settings)
{


}

bool MeshLightmapUVGen::Generate()
{
    if (model_.Null())
        return false;

    if (!model_->GetNumGeometries())
        return false;


    for (unsigned i = 0; i < model_->GetNumGeometries(); i++)
    {
        const unsigned char* positionData = 0;
        const unsigned char* normalData = 0;
        const unsigned char* indexData = 0;
        unsigned positionStride = 0;
        unsigned normalStride = 0;
        unsigned indexStride = 0;

        Geometry* geo = model_->GetGeometry(i, 0);

        if (geo->GetPrimitiveType() != TRIANGLE_LIST)
            return false;

        IndexBuffer* ib = geo->GetIndexBuffer();

        if (!ib)
            return false;

        PODVector<unsigned> indices;
        indices.Resize(geo->GetIndexCount());

        indexData = ib->GetShadowData();

        if (!indexData)
            return false;

        unsigned indexStart = geo->GetIndexStart();

        if (indexStride == sizeof(unsigned short))
        {
            // 16-bit indices
            const unsigned short* pindices = ((const unsigned short*)indexData) + indexStart;

            for (unsigned j = 0; j < geo->GetIndexCount(); j++)
            {
                indices[j] = (unsigned) (*pindices++);
            }
        }
        else
        {
            // 32-bit indices
            const unsigned* pindices = ((const unsigned*)indexData) + indexStart;

            for (unsigned j = 0; j < geo->GetIndexCount(); j++)
            {
                indices[j] = (*pindices++);
            }

        }

        VertexBuffer* vb = geo->GetVertexBuffer(0);

        if (!vb)
            return false;

        unsigned elementMask = vb->GetElementMask();

        unsigned char* data = vb->GetShadowData();

        if (!data)
            return false;

        if (elementMask & MASK_POSITION)
        {
            positionData = data;
            positionStride = vb->GetVertexSize();
        }
        if (elementMask & MASK_NORMAL)
        {
            normalData = data + vb->GetElementOffset(SEM_NORMAL);
            normalStride = vb->GetVertexSize();
        }

        if (!positionData)
            return false;

        bool hasNormals = normalData != 0;

    }

    /*

    // Setup thekla input mesh
    Thekla::Atlas_Input_Mesh* tmesh = new Thekla::Atlas_Input_Mesh();

    tmesh->vertex_count = mesh->mNumVertices;
    tmesh->vertex_array = new Thekla::Atlas_Input_Vertex[tmesh->vertex_count];

    unsigned numValidFaces = 0;

    for (unsigned index = 0; index < mesh->mNumFaces; index++)
    {
        if (mesh->mFaces[index].mNumIndices == 3)
            numValidFaces++;
    }

    tmesh->face_count = numValidFaces;
    tmesh->face_array = new Thekla::Atlas_Input_Face[tmesh->face_count];

    // copy vertex data
    for (unsigned index = 0; index < mesh->mNumVertices; index++)
    {
        Vector3 vertex = vertexTransform * ToVector3(mesh->mVertices[index]);
        Vector3 normal = normalTransform * ToVector3(mesh->mNormals[index]);

        tmesh->vertex_array[index].position[0] = vertex.x_;
        tmesh->vertex_array[index].position[1] = vertex.y_;
        tmesh->vertex_array[index].position[2] = vertex.z_;

        tmesh->vertex_array[index].normal[0] = normal.x_;
        tmesh->vertex_array[index].normal[1] = normal.y_;
        tmesh->vertex_array[index].normal[2] = normal.z_;
    }

    // copy face data
    unsigned curFace = 0;
    for (unsigned index = 0; index < mesh->mNumFaces; index++)
    {
        if (mesh->mFaces[index].mNumIndices != 3)
            continue;

        tmesh->face_array[curFace].vertex_index[0] = mesh->mFaces[index].mIndices[0];
        tmesh->face_array[curFace].vertex_index[1] = mesh->mFaces[index].mIndices[1];
        tmesh->face_array[curFace].vertex_index[2] = mesh->mFaces[index].mIndices[2];

        curFace++;
    }

    Thekla::Atlas_Options atlasOptions;
    atlas_set_default_options(&atlasOptions);
    atlasOptions.packer_options.witness.texel_area = 32;
    Thekla::Atlas_Error error = Thekla::Atlas_Error_Success;
    Thekla::Atlas_Output_Mesh* outputMesh = atlas_generate(tmesh, &atlasOptions, &error);

    delete [] tmesh->vertex_array;
    delete [] tmesh->face_array;
    delete tmesh;

    */

    return true;

}

}
