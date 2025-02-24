#pragma once

#include <Crowds/Boid.h>

namespace Crowds
{
	class Predator : public Boid
	{
	public:
		using Boid::Boid;

		virtual void update(double dt) override
		{
			Boid::update(dt);
		}
	};
}