#pragma once
#include "Avalon/Device/Device.hpp"

#include <dxgi1_6.h>
#include <d3d12.h>
#include <vector>

namespace Avalon
{
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

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cacheHandles;
		std::vector<ID3D12Resource*> resources;
	};
}
