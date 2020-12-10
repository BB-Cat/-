#pragma once
#include "Emitter.h"

#define BUBBLE_DURATION (3.0f)
#define BUBBLE_DURATION_VARIATION (0.2f)
#define BUBBLE_MAX_SIZE (0.5f)
#define BUBBLE_SIZE_VARIATION (0.7f)

class BubbleEmitter : public Emitter
{
public:
	BubbleEmitter(int num_particles, Vec3 pos, float radius);
	// Emitter ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
	virtual void update(float delta) override;
	virtual void spawn() override;
};