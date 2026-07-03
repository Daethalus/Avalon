#include "SceneViewer.hpp"

#include <SDL3/SDL.h>

#include "Avalon/Device/IDevice.hpp"
#include "spdlog/spdlog.h"

constexpr bool enableDebugLayers = true;

namespace Avalon
{
	void SceneViewer::Shutdown()
	{
		m_running = false;
	}

	i32 SceneViewer::Run()
	{
		if (CreateDevice(DeviceType::None,  &m_device) != DeviceResult::Success)
		{
			spdlog::error("Error creating device");
			return 1;
		}

		if (m_device->Init(enableDebugLayers) != DeviceResult::Success)
		{
			spdlog::error("Error initializing device");
			return 1;
		}

		for (IAdapter* adapter : m_device->Adapters())
		{
			spdlog::debug("[SceneViewer] - Available adapter {}", adapter->Name());
		}

		if (m_device->CreateDevice(nullptr) != DeviceResult::Success)
		{
			spdlog::error("Error selecting adapter");
			return 1;
		}

		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			spdlog::error("Error on initialize SDL {} ", SDL_GetError());
			return 1;
		}

		SDL_WindowFlags sdlFlags = 0;
		sdlFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_MAXIMIZED;

		SDL_Window* window = SDL_CreateWindow("Avalon Scene viewer", 1920, 1080, sdlFlags);


		i32 width, height;
		SDL_GetWindowSize(window, &width, &height);

		SDL_PropertiesID props = SDL_GetWindowProperties(window);

		SwapchainDesc swapchainDesc = {};
		swapchainDesc.width = width;
		swapchainDesc.height = height;
#ifdef AV_WIN
		swapchainDesc.nativeWindow = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
#endif

		m_swapchain = m_device->CreateSwapChain(swapchainDesc);
		if (m_swapchain == nullptr)
		{
			return 1;
		}

		RenderPassDesc renderPassDesc = {};
		m_renderPass = m_device->CreateRenderPass(renderPassDesc);

		for (int i = 0; i < AV_FRAMES_IN_FLIGHT; ++i)
		{
			m_commands[i] = m_device->CreateCommandList();
		}

		spdlog::info("Avalon viewer initialized successfully");

		while (m_running)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
					case SDL_EVENT_QUIT:
						Shutdown();
						break;
					default:
						break;
				}
			}

			ICommandList* cmd = m_commands[m_currentFrame];
			cmd->Reset();

			cmd->BeginRenderPass(BeginRenderPassInfo{
				.renderPass = m_renderPass
			});

			ViewportInfo viewportInfo;
			viewportInfo.width = width;
			viewportInfo.height = height;
			cmd->SetViewport(viewportInfo);
			cmd->SetScissor(0, 0, width, height);

			//TODO - draw commands

			cmd->EndRenderPass();

			cmd->Close();


			m_device->SubmitAndPresent(cmd, m_swapchain);

			m_currentFrame = (m_currentFrame + 1) % AV_FRAMES_IN_FLIGHT;
		}

		for (int i = 0; i < AV_FRAMES_IN_FLIGHT; ++i)
		{
			m_commands[i]->Destroy();
		}

		m_swapchain->Destroy();
		m_device->Destroy();

		SDL_DestroyWindow(window);
		SDL_Quit();

		return 0;
	}
}
