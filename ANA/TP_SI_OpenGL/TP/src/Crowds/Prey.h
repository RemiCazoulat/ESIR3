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
		// ----- Attributes -----
		// > Water attributes
		std::vector<Water*> m_waters;
		float m_thirstiness;
		bool m_inWater;
		Water* m_currentWater;
		time_t m_lastSecond;

		// > Movement attributes
		double m_dt;
		float m_previousAngle = 0;
		float m_perceptionRadius = 10.0;
		std::vector<std::shared_ptr<Prey>> m_entities;
		Math::Vector2f m_defaultSteeringForce = Math::makeVector(0.0f, 0.0f);
		Math::Vector2f m_thirstySteeringForce = Math::makeVector(0.0f, 0.0f);

		// > Tasks
		std::shared_ptr<AI::Tasks::Task> m_defaultTask;
		std::shared_ptr<AI::Tasks::Task> m_thirstyTask;
		std::shared_ptr<AI::Tasks::Task> m_preyBehavior;

		float distPreyWater(const Math::Vector2f preyPos, const Math::Vector2f waterPos, const float waterRadius)
		{
			Math::Vector2f dir = preyPos - waterPos;
			float dist = dir.norm() - waterRadius;
			return dist;
		}

		Water* findNearestWaterSource()
		{
			if (m_waters.empty())
			{
				std::cerr << "There is no water source ! " << std::endl;
				return nullptr;
			}
			Math::Vector2f pos = this->getPosition();
			Water* nearestWater = m_waters.at(0);
			float nearestDistance = distPreyWater(pos, nearestWater->getPosition(), nearestWater->getRadius());
			bool first = true;
			for (auto water : m_waters)
			{
				if (first)
				{
					first = false;
					continue;
				}

				float distance = distPreyWater(pos, water->getPosition(), water->getRadius());
				if (distance < nearestDistance)
				{
					nearestDistance = distance;
					nearestWater = water;
				}
			}
			return nearestWater;
		}

		void assignLambdasToTasks()
		{
			m_defaultTask = AI::Tasks::makeTask([this]()
				{
					std::cout << "|=====[ Default task ]=====|" << std::endl;
					std::cout << "> thirstiness = " << this->m_thirstiness << std::endl;

					if (this->mayCollide(this->m_entities))
					{
						this->m_defaultSteeringForce = this->avoidCollisions(m_entities);
					}

					else
					{
						bool isHeaviest = true;
						float mass = this->getMass();

						// Getting the heaviest sheep in the neighborhood
						float currentHeaviestMass = mass;
						int heaviestEntity = 0;
						int entityIndex = -1;
						for (auto& entity : m_entities)
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
							this->m_defaultSteeringForce = Boid::wander(2.0, 3.0, m_previousAngle, this->m_dt * 5.0);
						}

						// Second case -> follow heaviest sheep
						else
						{
							this->m_defaultSteeringForce = Boid::separation(this->m_perceptionRadius * 0.5, this->m_dt) * 0.005;
							this->m_defaultSteeringForce += this->arrival(m_entities.at(entityIndex)->getPosition(), this->m_dt);
							//followSteeringForce += Boid::alignment(m_perceptionRadius * 0.5, m_dt);
						}
					}

					// Computing m_thirstiness
					time_t currentTime = clock();
					float deltaTime = compute_time(this->m_lastSecond, currentTime);
					if (deltaTime > 1)
					{
						this->m_thirstiness--;

						this->m_lastSecond = currentTime;
					}
					if (this->m_thirstiness < 20 && findNearestWaterSource() != nullptr)
					{
						this->m_currentWater = findNearestWaterSource();
						return AI::Tasks::Task::Status::failure;
					}
					return AI::Tasks::Task::Status::running;
				});
			m_thirstyTask = AI::Tasks::makeTask([this]()
					{
						std::cout << "|=====[ Thirsty task ]=====|" << std::endl;

						std::cout << "> thirstiness = " << this->m_thirstiness << std::endl;

						// If there is no water source selected
						if (this->m_currentWater == nullptr)
						{
							this->m_currentWater = findNearestWaterSource();
							//std::cout << "> After find nearest water source" << std::endl;
						}

						// If there is no water source in the simulation
						
						if (this->m_currentWater == nullptr)
						{
							return AI::Tasks::Task::Status::failure;
							std::cout << "> There is no water sources ! (in lambda)" << std::endl;

						}
						
						// Computing if prey is in water
						this->m_inWater = false; /* Reset each time the bool -> without this, problem was :
												    Prey thirsty -> go in a water source
													Prey arrives in a water source -> m_inWater = true
													Prey is not thirsty anymore -> m_inWater stays true !
													Prey is thirsty again -> m_inWater is true, even if prey is not in a water source */
						Math::Vector2f pos = this->getPosition();
						for (auto water : this->m_waters)
						{
							Math::Vector2f waterPos = water->getPosition();
							float dist = (pos - waterPos).norm();
							if (dist < water->getRadius())
							{
								this->m_inWater = true;
								break;
							}
						}
						//std::cout << "> Computing if prey is in water" << std::endl;


						// Computing m_thirstiness
						time_t currentTime = clock();
						float deltaTime = compute_time(this->m_lastSecond, currentTime);
						if (deltaTime > 1)
						{
							this->m_thirstiness--;
							if (this->m_inWater)
							{
								this->m_thirstiness += 5;
								if (this->m_thirstiness > 100)
								{
									this->m_thirstiness = 100;
								}
							}

							this->m_lastSecond = currentTime;
						}
						//std::cout << "> Computing thirstiness" << std::endl;


						// If prey is at full thirst
						if (this->m_thirstiness >= 100)
						{
							this->m_currentWater = nullptr;
							return AI::Tasks::Task::Status::success;
						}

						// Making an arrival force on the pos of the targeted water source 
						this->m_thirstySteeringForce += this->arrival(m_currentWater->getPosition(), this->m_dt);
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
			m_waters.push_back(wm.m_water);
		}

		Prey(Simulator* simulator, const Math::Vector2f& position, float radius, float mass, float maxSpeed, float maxForce) 
			: Boid(simulator, position, radius, mass, maxSpeed, maxForce)
		{
			m_thirstiness = Math::Interval<float>(22.0, 23.0).random();
			m_inWater = false;
			m_lastSecond = clock();
			createReceiver<WaterMessage>([this](const WaterMessage& m) { addWater(m); });
			assignLambdasToTasks();
			m_preyBehavior = AI::Tasks::loop(m_defaultTask || m_thirstyTask);
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
		/// <param name="m_dt"></param>
		virtual void update(double dt) override

		{
			m_defaultSteeringForce = Math::makeVector(0.0f, 0.0f);
			m_thirstySteeringForce = Math::makeVector(0.0f, 0.0f);
			this->m_dt = dt;

			m_entities = this->perceive<Prey>(m_perceptionRadius);

			// Code before tasks
			/*
			// PRIO 0 -> avoid collision 
			if (this->mayCollide(m_entities))
			{
				avoidColSteeringForce = this->avoidCollisions(m_entities);
			}

			// PRIO 1 -> go drink
			else if (m_thirstiness < 10 && !m_inWater)
			{
				// If there is no water source selected
				if (m_currentWater == nullptr)
				{
					m_currentWater = findNearestWaterSource();
				}

				// If there is no water source in the simulation
				if (m_currentWater == nullptr)
				{
					goto prio2;
				}

				// Making an arrival force on the pos of the targeted water source 
				m_thirstySteeringForce += this->arrival(m_currentWater->getPosition(), m_dt);

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
				for (auto& entity : m_entities)
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
					wanderSteeringForce = Boid::wander(2.0, 3.0, m_previousAngle, m_dt * 5.0);
				}

				// Second case -> follow heaviest sheep
				else
				{
					followSteeringForce = Boid::separation(m_perceptionRadius * 0.5, m_dt) * 0.005;
					followSteeringForce += this->arrival(m_entities.at(entityIndex)->getPosition(), m_dt);
					//followSteeringForce += Boid::alignment(m_perceptionRadius * 0.5, m_dt);
				}
			}

			// Computing the m_thirstiness
			time_t currentTime = clock();
			float deltaTime = compute_time(m_lastSecond, currentTime);
			if (deltaTime > 1)
			{
				m_thirstiness--;
				if (m_inWater)
				{
					m_thirstiness += 5;
					if (m_thirstiness > 100)
					{
						m_thirstiness = 100;
					}
				}
				m_lastSecond = currentTime;
			}
			

			// Computing if is in water
			Math::Vector2f pos = this->getPosition();
			for (auto water : m_waters)
			{
				Math::Vector2f waterPos = water->getPosition();
				float dist = (pos - waterPos).norm();
				if (dist < water->getRadius())
				{
					m_inWater = true;
					break;
				}
			}
			*/
			// -- end 
			
			//AI::Tasks::Task::Status status = currentTask->execute();
			
			m_preyBehavior->execute();

			// Forgeting the water sources
			// std::cout << "nbr of water sources perveived : " << m_waters.size() << std::endl;
			m_waters.clear();



			this->addSteeringForce(m_defaultSteeringForce);
			this->addSteeringForce(m_thirstySteeringForce);

			Boid::update(dt);

			if (m_thirstiness <= 0)
			{
				killed();
			}

		}
	};
}