#include "D3D12Device.hpp"

#include "D3D12Types.hpp"
#include "Avalon/Core/Allocator.hpp"
#include "spdlog/spdlog.h"

namespace Avalon
{
	DeviceResultType D3D12Device::Init(bool enableDebugLayers)
	{
		u32 flags = 0;

		if (enableDebugLayers)
		{
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debug))))
			{
				m_debug->EnableDebugLayer();
				m_debug->SetEnableGPUBasedValidation(TRUE);
				flags = DXGI_CREATE_FACTORY_DEBUG;
			}
		}

		if (FAILED(CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_factory))))
		{
			spdlog::error("[D3D12] - Error on create DXGIFactory2");
			return DeviceResult::UnknownError;
		}

		for (u32 i = 0;; ++i)
		{
			IDXGIAdapter4* adapter = nullptr;

			HRESULT hr = m_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));

			if (hr == DXGI_ERROR_NOT_FOUND)
				break;

			m_adapters.emplace_back(Alloc<D3D12Adapter>(adapter));
		}

		if (m_adapters.empty())
		{
			spdlog::error("[D3D12] - No adapters found");
			return DeviceResult::UnknownError;
		}

		return DeviceResult::Success;
	}

	DeviceResultType D3D12Device::CreateDevice(IAdapter* selectedAdapter)
	{
		if (selectedAdapter == nullptr)
		{
			selectedAdapter = m_adapters[0];
		}

		D3D12Adapter* adapter = static_cast<D3D12Adapter*>(selectedAdapter);
		if (FAILED(D3D12CreateDevice(adapter->m_adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device))))
		{
			spdlog::error("[D3D12] - Error on create D3D12Device");
			return DeviceResult::UnknownError;
		}


		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		if (FAILED(m_device->CreateCommandQueue(&queueDesc,IID_PPV_ARGS(&m_queue))))
		{
			spdlog::error("[D3D12] - Error on create D3D12CommandQueue");
			return DeviceResult::UnknownError;
		}

		spdlog::info("[D3D12] - D3D12Device initialized successfully with adapter '{}' ", selectedAdapter->Name());

		return DeviceResult::Success;
	}

	DeviceResultType D3D12Device::Destroy()
	{
		for (IAdapter* adapter : m_adapters)
		{
			DestroyAndFree(static_cast<D3D12Adapter*>(adapter));
		}

		if (m_device != nullptr) m_device->Release();
		if (m_factory != nullptr) m_factory->Release();
		if (m_debug != nullptr) m_debug->Release();

		DestroyAndFree(this);

		return DeviceResult::Success;
	}

	ISwapChain* D3D12Device::CreateSwapChain(const SwapchainDesc& swachainDesc)
	{
		DXGI_SWAP_CHAIN_DESC1 swapDesc = {};

		swapDesc.BufferCount = 3;
		swapDesc.Width = swachainDesc.width;
		swapDesc.Height = swachainDesc.height;
		swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapDesc.SampleDesc.Count = 1;

		IDXGISwapChain1* swapChain = nullptr;
		if (FAILED(m_factory->CreateSwapChainForHwnd(m_queue, static_cast<HWND>(swachainDesc.nativeWindow), &swapDesc, nullptr, nullptr, &swapChain)))
		{
			spdlog::error("[D3D12] - Error on CreateSwapChainForHwnd");
			return nullptr;
		}

		return Alloc<D3D12Swapchain>(static_cast<IDXGISwapChain4*>(swapChain));
	}

	IRenderPass* D3D12Device::CreateRenderPass(const RenderPassDesc& renderPassDesc)
	{
		return nullptr;
	}

	ICommandList* D3D12Device::CreateCommandList()
	{
		ID3D12CommandAllocator* allocator = nullptr;
		ID3D12GraphicsCommandList7* commandList = nullptr;

		if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator))))
		{
			spdlog::error("[D3D12] - Error on CreateCommandList");
			return nullptr;
		}


		if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, nullptr,IID_PPV_ARGS(&commandList))))
		{
			spdlog::error("[D3D12] - Error on CreateCommandList");
			return nullptr;
		}

		commandList->Close();

		return Alloc<D3D12CommandList>(allocator, commandList);

	}

	DeviceResultType D3D12Device::SubmitAndPresent(ICommandList* commandList, ISwapChain* swapChain)
	{
		ID3D12CommandList* lists[] = {static_cast<D3D12CommandList*>(commandList)->commandList};
		m_queue->ExecuteCommandLists(1, lists);


		return DeviceResult::Success;
	}

	std::span<IAdapter*> D3D12Device::Adapters()
	{
		return m_adapters;
	}
}
