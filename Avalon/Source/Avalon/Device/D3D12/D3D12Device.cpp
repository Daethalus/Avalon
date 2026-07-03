#include "D3D12Device.hpp"

#include "D3D12CommandList.hpp"
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

		for (int i = 0; i < AV_FRAMES_IN_FLIGHT; ++i)
		{
			m_commands[i] = CreateCommandList();
		}

		m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&m_fence));
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

		spdlog::info("[D3D12] - D3D12Device initialized successfully with adapter '{}' ", selectedAdapter->Name());

		return DeviceResult::Success;
	}

	DeviceResultType D3D12Device::Destroy()
	{
		for (IAdapter* adapter : m_adapters)
		{
			DestroyAndFree(static_cast<D3D12Adapter*>(adapter));
		}

		if (m_fence) m_fence->Release();
		if (m_fenceEvent) CloseHandle(m_fenceEvent);
		if (m_device != nullptr) m_device->Release();
		if (m_factory != nullptr) m_factory->Release();
		if (m_debug != nullptr) m_debug->Release();

		DestroyAndFree(this);

		return DeviceResult::Success;
	}

	ISwapChain* D3D12Device::CreateSwapChain(const SwapchainDesc& swachainDesc)
	{
		DXGI_SWAP_CHAIN_DESC1 swapDesc = {};

		swapDesc.BufferCount = D3D12Swapchain::BufferCount;
		swapDesc.Width = swachainDesc.width;
		swapDesc.Height = swachainDesc.height;
		swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapDesc.SampleDesc.Count = 1;

		IDXGISwapChain1* d3d12Swapchain = nullptr;
		if (FAILED(m_factory->CreateSwapChainForHwnd(m_queue, static_cast<HWND>(swachainDesc.nativeWindow), &swapDesc, nullptr, nullptr, &d3d12Swapchain)))
		{
			spdlog::error("[D3D12] - Error on CreateSwapChainForHwnd");
			return nullptr;
		}

		D3D12Swapchain* swapChain = Alloc<D3D12Swapchain>();
		swapChain->m_desc = swachainDesc;
		swapChain->m_swapChain = static_cast<IDXGISwapChain4*>(d3d12Swapchain);

		for (int i = 0; i < D3D12Swapchain::BufferCount; ++i)
		{
			ID3D12Resource* renderTarget = nullptr;
			if (FAILED(swapChain->m_swapChain->GetBuffer(i,IID_PPV_ARGS(&renderTarget))))
			{
				spdlog::error("[D3D12] - Error on SwapChain GetBuffer");
				return nullptr;
			}

			D3D12Texture* texture = Alloc<D3D12Texture>();
			texture->m_resource = renderTarget;
			texture->CreateDefaultView();

			swapChain->m_backBuffers[i] = texture;
		}
		return swapChain;
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

	std::span<IAdapter*> D3D12Device::Adapters()
	{
		return m_adapters;
	}

	void D3D12Device::WaitIdle()
	{
		u64 value = m_nextFenceValue++;
		m_queue->Signal(m_fence, value);

		if (m_fence->GetCompletedValue() < value)
		{
			m_fence->SetEventOnCompletion(value, m_fenceValue);
			WaitForSingleObject(m_fenceValue, INFINITE);
		}
	}

	ICommandList* D3D12Device::BeginFrame()
	{
		if (m_fence->GetCompletedValue() < m_fenceValue[m_frameIndex])
		{
			m_fence->SetEventOnCompletion(m_fenceValue[m_frameIndex], m_fenceEvent);
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}

		return m_commands[m_frameIndex];
	}

	DeviceResultType D3D12Device::EndFrame(std::span<ISwapChain*> swapchains)
	{
		ICommandList* commandList = m_commands[m_frameIndex];

		ID3D12CommandList* lists[] = {static_cast<D3D12CommandList*>(commandList)->commandList};
		m_queue->ExecuteCommandLists(1, lists);

		u64 signalValue = m_nextFenceValue++;
		m_queue->Signal(m_fence, signalValue);
		m_fenceValue[m_frameIndex] = signalValue;

		for (ISwapChain* swapchain : swapchains)
		{
			if (FAILED(static_cast<D3D12Swapchain*>(swapchain)->Present()))
			{
				spdlog::error("[D3D12] - Error on SubmitAndPresent - failed to Present");
				return DeviceResult::PresentFailed;
			}
		}

		m_frameIndex = (m_frameIndex + 1) % AV_FRAMES_IN_FLIGHT;

		return DeviceResult::Success;
	}
}
