#include "D3D12CommandList.hpp"

#include "D3D12Types.hpp"
#include "Avalon/Core/Allocator.hpp"

namespace Avalon
{
	void D3D12CommandList::Reset()
	{
		commandAllocator->Reset();
		commandList->Reset(commandAllocator, nullptr);
	}

	void D3D12CommandList::Close()
	{
		commandList->Close();
	}

	void D3D12CommandList::BeginRenderPass(const BeginRenderPassInfo& info)
	{
		cacheHandles.reserve(info.colors.size());
		resources.reserve(info.colors.size());

		for (int i = 0; i < info.colors.size(); ++i)
		{
			D3D12TextureView* view = static_cast<D3D12TextureView*>(info.colors[i].view);
			D3D12Texture* texture = static_cast<D3D12Texture*>(view->m_texture);

			cacheHandles.emplace_back(view->m_rtvAlloc.cpu);
			resources.emplace_back(texture->m_resource);

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = texture->m_resource;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			commandList->ResourceBarrier(1, &barrier);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle = nullptr;
		commandList->OMSetRenderTargets(cacheHandles.size(), cacheHandles.data(), FALSE, dsvHandle);

		for (int i = 0; i < info.colors.size(); ++i)
		{
			D3D12TextureView* view = static_cast<D3D12TextureView*>(info.colors[i].view);
			commandList->ClearRenderTargetView(view->m_rtvAlloc.cpu, info.colors[i].clearColor.arr, 0, nullptr);
		}

		// commandList->ClearDepthStencilView(
		// 		dsvHandle,
		// 		D3D12_CLEAR_FLAG_DEPTH,
		// 		1.0f,
		// 		0,
		// 		0,
		// 		nullptr);
	}

	void D3D12CommandList::EndRenderPass()
	{
		for (int i = 0; i < resources.size(); ++i)
		{
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = resources[i];
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET ;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			commandList->ResourceBarrier(1, &barrier);
		}

		cacheHandles.clear();
		resources.clear();
	}

	void D3D12CommandList::SetViewport(const ViewportInfo& viewportInfo)
	{
		D3D12_VIEWPORT viewport = {};
		viewport.TopLeftX = viewportInfo.x;
		viewport.TopLeftY = viewportInfo.y;
		viewport.Width = (float)viewportInfo.width;
		viewport.Height = (float)viewportInfo.height;
		viewport.MinDepth = viewportInfo.minDepth;
		viewport.MaxDepth = viewportInfo.maxDepth;

		commandList->RSSetViewports(1, &viewport);
	}

	void D3D12CommandList::SetScissor(i32 x, i32 y, u32 width, u32 height)
	{
		D3D12_RECT scissor =
		{
			x,
			y,
			(LONG)width,
			(LONG)height
		};
		commandList->RSSetScissorRects(1, &scissor);
	}

	void D3D12CommandList::Destroy()
	{
		if (commandAllocator != nullptr) commandAllocator->Release();
		if (commandList != nullptr) commandList->Release();

		DestroyAndFree(this);
	}
}
