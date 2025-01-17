#pragma once
#include <cstdint>

/**
 * Pixel structure. Can also be used for Colors.
 * TODO: Old, remove all occurrences (maybe only keep ColorToPixel/PixelToColor)
 */
struct pixel {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;

	// pixel() : b(0), g(0), r(0), a(0) {}
	// pixel(uint8_t r, uint8_t g, uint8_t b) : b(b), g(g), r(r), a(UINT8_MAX) {}
	// pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : b(b), g(g), r(r), a(a) {}

	static pixel ColorToPixel(uint32_t color)
	{
		pixel pixel;
		pixel.b = color >> 0 & 0xFF;
		pixel.g = color >> 8 & 0xFF;
		pixel.r = color >> 16 & 0xFF;
		pixel.a = color >> 24 & 0xFF;
		return pixel;
	}

	static uint32_t PixelToColor(pixel pixel)
	{
		return pixel.b << 0 | pixel.g << 8 | pixel.r << 16 | pixel.a;
	}
};
