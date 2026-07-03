#include "D3D12Types.hpp"

#include <algorithm>
#include <iterator>

#include "D3D12Device.hpp"
#include "Avalon/Core/Allocator.hpp"
#include "spdlog/spdlog.h"

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
		return m_view;
	}

	void D3D12Texture::Destroy()
	{
		if (m_view) m_view->Destroy();
		if (m_resource) m_resource->Release();

		DestroyAndFree(this);
	}

	void D3D12Texture::CreateDefaultView()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		D3D12TextureView* textureView = Alloc<D3D12TextureView>();
		textureView->m_device = m_device;
		textureView->m_texture = this;

		textureView->m_srvAlloc = m_device->GetSRVHeap().Allocate();
		m_device->GetDevice()->CreateShaderResourceView(m_resource, &srvDesc, textureView->m_srvAlloc.cpu);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		textureView->m_rtvAlloc = m_device->GetRTVHeap().Allocate();
		m_device->GetDevice()->CreateRenderTargetView(m_resource, &rtvDesc, textureView->m_rtvAlloc.cpu);

		m_view = textureView;
	}

	D3D12DescriptorAllocation D3D12DescriptorHeapAllocator::Allocate()
	{
		if (m_allocated >= m_capacity)
		{
			spdlog::error("D3D12DescriptorHeapAllocator::Allocate > capacity");
			return {};
		}

		uint32_t index = m_allocated++;

		D3D12DescriptorAllocation allocation;
		allocation.index = index;

		allocation.cpu.ptr = m_cpuStart.ptr + static_cast<size_t>(index) * m_incrementSize;

		if (m_heap->GetDesc().Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		{
			allocation.gpu.ptr = m_gpuStart.ptr + static_cast<uint64_t>(index) * m_incrementSize;
		}

		return allocation;
	}

	void D3D12DescriptorHeapAllocator::Destroy()
	{
		if (m_heap) m_heap->Release();
		m_heap = {};
	}

	void D3D12TextureView::Destroy()
	{
		DestroyAndFree(this);
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

	void D3D12Swapchain::UpdateResources()
	{
		for (int i = 0; i < BufferCount; ++i)
		{
			ID3D12Resource* renderTarget = nullptr;
			if (FAILED(m_swapChain->GetBuffer(i,IID_PPV_ARGS(&renderTarget))))
			{
				spdlog::error("[D3D12] - Error on SwapChain GetBuffer");
			}

			D3D12Texture* texture = Alloc<D3D12Texture>();
			texture->m_device = m_device;
			texture->m_resource = renderTarget;
			texture->CreateDefaultView();

			m_backBuffers[i] = texture;
		}
	}

	u32 D3D12Swapchain::AcquireNextImage()
	{
		return m_swapChain->GetCurrentBackBufferIndex();
	}

	ITexture* D3D12Swapchain::GetBackBuffer(u32 index)
	{
		return m_backBuffers[index];
	}

	void D3D12Swapchain::Resize(u32 width, u32 height)
	{
		m_device->WaitIdle();

		for (int i = 0; i < BufferCount; ++i)
		{
			m_backBuffers[i]->Destroy();
			m_backBuffers[i] = {};
		}

		m_swapChain->ResizeBuffers(
			BufferCount,
			width,
			height,
			DXGI_FORMAT_R8G8B8A8_UNORM,	//TODO format
			0);

		UpdateResources();
	}

	void D3D12Swapchain::Destroy()
	{
		if (m_swapChain != nullptr)
		{
			m_swapChain->Release();
		}


		DestroyAndFree(this);
	}

	DeviceResultType D3D12DescriptorHeapAllocator::Init(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t maxDescriptors, bool shaderVisible)
	{
		m_type = type;
		m_capacity = maxDescriptors;
		m_allocated = 0;

		m_incrementSize = device->GetDescriptorHandleIncrementSize(type);

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = type;
		desc.NumDescriptors = maxDescriptors;
		desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap))))
		{
			spdlog::error("Failed to create descriptor heap");
			return DeviceResult::UnknownError;
		}

		m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();

		if (shaderVisible)
		{
			m_gpuStart = m_heap->GetGPUDescriptorHandleForHeapStart();
		}

		return DeviceResult::Success;
	}
}
