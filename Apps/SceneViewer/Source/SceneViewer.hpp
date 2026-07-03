#pragma once
#include "Avalon/Common.hpp"
#include "Avalon/Device/Device.hpp"

namespace Avalon
{
	class ICommandList;
	class ISwapChain;
	class IDevice;

	class SceneViewer
	{
	public:
		void Shutdown();
		i32  Run();

	private:
		bool          m_running = true;
		IDevice*      m_device = nullptr;
		ISwapChain*   m_swapchain = nullptr;
	};
}
