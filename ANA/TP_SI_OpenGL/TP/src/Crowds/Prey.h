#pragma once
#include <Crowds/Boid.h>
#include <Crowds/Messages.h>
#include <Crowds/Water.h>
#include <AI/Tasks/Task.h>
#include <AI/Tasks/Operators.h>
#include <AI/Tasks/TimeOperators.h>

namespace Crowds
{
	float compute_time(time_t start, time_t end)
	{
		return ((float)((int)end - (int)start)) / CLOCKS_PER_SEC;
	}

	class Prey : public Boid
	{
		float previousAngle = 0;
		std::vector<Water*> waters;
		float thirstiness;
		bool inWater;
		Water* currentWater;
		time_t lastSecond;

		
		double dt;
		float perceptionRadius = 10.0;
		std::vector<std::shared_ptr<Prey>> entities;
		Math::Vector2f defaultSteeringForce = Math::makeVector(0.0f, 0.0f);
		Math::Vector2f thirstySteeringForce = Math::makeVector(0.0f, 0.0f);

		std::shared_ptr<AI::Tasks::Task> defaultTask;
		std::shared_ptr<AI::Tasks::Task> thirstyTask;
		std::shared_ptr <AI::Tasks::Task> preyBehavior;

		float distPreyWater(const Math::Vector2f preyPos, const Math::Vector2f waterPos, const float waterRadius)
		{
			Math::Vector2f dir = preyPos - waterPos;
			float dist = dir.norm() - waterRadius;
			return dist;
		}

		Water* findNearestWaterSource()
		{
			if (waters.empty())
			{
				std::cerr << "There is no water source ! " << std::endl;
				return nullptr;
			}
			Math::Vector2f pos = this->getPosition();
			Water* nearestWater = waters.at(0);
			float currentNearestDistance = distPreyWater(pos, nearestWater->getPosition(), nearestWater->getRadius());
			bool first = true;
			for (auto water : waters)
			{
				if (first)
				{
					first = false;
					continue;
				}

				float distance = distPreyWater(pos, water->getPosition(), water->getRadius());
				if (distance < currentNearestDistance)
				{
					currentNearestDistance = distance;
					nearestWater = water;
				}
			}
			return nearestWater;
		}

		void assignLambdasToTasks()
		{
			defaultTask = AI::Tasks::makeTask([this]()
				{
					std::cout << "in default task. thirstiness = " << thirstiness << std::endl;
					if (this->mayCollide(entities))
					{
						defaultSteeringForce = this->avoidCollisions(entities);
					}

					else
					{
						bool isHeaviest = true;
						float mass = this->getMass();

						// Getting the heaviest sheep in the neighborhood
						float currentHeaviestMass = mass;
						int heaviestEntity = 0;
						int entityIndex = -1;
						for (auto& entity : entities)
						{
							entityIndex++;
							float entityMass = entity->getMass();
							if (entityMass > currentHeaviestMass)
							{
								isHeaviest = false;
								currentHeaviestMass = entityMass;
								heaviestEntity = entityIndex;
							}
						}

						// First case -> is the heaviest : just wander
						if (isHeaviest)
						{
							defaultSteeringForce = Boid::wander(2.0, 3.0, previousAngle, this->dt * 5.0);
						}

						// Second case -> follow heaviest sheep
						else
						{
							defaultSteeringForce = Boid::separation(this->perceptionRadius * 0.5, this->dt) * 0.005;
							defaultSteeringForce += this->arrival(entities.at(entityIndex)->getPosition(), this->dt);
							//followSteeringForce += Boid::alignment(perceptionRadius * 0.5, dt);
						}
					}

					// Computing thirstiness
					time_t currentTime = clock();
					float deltaTime = compute_time(lastSecond, currentTime);
					if (deltaTime > 1)
					{
						thirstiness--;

						lastSecond = currentTime;
					}
					if (thirstiness < 10 && findNearestWaterSource() != nullptr)
					{
						return AI::Tasks::Task::Status::failure;
					}
					return AI::Tasks::Task::Status::running;
				});
			thirstyTask = AI::Tasks::makeTask([this]()
					{
						std::cout << "in thirsty task" << std::endl;

						// If there is no water source selected
						if (currentWater == nullptr)
						{
							currentWater = findNearestWaterSource();
						}

						// If there is no water source in the simulation
						/*
						if (currentWater == nullptr)
						{
							return AI::Tasks::Task::Status::failure;
						}
						*/

						// Computing if prey is in water
						Math::Vector2f pos = this->getPosition();
						for (auto water : waters)
						{
							Math::Vector2f waterPos = water->getPosition();
							float dist = (pos - waterPos).norm();
							if (dist < water->getRadius())
							{
								inWater = true;
								break;
							}
						}

						// Computing thirstiness
						time_t currentTime = clock();
						float deltaTime = compute_time(lastSecond, currentTime);
						if (deltaTime > 1)
						{
							thirstiness--;
							if (inWater)
							{
								thirstiness += 5;
								if (thirstiness > 100)
								{
									thirstiness = 100;
								}
							}

							lastSecond = currentTime;
						}

						// If prey is at full thirst
						if (thirstiness >= 100)
						{
							return AI::Tasks::Task::Status::success;
						}

						// Making an arrival force on the pos of the targeted water source 
						thirstySteeringForce += this->arrival(currentWater->getPosition(), this->dt);
						return AI::Tasks::Task::Status::running;
					});
		}

