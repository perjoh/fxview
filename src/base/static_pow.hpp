#pragma once

namespace base
{
	// Compile time integer power of.

	template <unsigned base, unsigned exponent>
	struct static_pow
	{
		static const unsigned result = base*static_pow<base, exponent-1>::result;
	};

	template <unsigned base, 0>
	struct static_pow
	{
		static const unsigned result = 1;
	};
}
