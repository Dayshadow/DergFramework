#pragma once
#include "Framework/Graphics/Text.hpp"
class dffonts {
	TextContext ftctx;
public:
	Font videotype = { "videotype.ttf", ftctx };
};

extern dffonts DefaultFonts;