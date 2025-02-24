#pragma once

#include <Crowds/Boid.h>

namespace Crowds
{
	class Prey : public Boid
	{
	public:
		using Boid::Boid;

		virtual void killed()
		{
			setPosition(Math::makeVector(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
			setStatus(Status::stopped);
		}

		virtual void update(double dt) override
		{
			Boid::update(dt);
		}
	};
}