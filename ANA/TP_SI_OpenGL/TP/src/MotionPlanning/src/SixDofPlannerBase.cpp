#include <MotionPlanning/SixDofPlannerBase.h>

namespace MotionPlanning
{
	SixDofPlannerBase::SixDofPlannerBase(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::initializer_list<std::pair<float, float>>& intervals):
		m_intervals(intervals), m_collisionManager(collisionManager), m_object(object)
	{
		assert(m_intervals.size() == 6);
	}

	SixDofPlannerBase::SixDofPlannerBase(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::vector<std::pair<float, float>>& intervals)
	: m_intervals(intervals), m_collisionManager(collisionManager), m_object(object)
	{
		assert(m_intervals.size() == 6);
	}

	SixDofPlannerBase::Configuration SixDofPlannerBase::randomConfiguration() const
	{
		float dofs[6];
		for (size_t cpt = 0; cpt<6; ++cpt)
		{
			dofs[cpt] = static_cast<float>(m_uniformRandom(m_intervals[cpt].first, m_intervals[cpt].second));
		}
		return Configuration{ Math::makeVector(dofs[0], dofs[1], dofs[2]), Math::makeVector(dofs[3], dofs[4], dofs[5]) };
	}

	float SixDofPlannerBase::distanceToObstacles(const Configuration & configuration) const
	{
		m_object.setTranslation(configuration.m_translation);
		m_object.setOrientation(configuration.m_orientation);
		return m_collisionManager->computeDistance();
	}

	bool SixDofPlannerBase::doCollide(const Configuration & configuration) const
	{
		m_object.setTranslation(configuration.m_translation);
		//m_object.setOrientation(toQuaternion(configuration.m_eulerAngles));
		m_object.setOrientation(configuration.m_orientation);
		//m_object.setOrientation(configuration.m_eulerAngles[0], configuration.m_eulerAngles[1], configuration.m_eulerAngles[2]);
		return m_collisionManager->doCollide();
	}

	float SixDofPlannerBase::configurationDistance(const Configuration & c1, const Configuration & c2) 
	{
		return
			std::abs((c1.m_translation - c2.m_translation).norm()) +
			std::acos(std::abs(c1.m_orientation.dot(c2.m_orientation))) / (0.5 * Math::pi);
	}

	SixDofPlannerBase::Configuration SixDofPlannerBase::limitDistance(const Configuration& source, const Configuration& target, float maxDistance, size_t iterationLimit)
	{
		std::pair<float, float> interval = {0.0f, 1.0f};
		if (configurationDistance(source, target) <= maxDistance) { return target; }
		for (size_t cpt = 0; cpt < iterationLimit; ++cpt)
		{
			float middle = (interval.first + interval.second) * 0.5f;
			if (configurationDistance(source, source.interpolate(target, middle)) < maxDistance)
			{
				interval.first = middle;
			}
			else
			{
				interval.second = middle;
			}
		}
		Configuration result = source.interpolate(target, interval.first);
		return result;
	}

	bool SixDofPlannerBase::doCollide(const Configuration & start, const Configuration & end, float dq) const
	{
		if (doCollide(start) || doCollide(end)) return true; // One of the initial points (or both) is colliding
		const float distance = configurationDistance(start, end);
		float progression = dq;
		while(progression < distance) {
			const Configuration inter = start.interpolate(end, progression);
			if (doCollide(inter)) return true; // There is a collision between start and end
			progression += dq;
		}
		return false; // If it survives until here, there is no collisions

	}

	void SixDofPlannerBase::optimize(::std::vector<Configuration>& toOptimize, float dq) const
	{
		std::cout << "start first optim ..." << std::endl;
		std::vector<Configuration> optimized;
		int modif = 1;
		int it = 0;
		while (modif != 0) {
			it++;
			//if (it % 1000 == 0) std::cout << "modif : " << modif << std::endl;
			modif = 0;
			optimized.clear();
			optimized.push_back(toOptimize.at(0));
			for (int i = 1; i < toOptimize.size() - 1; i++)
			{
				if (i % 10 == 0) std::cout << "i : " << i << "/" << toOptimize.size()  - 1 << std::endl;

				Configuration config0 = toOptimize.at(i - 1);
				Configuration config = toOptimize.at(i);
				Configuration config1 = toOptimize.at(i + 1);
				if (doCollide(config0, config1, dq) == true) 
				{
					optimized.push_back(config);
					//std::cout << "collision" << std::endl;
				}
				else
				{ 
					modif++;
					//std::cout << " no collision" << std::endl;

				}
			}
			optimized.push_back(toOptimize.at(toOptimize.size() - 1));
			toOptimize = optimized;
		}

		// second optim
		std::cout << "start second optim ..." << std::endl;
		int max = toOptimize.size() - 2;
		int min = 1;
		for (int k = 0; k < toOptimize.size() * 20; k++) {
			if (k % 1000 == 0) std::cout << "k : " << k << "/" << toOptimize.size() * 20 << std::endl;
			int i = (std::rand() % (max - min)) + min;
			int i0 = i - 1;
			int i1 = i + 1;
			double interval = (double)std::rand() / RAND_MAX;
			const Configuration start = toOptimize.at(i0);
			const Configuration end = toOptimize.at(i);
			const Configuration init = toOptimize.at(i1);
			const Configuration inter = start.interpolate(end, interval);
			if (doCollide(inter, init, dq) == false) {
				toOptimize.at(i) = inter;
			}
		}
	}
}