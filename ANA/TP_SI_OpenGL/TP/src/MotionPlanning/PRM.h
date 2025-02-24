#pragma once
#include <MotionPlanning/SixDofPlannerBase.h>
#include <MotionPlanning/SixDofConfigurationGraph.h>


namespace MotionPlanning
{
	class PRM : public SixDofPlannerBase {
		SixDofConfigurationGraph * graph;
		size_t k = 0;

	public:
		PRM(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object)
			: SixDofPlannerBase(collisionManager, object)
		{
			graph = new SixDofConfigurationGraph();
		}

		void grow(size_t nbNodes, size_t k, float dq, size_t maxSamples) 
		{
			std::cout << "Starting growing ..." << std::endl;
			this->k = k;
			size_t currentNodes = 0;
			for (int i = 0; i < maxSamples; i++)
			{
				Configuration new_config = SixDofPlannerBase::randomConfiguration();

				if (doCollide(new_config)) continue;

				auto neighbours = graph->kNearestNeighbours(new_config, k);

				SixDofConfigurationGraph::Node* new_node = graph->add(new_config);

				currentNodes++;

				for (auto neighbour : neighbours) 
				{
					if (SixDofPlannerBase::doCollide(neighbour->getConfiguration(), new_node->getConfiguration(), dq)) continue;

					graph->addBidirectionalEdge(new_node, neighbour);
				}

				if(i % 1000 == 0) std::cout << " > " << currentNodes << "/" << nbNodes << std::endl;
				
				if (currentNodes > nbNodes) break;
			}
			std::cout << "Growing done !" << std::endl;
		};

		float heuristic_dist(const SixDofConfigurationGraph::Node* start, const SixDofConfigurationGraph::Node* target) {
			return configurationDistance(start->getConfiguration(), target->getConfiguration());
		}

		bool plan(const Configuration& start, const Configuration& target, float radius, float dq, std::vector<Configuration>& result) override
		{
			result.clear();
			bool path_exists = false;

			// Nodes in graph, that are the nearest of start and target config
			SixDofConfigurationGraph::Node* source; bool source_exists = false;
			SixDofConfigurationGraph::Node* dest; bool dest_exists = false;

			// Getting the neighbours of start and target config
			std::vector<SixDofConfigurationGraph::Node*> startNeighbours = graph->kNearestNeighbours(start, k);
			std::vector<SixDofConfigurationGraph::Node*> targetNeighbours = graph->kNearestNeighbours(target, k);

			// Verifying if the knearest are not colliding with the start config. If not, return false
			for (SixDofConfigurationGraph::Node* neighbour : startNeighbours)
			{
				if (!SixDofPlannerBase::doCollide(neighbour->getConfiguration(), start, dq))
				{
					source = graph->add(start);
					graph->addBidirectionalEdge(source, neighbour);
					//source = neighbour;
					source_exists = true;
					break;
				}
			}
			if (!source_exists)
			{
				std::cout << "Source doesn't exists (start doesn't can't connect to one of his " << k <<" nearest neighbours of the graph) !" << std::endl;
				return false;
			}
			else
			{
				std::cout << "Source exists !" << std::endl;
			}
			// verifying if the knearest are not colliding with the target config, If not, return false
			for (SixDofConfigurationGraph::Node* neighbour : targetNeighbours) 
			{
				if (!SixDofPlannerBase::doCollide(neighbour->getConfiguration(), target, dq))
				{
					dest = graph->add(target);
					graph->addBidirectionalEdge(dest, neighbour);
					//dest = neighbour;
					dest_exists = true;
					break;
				}
			}
			if (!dest_exists) 
			{
				std::cout << "Dest doesn't exists (target doesn't can't connect to one of his " << k << " nearest neighbours of the graph) !" << std::endl;
				return false;
			}
			else
			{
				std::cout << "Dest exists !" << std::endl;
			}

			// Checking if source and dest are in the same connex component. If not, return false
			auto source_connex_comp = graph->getConnectedComponent(source);
			auto dest_connex_comp = graph->getConnectedComponent(dest);
			
			if (source_connex_comp != dest_connex_comp)
			{
				while (source_connex_comp != dest_connex_comp)
				{
					grow(5000, 6, dq, 1000000);
					source_connex_comp = graph->getConnectedComponent(source);
					dest_connex_comp = graph->getConnectedComponent(dest);
				}
			}
			else std::cout << "Source and dest are in the same connex composant !" << std::endl;

			// ------------------
			//    A* algorithm 
			// ------------------

			// Init Q

			// Lambda to compare nodes in Q, to sort them automatically
			auto comp = [](std::pair<float, SixDofConfigurationGraph::Node*> a,
				std::pair<float, SixDofConfigurationGraph::Node*> b)
				{ return a.first > b.first; };

			// Init Q with lambda comp 
			std::priority_queue<std::pair<float, SixDofConfigurationGraph::Node*>,
				std::vector<std::pair<float, SixDofConfigurationGraph::Node*>>,
				decltype(comp)> Q(comp);

			// Init cost and pre
			std::unordered_map<SixDofConfigurationGraph::Node*, float> cost;
			std::unordered_map<SixDofConfigurationGraph::Node*, SixDofConfigurationGraph::Node*> pre;

			// Pushing first element in Q
			Q.push(std::make_pair(heuristic_dist(source, dest), source));
			cost[source] = 0;

			// Starting loop 
			int it = 0;
			while (!Q.empty())
			{
				it ++;
				if(it % 1000 == 0) std::cout << "Q size : " << Q.size() << std::endl;

				std::pair<float, SixDofConfigurationGraph::Node*> c = Q.top();
				Q.pop();

				// Checking if c is the dest node
				if (c.second == dest)
				{
					std::cout << "in A* : path exists !" << std::endl;
					path_exists = true;
					break;
				}

				// Else, we iterate on all the neighbours of c
				std::vector<SixDofConfigurationGraph::Node::Transition> S = c.second->getOutgoingTransitions();
				//std::cout << " >>> S  size : " << S.size() << std::endl;
				for (SixDofConfigurationGraph::Node::Transition transition : S)
				{
					SixDofConfigurationGraph::Node* S_node = transition.m_extremity;

					float new_cost = cost[c.second] + transition.m_distance;

					// Verifying if passing by current neighbour is better
					bool end = cost.find(S_node) == cost.end();
					bool inf = new_cost < cost[S_node];
					bool total = end || inf;
					//std::cout << "cost.find(S_node) != cost.end() = " << end << std::endl;
					//std::cout << "new_cost < cost[S_node] = " << inf << std::endl;
					//std::cout << "end || inf = " << total << std::endl;
					
					if ( total)
					{
						//std::cout << " ------ Adding a node in Q" << std::endl;

						Q.push(std::make_pair(new_cost + heuristic_dist(S_node, dest), S_node));
						cost[S_node] = new_cost;
						pre[S_node] = c.second;
					}
				}
			}
			std::cout << "A* has ended. Path exists : " << path_exists << std::endl;

			// Filling result
			if (path_exists)
			{
				//result.push_back(target); // Adding target configuration

				SixDofConfigurationGraph::Node* current = dest;
				
				// Adding all configurations found thanks to growing and A*
				while (current != nullptr)
				{
					result.push_back(current->getConfiguration());
					current = pre[current];  // Get parent
				}

				// Adding start configuration
				//result.push_back(start);

				// Swaping path to get path from source to dest
				std::reverse(result.begin(), result.end());
			}

			// Returning final value
			return path_exists;
		}
	};
}