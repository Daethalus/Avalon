#include "SceneViewer.hpp"

#include <SDL3/SDL.h>

#include "Avalon/Device/IDevice.hpp"
#include "spdlog/spdlog.h"

namespace Avalon
{
	void SceneViewer::Shutdown()
	{
		m_running = false;
	}

	i32 SceneViewer::Run()
	{
		if (CreateDevice(DeviceType::Vulkan, &m_device) != DeviceResult::Success)
		{
			spdlog::error("Error creating device");
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
		}

		SDL_DestroyWindow(window);
		SDL_Quit();

		return 0;
	}
}
