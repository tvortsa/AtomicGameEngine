//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Includes work from MIT licensed Urho3d terrain editor by JTippetts
// https://github.com/JTippetts/U3DTerrainEditor
// Copyright (c) 2015 JTippetts
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include <Atomic/IO/Log.h>
#include <Atomic/Core/CoreEvents.h>
#include <AtomicEditor/Editors/SceneEditor3D/SceneEditor3D.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Terrain.h>
#include "TerrainBrush.h"


namespace AtomicEditor
{

	TerrainBrush::TerrainBrush(Context* context, SceneEditor3D *sceneEditor, Terrain* terrain) : Object(context)
	{  
		    ResourceCache* cache = GetSubsystem<ResourceCache>();
	     	brushcursornode_ = sceneEditor->GetScene()->CreateChild();
			brushcursor_ = brushcursornode_->CreateComponent<CustomGeometry>();
			brushcursor_->SetNumGeometries(1);
			brushmat_ = cache->GetResource<Material>("Material", "Materials/TerrainBrush.xml");
			terrain_ = terrain;
	}

	TerrainBrush::~TerrainBrush()
	{
	
	}

	void TerrainBrush::BuildCursorMesh(float radius) {
		if (!brushcursor_)
			return;

		brushcursor_->BeginGeometry(0, TRIANGLE_LIST);
		brushcursor_->SetDynamic(true);
		brushcursor_->SetMaterial(0, brushmat_);

		Vector3 spacing = terrain_->GetSpacing();
		float spacingx = spacing.x_;
		float spacingz = spacing.z_;
		float meshsize = floor(radius) * 2 + 2;
		float originx = (-meshsize)*spacingx*0.5;
		float originz = (-meshsize)*spacingx*0.5;

		float uvspacing = 1 / (meshsize - 1);

		float x, z;

					for (x = 0; x < meshsize - 2; x++) {
						for (z = 0; z < meshsize - 2; z++)
						{
							brushcursor_->DefineVertex(Vector3(originx + x*spacingx, 0, originz + z*spacingz));
							brushcursor_->DefineTexCoord(Vector2(x*uvspacing, z*uvspacing));

							brushcursor_->DefineVertex(Vector3(originx + (x + 1)*spacingx, 0, originz + (z + 1)*spacingz));
							brushcursor_->DefineTexCoord(Vector2((x + 1)*uvspacing, (z + 1)*uvspacing));

							brushcursor_->DefineVertex(Vector3(originx + x*spacingx, 0, originz + (z + 1)*spacingz));
							brushcursor_->DefineTexCoord(Vector2(x*uvspacing, (z + 1)*uvspacing));

							brushcursor_->DefineVertex(Vector3(originx + x*spacingx, 0, originz + z*spacingz));
							brushcursor_->DefineTexCoord(Vector2(x*uvspacing, z*uvspacing));

							brushcursor_->DefineVertex(Vector3(originx + (x + 1)*spacingx, 0, originz + z*spacingz));
							brushcursor_->DefineTexCoord(Vector2((x + 1)*uvspacing, z*uvspacing));

							brushcursor_->DefineVertex(Vector3(originx + (x + 1)*spacingx, 0, originz + (z + 1)*spacingz));
							brushcursor_->DefineTexCoord(Vector2((x + 1)*uvspacing, (z + 1)*uvspacing));
					  }
				   }
					brushcursor_->Commit();
					brushcursor_->SetMaterial(0, brushmat_);
	}


}
