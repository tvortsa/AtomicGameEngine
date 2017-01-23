// ATOMIC GAME ENGINE - The code in this file was adapted from Lumak's
// Terrain editor shared on the Urho3d forum under the Urho3d MIT licence
// http://discourse.urho3d.io/t/terrain-editor/1769/13

//
// Copyright (c) 2008-2015 the Urho3D project.
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

#pragma once
#include <Atomic/Core/Object.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Resource/Image.h>
#include <Atomic/Graphics/Texture2D.h>

namespace Atomic
{

	class Node;
	class Scene;
}

namespace AtomicEditor 
{

	class ColorMap : public Image
	{
		ATOMIC_OBJECT(ColorMap, Image);

	public:
		ColorMap(Context *_pContext)
			: Image(_pContext)
			, m_pTexture2DSource(nullptr)
		{
		}

		virtual ~ColorMap()
		{
			m_pTexture2DSource = NULL;
		}

		void SetSourceColorMap(Texture2D *_pTexture2DOrigin);
		void ApplyColorMap();

	protected:
		SharedPtr<Texture2D> m_pTexture2DSource;
	};
}