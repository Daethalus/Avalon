#pragma once
#include "Avalon/Common.hpp"

namespace Avalon
{
	class IDevice;

	class SceneViewer
	{
	public:
		void Shutdown();
		i32  Run();

	private:
		bool     m_running = true;
		IDevice* m_device = nullptr;
	};
}
