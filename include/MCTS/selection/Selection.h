#pragma once

#include <assert.h>
#include "MCTS/Config.h"
#include "MCTS/selection/TreeNode.h"
#include "MCTS/selection/TraversedNodeInfo.h"

namespace mcts
{
	namespace selection
	{
		class Selection
		{
		public:
			void StartNewAction() { saved_path_ = path_; }
			void RestartAction() { path_ = saved_path_; }

			void StartMainAction(TreeNode * root)
			{
				path_.clear();
				StepNext(-1, root);
				new_node_created_ = false;
			}

			// @return >= 0 for the chosen action; < 0 if no valid action
			int GetAction(
				board::Board const& board,
				ActionType action_type,
				board::ActionChoices const& choices)
			{
				std::pair<int, TreeNode*> next_info;
				if (action_type.IsChosenRandomly()) {
					next_info = SelectActionByRandom(action_type, choices); // TODO: do not create node
					int next_choice = next_info.first;
					assert(next_choice >= 0);
					if (GetCurrentNode() != nullptr) {
						// postpone the find of TreeNode on next non-random (sub)-action
						StepNext(next_choice, nullptr);
					}
					// TODO: we don't know if we 'created_new_node' here
					// so the caller cannot change to simulation mode
					return next_choice;
				}

				assert(action_type.IsChosenManually());
				if (GetCurrentNode() == nullptr) {
					if (action_type.GetType() == ActionType::kMainAction) {
						// TODO: use hash table to find mapped node

						// TODO: if a new node is created, it means we should already switched to simulation stage
					}
					else {
						// TODO: we can swap the random actions to be done first
						assert(false);
					}
				}
				next_info = SelectActionByChoice(action_type, board, choices);
				
				int next_choice = next_info.first;
				TreeNode* next_node = next_info.second;

				if (!next_node) {
					// all of the choices are invalid actions
					return -1;
				}

				StepNext(next_choice, next_node);
				return next_choice;
			}

			void FinishMainAction(bool * created_new_node) {
				*created_new_node = new_node_created_;
			}

			void ReportInvalidAction() {
				auto it = path_.rbegin();
				
				assert(it != path_.rend());
				TreeNode* child = it->node;
				TreeNode* parent = nullptr;
				int edge = it->leading_choice;

				while (true) {
					++it;
					assert(it != path_.rend()); // we should be able to find a blame node along the path
					parent = it->node;

					// if a sub-action failed, it means the main action failed.
					//    More precisely, it means the calling to FlowController is failed
					//    not only the callback like 'GetTarget()' or 'ChooseOne()' is failed
					//    so we find the node to blame, and remove it from its parent
					if (parent->GetActionType().IsInvalidStateBlameNode()) break;

					// look up further to find the blame node
					child = it->node;
					edge = it->leading_choice;
				}

				parent->MarkChildInvalid(edge, child);
			}

			std::vector<TraversedNodeInfo> const& GetTraversedPath() const { return path_; }

			TreeNode* GetCurrentNode() const { return path_.back().node; }

			void StepNext(int leading_choice, TreeNode* next_node)
			{
				path_.push_back({ leading_choice, next_node });
			}

		private:
			class SelectRandomlyHelper {
			public:
				SelectRandomlyHelper() : idx_(0), target_idx_(0), result_{ 0, nullptr } {}

				void ReportChoicesCount(int count) {
					target_idx_ = StaticConfigs::SelectionPhaseRandomActionPolicy::GetRandom(count);
					idx_ = 0;
				}
				void AddChoice(int choice, TreeNode* node) {
					// if an action was reported as invalid before, a nullptr is passed to 'node'
					assert(node); // random actions should be an invalid action
					if (idx_ == target_idx_) result_ = { choice, node };
					++idx_;
				}
				std::pair<int, TreeNode*> SelectChoice() {
					assert(result_.second);
					return result_;
				}

			private:
				int idx_;
				int target_idx_;
				std::pair<int, TreeNode*> result_;
			};

			std::pair<int, TreeNode*> SelectActionByRandom(ActionType action_type, board::ActionChoices const& choices)
			{
				return GetCurrentNode()->Select(action_type, choices, SelectRandomlyHelper(), &new_node_created_);
			}

		private:
			std::pair<int, TreeNode*> SelectActionByChoice(
				ActionType action_type, board::Board const& board, board::ActionChoices const& choices)
			{
				using PolicyHelper = StaticConfigs::SelectionPhaseSelectActionPolicy;
				return GetCurrentNode()->Select(action_type, choices, PolicyHelper(), &new_node_created_);
			}

		private:
			std::vector<TraversedNodeInfo> path_;

			std::vector<TraversedNodeInfo> saved_path_;

			bool new_node_created_;
		};
	}
}
