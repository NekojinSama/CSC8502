#pragma once
#include "Vector4.h"

class Colour
{
public:
	static Vector4 RED;
	static Vector4 GREEN;
	static Vector4 BLUE;
	static Vector4 CRGB;

};

Vector4 Colour::RED(1.0f, 0.0f, 0.0f, 1.0f);
Vector4 Colour::GREEN(0.0f, 1.0f, 0.0f, 1.0f);
Vector4 Colour::BLUE(0.0f, 0.0f, 1.0f, 1.0f);
Vector4 Colour::CRGB(1.0f, 1.0f, 1.0f, 1.0f);