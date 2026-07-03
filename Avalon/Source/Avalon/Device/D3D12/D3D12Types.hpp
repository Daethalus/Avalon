#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>

#include "Avalon/Device/Device.hpp"

namespace Avalon
{
	class D3D12Device;

	struct D3D12DescriptorAllocation
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpu{};
		D3D12_GPU_DESCRIPTOR_HANDLE gpu{};
		uint32_t index = UINT32_MAX;
	};

	class D3D12DescriptorHeapAllocator
	{
	public:
		DeviceResultType Init(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t maxDescriptors, bool shaderVisible);
		D3D12DescriptorAllocation Allocate();

		void Destroy();

	private:
		ID3D12DescriptorHeap* m_heap = nullptr;

		D3D12_DESCRIPTOR_HEAP_TYPE m_type{};
		uint32_t m_capacity = 0;
		uint32_t m_allocated = 0;
		uint32_t m_incrementSize = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart{};
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart{};
	};

	class D3D12Adapter : public IAdapter
	{
	public:
		D3D12Adapter(IDXGIAdapter4* adapter) : m_adapter(adapter) {}

		std::string Name() const override;

		friend class D3D12Device;

	protected:
		~D3D12Adapter() override = default;

	private:
		IDXGIAdapter4* m_adapter = nullptr;
	};

	class D3D12Texture : public ITexture
	{
	public:
		ITextureView* GetView() override;
		void          Destroy() override;

		D3D12Device*    m_device = nullptr;
		ID3D12Resource* m_resource = nullptr;
		ITextureView*   m_view = nullptr;

		void CreateDefaultView();
	};

	class D3D12TextureView : public ITextureView
	{
	public:
		void Destroy() override;

		D3D12Device*              m_device = nullptr;
		ITexture*                 m_texture = nullptr;
		D3D12DescriptorAllocation m_srvAlloc = {};
		D3D12DescriptorAllocation m_rtvAlloc = {};
	};


	class D3D12Swapchain : public ISwapChain
	{
	public:
		constexpr static u32 BufferCount = 3;

		HRESULT Present() const;
		void UpdateResources();

		u32       AcquireNextImage() override;
		ITexture* GetBackBuffer(u32 index) override;
		void      Resize(u32 width, u32 height) override;

		void Destroy() override;

		D3D12Device* m_device = nullptr;
		SwapchainDesc m_desc;
		IDXGISwapChain4* m_swapChain = nullptr;
		ITexture* m_backBuffers[BufferCount] = {};

	protected:
		~D3D12Swapchain() override = default;

	};
}
