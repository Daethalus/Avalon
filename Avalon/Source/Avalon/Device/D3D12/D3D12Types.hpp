#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>

#include "Avalon/Device/IDevice.hpp"

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


	class D3D12Swapchain : public ISwapChain
	{
	public:
		D3D12Swapchain(IDXGISwapChain4* m_swapchain) : m_swapchain(m_swapchain) {}

		void Destroy() override;
	protected:
		~D3D12Swapchain() override = default;

	private:
		IDXGISwapChain4* m_swapchain = nullptr;
	};


	class D3D12CommandList : public ICommandList
	{
	public:
		D3D12CommandList(ID3D12CommandAllocator* commandAllocator, ID3D12GraphicsCommandList7* commandList) : commandAllocator(commandAllocator), commandList(commandList) {}

		void Reset() override;
		void Close() override;

		void BeginRenderPass(const BeginRenderPassInfo& info) override;
		void EndRenderPass() override;

		void SetViewport(const ViewportInfo& viewportInfo) override;
		void SetScissor(i32 x, i32 y, u32 width, u32 height) override;

		void Destroy() override;

		ID3D12CommandAllocator*     commandAllocator = nullptr;
		ID3D12GraphicsCommandList7* commandList = nullptr;

	protected:

		~D3D12CommandList() override = default;
	};
}
