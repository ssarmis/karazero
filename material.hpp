#pragma once

#include "bitmap.hpp"

struct Material {
	Bitmap diffuse;
	Bitmap normal;
	Bitmap roughness;
	Bitmap metalic;
	Bitmap ambientOcclusion;
	Bitmap emissive;
};