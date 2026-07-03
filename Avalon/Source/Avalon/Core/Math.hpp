#pragma once
#include "Avalon/Common.hpp"

namespace Avalon
{
	struct Rect
	{
		i32 x;
		i32 y;
		u32 width;
		u32 height;
	};

	template<typename T>
	struct TVec4
	{
		union
		{
			union
			{
				T x;
				T r;
			};

			union
			{
				T y;
				T g;
			};

			union
			{
				T z;
				T b;
			};

			union
			{
				T w;
				T a;
			};

			T arr[4] = {};
		};
		constexpr TVec4() = default;
		constexpr TVec4(T _x, T _y, T _z, T _w);
	};

	template <typename T>
	constexpr TVec4<T>::TVec4(T _x, T _y, T _z, T _w)
	{
		arr[0] = _x;
		arr[1] = _y;
		arr[2] = _z;
		arr[3] = _w;
	}


	using Vec4f = TVec4<f32>;
	using Vec4d = TVec4<f64>;
}
