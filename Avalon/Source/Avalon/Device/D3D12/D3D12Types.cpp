#include "D3D12Types.hpp"

#include <algorithm>
#include <iterator>

namespace Avalon
{
	std::string D3D12Adapter::Name() const
	{
		DXGI_ADAPTER_DESC1 desc;
		m_adapter->GetDesc1(&desc);

		auto w = std::wstring(desc.Description);

		std::string str;
		std::transform(w.begin(), w.end(), std::back_inserter(str), [](wchar_t c)
		{
			return static_cast<char>(c);
		});
		return str;
	}
}
