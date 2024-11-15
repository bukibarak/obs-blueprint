#pragma once
#include <cstdint>

/**
 * Pixel structure. Can also be used for Colors.
 */
struct pixel {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	pixel() : r(0), g(0), b(0), a(0) {}
	pixel(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(UINT8_MAX) {}
	pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

	static pixel ColorToPixel(uint32_t color)
	{
		pixel pixel;
		pixel.r = color >> 0 & 0xFF;
		pixel.g = color >> 8 & 0xFF;
		pixel.b = color >> 16 & 0xFF;
		pixel.a = color >> 24 & 0xFF;
		return pixel;
	}

	static uint32_t PixelToColor(pixel pixel)
	{
		return pixel.r << 0 | pixel.g << 8 | pixel.b << 16 | pixel.a;
	}
};