	public:
		using Boid::Boid;

		virtual void killed()
		{
			setPosition(Math::makeVector(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
			setStatus(Status::stopped);
		}

		void addWater(const WaterMessage & wm)
		{
			waters.push_back(wm.m_water);
		}

		Prey(Simulator* simulator, const Math::Vector2f& position, float radius, float mass, float maxSpeed, float maxForce) 
			: Boid(simulator, position, radius, mass, maxSpeed, maxForce)
		{
			thirstiness = Math::Interval<float>(11.0, 12.0).random();
			inWater = false;
			lastSecond = clock();
			createReceiver<WaterMessage>([this](const WaterMessage& m) { addWater(m); });
			assignLambdasToTasks();
			preyBehavior = AI::Tasks::loop(defaultTask || thirstyTask);
		}

		/// <summary>
		/// Organisation of the update function
		/// Behavior :
		/// - Check if collision. If yes, avoid. STOP
		/// - Check if thirsty, and not in water. If yes, go towards the nearest water source. STOP
		/// - If biggest in neighborhood, just wander. STOP
		/// - If not biggest in neighborhood, follow biggest. STOP
		/// Computes : 
		/// - Compute if prey is thirsty
		/// - Compute if prey is in a water source
		/// - Apply all steering forces
		/// </summary>
		/// <param name="dt"></param>
		virtual void update(double dt) override

		{
			defaultSteeringForce = Math::makeVector(0.0f, 0.0f);
			thirstySteeringForce = Math::makeVector(0.0f, 0.0f);
			this->dt = dt;

			entities = this->perceive<Prey>(perceptionRadius);

			// Code before tasks
			/*
			// PRIO 0 -> avoid collision 
			if (this->mayCollide(entities))
			{
				avoidColSteeringForce = this->avoidCollisions(entities);
			}

			// PRIO 1 -> go drink
			else if (thirstiness < 10 && !inWater)
			{
				// If there is no water source selected
				if (currentWater == nullptr)
				{
					currentWater = findNearestWaterSource();
				}

				// If there is no water source in the simulation
				if (currentWater == nullptr)
				{
					goto prio2;
				}

				// Making an arrival force on the pos of the targeted water source 
				thirstySteeringForce += this->arrival(currentWater->getPosition(), dt);

			}

			// PRIO 2 -> go wander 
			else
			{	
				prio2:
				bool isHeaviest = true;
				float mass = this->getMass();	

				// Getting the heaviest sheep in the neighborhood
				float currentHeaviestMass = mass; 
				int heaviestEntity = 0;
				int entityIndex = -1;
				for (auto& entity : entities)
				{
					entityIndex++;
					float entityMass = entity->getMass();
					if (entityMass > currentHeaviestMass)
					{
						isHeaviest = false;
						currentHeaviestMass = entityMass;
						heaviestEntity = entityIndex;
					}
				}

				// First case -> is the heaviest : just wander
				if (isHeaviest)
				{
					wanderSteeringForce = Boid::wander(2.0, 3.0, previousAngle, dt * 5.0);
				}

				// Second case -> follow heaviest sheep
				else
				{
					followSteeringForce = Boid::separation(perceptionRadius * 0.5, dt) * 0.005;
					followSteeringForce += this->arrival(entities.at(entityIndex)->getPosition(), dt);
					//followSteeringForce += Boid::alignment(perceptionRadius * 0.5, dt);
				}
			}

			// Computing the thirstiness
			time_t currentTime = clock();
			float deltaTime = compute_time(lastSecond, currentTime);
			if (deltaTime > 1)
			{
				thirstiness--;
				if (inWater)
				{
					thirstiness += 5;
					if (thirstiness > 100)
					{
						thirstiness = 100;
					}
				}
				lastSecond = currentTime;
			}
			

			// Computing if is in water
			Math::Vector2f pos = this->getPosition();
			for (auto water : waters)
			{
				Math::Vector2f waterPos = water->getPosition();
				float dist = (pos - waterPos).norm();
				if (dist < water->getRadius())
				{
					inWater = true;
					break;
				}
			}
			*/
			// -- end 
			
			//AI::Tasks::Task::Status status = currentTask->execute();
			
			preyBehavior->execute();

			// Forgeting the water sources
			// std::cout << "nbr of water sources perveived : " << waters.size() << std::endl;
			waters.clear();



			this->addSteeringForce(defaultSteeringForce);
			this->addSteeringForce(thirstySteeringForce);

			Boid::update(dt);

			if (thirstiness <= 0)
			{
				killed();
			}

		}
	};
}