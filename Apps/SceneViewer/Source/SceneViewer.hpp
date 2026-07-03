#pragma once
#include "Avalon/Common.hpp"
#include "Avalon/Device/IDevice.hpp"

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
		IRenderPass*  m_renderPass = nullptr;
		ICommandList* m_commands[AV_FRAMES_IN_FLIGHT] = {};
		u32           m_currentFrame = 0;
	};
}
