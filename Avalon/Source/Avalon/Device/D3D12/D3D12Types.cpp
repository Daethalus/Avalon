#include "D3D12Types.hpp"

#include <algorithm>
#include <iterator>

#include "Avalon/Core/Allocator.hpp"

namespace Avalon
{
	std::string D3D12Adapter::Name() const
	{
		DXGI_ADAPTER_DESC1 desc;
		m_adapter->GetDesc1(&desc);

		auto w = std::wstring(desc.Description);

		std::string str;
		std::transform(w.begin(), w.end(), std::back_inserter(str), [](wchar_t c)
		{
			return static_cast<char>(c);
		});
		return str;
	}

	ITextureView* D3D12Texture::GetView()
	{
		return nullptr;
	}

	void D3D12Texture::Destroy()
	{
		DestroyAndFree(this);
	}

	void D3D12Texture::CreateDefaultView()
	{
		//TODO
	}


	HRESULT D3D12Swapchain::Present() const
	{
		m_swapChain->Present(1, 0);
		u32 flags = 0;
		if (!m_desc.vsync && m_desc.allowTearing)
		{
			flags = DXGI_PRESENT_ALLOW_TEARING;
		}
		return m_swapChain->Present(m_desc.vsync ? 1 : 0, flags);
	}

	u32 D3D12Swapchain::AcquireNextImage()
	{
		return m_swapChain->GetCurrentBackBufferIndex();
	}

	ITexture* D3D12Swapchain::GetBackBuffer(u32 index)
	{
		return m_backBuffers[index];
	}

	void D3D12Swapchain::Destroy()
	{
		if (m_swapChain != nullptr)
		{
			m_swapChain->Release();
		}

		DestroyAndFree(this);
	}
}
