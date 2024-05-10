#pragma once

UENUM()
enum class EWeatherTypes
{
	// The default weather without any special effects
	Sunny,

	// Weather types that need snow on the landscape should use this
	Snowy,

	// Weather types that need rain on the landscape should use this
	Rainy
};