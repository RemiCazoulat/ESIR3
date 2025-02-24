#pragma once

#include <Crowds/Messages.h>
#include <Crowds/Agent.h>
#include <Crowds/Prey.h>

namespace Crowds
{
	class Water : public Agent
	{
	public:
		Water(Simulator * simulator, const Math::Vector2f & position, float radius)
			: Agent(simulator, position, radius)
		{}

		virtual void update(double dt) override
		{
			auto entities = this->perceive<Agent>(this->getRadius() + 1000.0);

			WaterMessage waterMessage{this};

			for (auto& entity : entities)
			{
				this->post(waterMessage, entity.get());
			}

		}
	};
}