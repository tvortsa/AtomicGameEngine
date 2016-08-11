
#include <ThirdParty/thekla/thekla_atlas.h>

#include <Atomic/IO/Log.h>

#include "ModelPacker.h"
#include "MeshLightmapUVGen.h"


namespace  ToolCore
{


MeshLightmapUVGen::MeshLightmapUVGen(Context* context, Model* model, const Settings& settings) : Object(context),
    model_(model),
    modelPacker_(new ModelPacker(context)),
    settings_(settings),
    tOutputMesh_(0),
    tInputMesh_(0)
{


}

MeshLightmapUVGen::~MeshLightmapUVGen()
{

}

void MeshLightmapUVGen::WriteLightmapUVCoords()
{
    Thekla::atlas_write_debug_textures(tOutputMesh_, tInputMesh_, "/Users/jenge/Desktop/lmWorldSpaceTexture.png", "/Users/jenge/Desktop/lmNormalTexture.png");

    SharedArrayPtr<unsigned> vertexCounts(new unsigned[curLOD_->mpGeometry_.Size()]);
    SharedArrayPtr<unsigned> indexCounts(new unsigned[curLOD_->mpGeometry_.Size()]);
    memset(&vertexCounts[0], 0, sizeof(unsigned) * curLOD_->mpGeometry_.Size());
    memset(&indexCounts[0], 0, sizeof(unsigned) * curLOD_->mpGeometry_.Size());

    for (unsigned i = 0; i < tOutputMesh_->index_count/3; i++)
    {
        unsigned v0 = (unsigned) tOutputMesh_->index_array[i * 3];
        unsigned v1 = (unsigned) tOutputMesh_->index_array[i * 3 + 1];
        unsigned v2 = (unsigned) tOutputMesh_->index_array[i * 3 + 2];

        Thekla::Atlas_Output_Vertex& tv0 = tOutputMesh_->vertex_array[v0];
        Thekla::Atlas_Output_Vertex& tv1 = tOutputMesh_->vertex_array[v1];
        Thekla::Atlas_Output_Vertex& tv2 = tOutputMesh_->vertex_array[v2];

        LMVertex& lv0 = lmVertices_[tv0.xref];
        LMVertex& lv1 = lmVertices_[tv1.xref];
        LMVertex& lv2 = lmVertices_[tv2.xref];

        unsigned geometryIdx = lv0.geometryIdx_;

        // check for mixed geometry in triangle
        if (geometryIdx != lv1.geometryIdx_ || geometryIdx != lv2.geometryIdx_)
        {
            assert(0);
        }

        indexCounts[geometryIdx] += 3;

        if ((v0 + 1) >= vertexCounts[geometryIdx])
            vertexCounts[geometryIdx] = v0 + 1;
        if ((v1 + 1) >= vertexCounts[geometryIdx])
            vertexCounts[geometryIdx] = v1 + 1;
        if ((v2 + 1) >= vertexCounts[geometryIdx])
            vertexCounts[geometryIdx] = v2 + 1;
    }

    float uscale = 1.f / tOutputMesh_->atlas_width;
    float vscale = 1.f / tOutputMesh_->atlas_height;

    for (unsigned i = 0; i < curLOD_->mpGeometry_.Size(); i++)
    {
        PODVector<MPVertex> nVertices;
        nVertices.Resize(vertexCounts[i]);
        SharedArrayPtr<unsigned> nIndices(new unsigned[indexCounts[i]]);

        memset(&nVertices[0], 0, sizeof(MPVertex) * vertexCounts[i]);
        memset(&nIndices[0], 0, sizeof(unsigned) * indexCounts[i]);

        SharedArrayPtr<int> mappedVertices(new int[vertexCounts[i]]);
        for (unsigned j = 0; j < vertexCounts[i]; j++)
        {
            mappedVertices[j] = -1;
        }

        unsigned curIndex = 0;
        unsigned curVertex = 0;

        MPGeometry* mpGeo = curLOD_->mpGeometry_[i];

        for (unsigned j = 0; j < tOutputMesh_->index_count/3; j++)
        {
            unsigned v0 = (unsigned) tOutputMesh_->index_array[j * 3];
            unsigned v1 = (unsigned) tOutputMesh_->index_array[j * 3 + 1];
            unsigned v2 = (unsigned) tOutputMesh_->index_array[j * 3 + 2];

            Thekla::Atlas_Output_Vertex& tv0 = tOutputMesh_->vertex_array[v0];
            Thekla::Atlas_Output_Vertex& tv1 = tOutputMesh_->vertex_array[v1];
            Thekla::Atlas_Output_Vertex& tv2 = tOutputMesh_->vertex_array[v2];

            LMVertex& lv0 = lmVertices_[tv0.xref];
            LMVertex& lv1 = lmVertices_[tv1.xref];
            LMVertex& lv2 = lmVertices_[tv2.xref];

            unsigned geometryIdx = lv0.geometryIdx_;

            if (geometryIdx != i)
                continue;

            // Map vertices

            if (mappedVertices[v0] == -1)
            {
                // we haven't mapped this vertice
                nVertices[curVertex] = mpGeo->vertices_[lv0.originalVertex_];
                nVertices[curVertex].uv1_ = Vector2(tv0.uv[0] * uscale, tv0.uv[1] * vscale);
                mappedVertices[v0] = curVertex;
                v0 = curVertex++;
            }
            else
            {
                v0 = mappedVertices[v0];
            }

            if (mappedVertices[v1] == -1)
            {
                // we haven't mapped this vertice
                nVertices[curVertex] = mpGeo->vertices_[lv1.originalVertex_];
                nVertices[curVertex].uv1_ = Vector2(tv1.uv[0] * uscale,  tv1.uv[1] * vscale);
                mappedVertices[v1] = curVertex;
                v1 = curVertex++;
            }
            else
            {
                v1 = mappedVertices[v1];
            }

            if (mappedVertices[v2] == -1)
            {
                // we haven't mapped this vertice
                nVertices[curVertex] = mpGeo->vertices_[lv2.originalVertex_];
                nVertices[curVertex].uv1_ = Vector2(tv2.uv[0] * uscale, tv2.uv[1] * vscale);
                mappedVertices[v2] = curVertex;
                v2 = curVertex++;
            }
            else
            {
                v2 = mappedVertices[v2];
            }

            nIndices[curIndex++] = v0;
            nIndices[curIndex++] = v1;
            nIndices[curIndex++] = v2;

        }

        mpGeo->vertices_ = nVertices;
        mpGeo->indices_ = nIndices;

        // Check whether we need to add UV1 semantic

        PODVector<VertexElement> nElements;

        unsigned texCoordCount = 0;
        for (unsigned j = 0; j < mpGeo->elements_.Size(); j++)
        {
            VertexElement element = mpGeo->elements_[j];

            if (element.type_ == TYPE_VECTOR2 && element.semantic_ == SEM_TEXCOORD)
                texCoordCount++;
        }

        if (texCoordCount <= 1)
        {
            bool added = false;
            for (unsigned j = 0; j < mpGeo->elements_.Size(); j++)
            {
                VertexElement element = mpGeo->elements_[j];

                nElements.Push(element);

                if ( (element.type_ == TYPE_VECTOR2 && element.semantic_ == SEM_TEXCOORD) || (!added && j == (mpGeo->elements_.Size() - 1) ) )
                {
                    added = true;
                    VertexElement element(TYPE_VECTOR2, SEM_TEXCOORD, 1);
                    nElements.Push(element);
                }

            }

            mpGeo->elements_ = nElements;
        }

    }

}

bool MeshLightmapUVGen::Generate()
{
    if (model_.Null())
        return false;    

    if (!modelPacker_->Unpack(model_))
    {
        return false;
    }

    for (unsigned i = 0; i < modelPacker_->lodLevels_.Size(); i++)
    {

        curLOD_ = modelPacker_->lodLevels_[i];

        // combine all LOD vertices/indices

        unsigned totalVertices = 0;
        unsigned totalIndices = 0;
        for (unsigned j = 0; j < curLOD_->mpGeometry_.Size(); j++)
        {
            MPGeometry* geo = curLOD_->mpGeometry_[j];
            totalVertices += geo->vertices_.Size();
            totalIndices += geo->numIndices_;
        }

        // Setup thekla input mesh
        tInputMesh_ = new Thekla::Atlas_Input_Mesh();

        // Allocate vertex arrays
        lmVertices_ = new LMVertex[totalVertices];

        tInputMesh_->vertex_count = totalVertices;
        tInputMesh_->vertex_array = new Thekla::Atlas_Input_Vertex[tInputMesh_->vertex_count];

        tInputMesh_->face_count = totalIndices / 3;
        tInputMesh_->face_array = new Thekla::Atlas_Input_Face[tInputMesh_->face_count];

        unsigned vCount = 0;
        unsigned fCount = 0;

        for (unsigned j = 0; j < curLOD_->mpGeometry_.Size(); j++)
        {
            MPGeometry* geo = curLOD_->mpGeometry_[j];

            unsigned vertexStart = vCount;

            for (unsigned k = 0; k < geo->vertices_.Size(); k++, vCount++)
            {
                const MPVertex& mpv = geo->vertices_[k];

                LMVertex& lmv = lmVertices_[vCount];
                Thekla::Atlas_Input_Vertex& tv = tInputMesh_->vertex_array[vCount];

                lmv.geometry_ = geo;
                lmv.geometryIdx_ = j;
                lmv.originalVertex_ = k;

                tv.position[0] = mpv.position_.x_;
                tv.position[1] = mpv.position_.y_;
                tv.position[2] = mpv.position_.z_;

                tv.normal[0] = mpv.normal_.x_;
                tv.normal[1] = mpv.normal_.y_;
                tv.normal[2] = mpv.normal_.z_;

                tv.uv[0] = mpv.uv0_.x_;
                tv.uv[1] = mpv.uv0_.y_;

                // this appears unused in thekla atlas?
                tv.first_colocal = vCount;

            }

            for (unsigned k = 0; k < geo->numIndices_/3; k++, fCount++)
            {
                Thekla::Atlas_Input_Face& tface = tInputMesh_->face_array[fCount];

                tface.vertex_index[0] = (int) (geo->indices_[k * 3] + vertexStart);
                tface.vertex_index[1] = (int) (geo->indices_[k * 3 + 1] + vertexStart);
                tface.vertex_index[2] = (int) (geo->indices_[k * 3 + 2] + vertexStart);

                if (tface.vertex_index[0] > totalVertices || tface.vertex_index[1] > totalVertices || tface.vertex_index[2] > totalVertices)
                {
                    ATOMIC_LOGERROR("Vertex overflow");
                    return false;
                }

                tface.material_index = j;

            }

        }

        Thekla::Atlas_Options atlasOptions;
        atlas_set_default_options(&atlasOptions);
        atlasOptions.packer_options.witness.texel_area = 32;
        Thekla::Atlas_Error error = Thekla::Atlas_Error_Success;

        tOutputMesh_ = atlas_generate(tInputMesh_, &atlasOptions, &error);

        if (tOutputMesh_)
        {
            WriteLightmapUVCoords();
        }

        delete [] tInputMesh_->vertex_array;
        delete [] tInputMesh_->face_array;
        delete tInputMesh_;
        tInputMesh_ = 0;

        atlas_free(tOutputMesh_);
        tOutputMesh_ = 0;

    }

    // update model
    modelPacker_->Pack();

    return true;

}

}
