#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>

#include "Avalon/Device/Device.hpp"



namespace Avalon
{


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

		ID3D12Resource* m_resource = nullptr;

		void CreateDefaultView();
	};


	class D3D12Swapchain : public ISwapChain
	{
	public:
		constexpr static u32 BufferCount = 3;

		HRESULT Present() const;

		u32       AcquireNextImage() override;
		ITexture* GetBackBuffer(u32 index) override;

		void Destroy() override;

		SwapchainDesc m_desc;
		IDXGISwapChain4* m_swapChain = nullptr;
		ITexture* m_backBuffers[BufferCount] = {};

	protected:
		~D3D12Swapchain() override = default;

	};
}
