#pragma once
#include <MotionPlanning/SixDofPlannerBase.h>
#include <MotionPlanning/SixDofConfigurationTree.h>
namespace MotionPlanning
{
	class RRT: public SixDofPlannerBase {
	public:
		RRT(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object)
			: SixDofPlannerBase(collisionManager, object) {}

		bool plan(const Configuration& start, const Configuration& target, float radius, float dq, std::vector<Configuration>& result) override {
			SixDofConfigurationTree root;
			Configuration current_config(start);
			Configuration next_config;
			SixDofConfigurationTree::Node* next_node;
			root.createNode(current_config); 
			printf("[DEBUG] entering in plan function \n");
			// finding a path by building the tree
			printf("[DEBUG] config dist between start and target : %f, collision : %d\n", configurationDistance(start, target), doCollide(start, target, dq));
			printf("[DEBUG] config dist between current and target : %f, collision : %d\n", configurationDistance(current_config, target), doCollide(current_config, target, dq));

			while (configurationDistance(current_config, target) > radius && doCollide(current_config, target, dq)) {
				//printf("passing in while 1 loop \n");
				Configuration next_config = SixDofPlannerBase::randomConfiguration();
				SixDofConfigurationTree::Node * current_node = root.nearest(next_config);
				current_config = current_node->getConfiguration();
				const float distance = SixDofPlannerBase::configurationDistance(current_config, next_config);

				if (distance > radius) next_config = current_config.interpolate(next_config, radius / distance);
				
				if (SixDofPlannerBase::doCollide(current_config, next_config, dq) == false) {
					next_node = root.createNode(next_config, current_node);
				}
			}
			next_node = root.createNode(target, next_node);
			// building result vector
			while (next_node->getFather() != nullptr) {
				result.push_back(next_node->getConfiguration());
				next_node = next_node->getFather();
			}
			std::vector<Configuration> result_tmp;
			for (auto& config : result) {
				result_tmp.push_back(config);
			}
			result = result_tmp;
			return true;
		}
	};
}