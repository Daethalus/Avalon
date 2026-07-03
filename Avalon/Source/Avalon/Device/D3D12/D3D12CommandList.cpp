#include "D3D12CommandList.hpp"

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
		// commandList->OMSetRenderTargets(
		// 		1,
		// 		&rtv,
		// 		FALSE,
		// 		&dsvHandle);

		// 	float clearColor[] =
		// 	{
		// 		0.1f,
		// 		0.2f,
		// 		0.3f,
		// 		1.0f
		// };
		//
		// 	commandList->ClearRenderTargetView(
		// 			rtv,
		// 			clearColor,
		// 			0,
		// 			nullptr);

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
		//TODO
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
