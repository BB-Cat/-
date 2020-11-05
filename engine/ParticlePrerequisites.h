#pragma once
#include <memory>

class Particle;
class SP_Bubble;


enum particle_types
{
	Bubble,
};

//array of file locations for sprite textures !! needs to line up to the particle type enumeration to load properly
const wchar_t* particle_sprites =
{
	L"..\\Assets\\Textures\\bubble.png",
};