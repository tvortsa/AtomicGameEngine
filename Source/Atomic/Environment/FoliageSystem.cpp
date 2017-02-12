//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
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

#include "Precompiled.h"
#include "../Core/Context.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsEvents.h"
#include "../Graphics/View.h"
#include "../Scene/Scene.h"
#include "../Graphics/Camera.h"
#include "../Scene/SceneEvents.h"
#include "../Graphics/Light.h"
#include "../Resource/ResourceCache.h"
#include "../Graphics/Technique.h"
#include "../Environment/FoliageSystem.h"
#include "../Graphics/Renderer.h"

#if defined(_MSC_VER)
#include "stdint.h"
#endif

#if defined(EMSCRIPTEN) || defined(ATOMIC_PLATFORM_LINUX)
#include <stdint.h>
#endif

namespace Atomic
{

	extern const char* GEOMETRY_CATEGORY;

	FoliageSystem::FoliageSystem(Context *context) : Component(context)
	{


	}

	FoliageSystem::~FoliageSystem()
	{

	}



	void FoliageSystem::RegisterObject(Context* context)
	{
		context->RegisterFactory<FoliageSystem>(GEOMETRY_CATEGORY);

		ATOMIC_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
	}


	void FoliageSystem::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
	{



	}


	void FoliageSystem::Initialize()
	{
		initialized_ = true;

	}

	void FoliageSystem::OnNodeSet(Node* node)
	{

		if (node && node->GetScene())
		{
			SubscribeToEvent(node->GetScene(), E_SCENEUPDATE, ATOMIC_HANDLER(FoliageSystem, HandleSceneUpdate));
			node_ = node;
			terrain_ = node->GetComponent<Terrain>();
			DrawGrass();
			//SubscribeToEvent(E_BEGINVIEWUPDATE, ATOMIC_HANDLER(FoliageSystem, HandleBeginViewUpdate));
		}

	}

	void FoliageSystem::DrawGrass() {
		const unsigned NUM_OBJECTS = 100000;
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		Quaternion rot = node_->GetRotation();
	//	Vector3 rotatedpos = (rot.Inverse() * qp.pos);  //  (rot.Inverse() * qp.pos) + terrainpos;
		for (unsigned i = 0; i < NUM_OBJECTS; ++i)
		{
			PRotScale qp;
			
			qp.pos = Vector3(Random(180.0f) - 90.0f, 0.0f, Random(180.0f) - 90.0f);
			qp.rot = Quaternion(0.0f, Random(360.0f), 0.0f);
			qp.pos.y_ = terrain_->GetHeight(rot * qp.pos) - 0.2f;
			qp.scale = 2.5f + Random(2.0f);
			qpList_.Push(qp);
		}

		Model *pModel = cache->GetResource<Model>("Models/Veg/vegbrush.mdl");
		SharedPtr<Model> cloneModel = pModel->Clone();

		nodeRep_ = node_->CreateChild("Vegrep");
		nodeRep_->SetTemporary(true);
		vegReplicator_ = nodeRep_->CreateComponent<GeomReplicator>();
		vegReplicator_->SetModel(cloneModel);
		vegReplicator_->SetMaterial(cache->GetResource<Material>("Models/Veg/veg-alphamask.xml"));

		Vector3 lightDir(0.6f, -1.0f, 0.8f);
		lightDir = -1.0f * lightDir.Normalized();
		vegReplicator_->Replicate(qpList_, lightDir);

		// specify which verts in the geom to move
		// - for the vegbrush model, the top two vertex indeces are 2 and 3
		PODVector<unsigned> topVerts;
		topVerts.Push(2);
		topVerts.Push(3);

		// specify the number of geoms to update at a time
		unsigned batchCount = 10000;

		// wind velocity (breeze velocity shown)
		Vector3 windVel(0.2f, -0.2f, 0.2f);

		// specify the cycle timer
		float cycleTimer = 0.4f;

		vegReplicator_->ConfigWindVelocity(topVerts, batchCount, windVel, cycleTimer);
		vegReplicator_->WindAnimationEnabled(true);

	}

}
