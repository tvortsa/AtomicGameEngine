#include <AtomicEditor/Editors/SceneEditor3D/SceneEditor3D.h>
#include <Atomic/Resource/ResourceCache.h>
#include "ColorMap.h"


namespace AtomicEditor
{

	void ColorMap::SetSourceColorMap(Texture2D *_pTexture2DSource)
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		SharedPtr<Image> imageSource;

		m_pTexture2DSource = _pTexture2DSource;
		String name = m_pTexture2DSource->GetName();
		imageSource = cache->GetResource<Image>(name);

		SetSize(imageSource->GetWidth(), imageSource->GetHeight(), imageSource->GetDepth(), imageSource->GetComponents());
		SetData(imageSource->GetData());
	}

	void ColorMap::ApplyColorMap()
	{
		if (m_pTexture2DSource)
		{
			m_pTexture2DSource->SetData(0, 0, 0, GetWidth(), GetHeight(), GetData());
		}
	}

}