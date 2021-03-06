#pragma once

#include "state/Types.h"
#include "state/targetor/Targets.h"
#include "engine/FlowControl/Manipulate.h"
#include "engine/FlowControl/enchantment/Enchantments.h"

namespace state
{
	class State;
	namespace Cards { class Card; }
}

namespace engine {
	namespace FlowControl
	{
		namespace aura
		{
			class Handler;

			namespace contexts {
				struct AuraIsValid
				{
					Manipulate manipulate_;
					state::CardRef card_ref_;
					aura::Handler const& aura_handler_;
					bool & need_update_;
				};

				struct AuraGetTargets
				{
					Manipulate manipulate_;
					state::CardRef card_ref_;
					std::vector<state::CardRef> & new_targets;
				};

				struct AuraGetTarget
				{
					Manipulate manipulate_;
					state::CardRef card_ref_;
					state::CardRef & new_target;
				};

				struct AuraApplyOn
				{
					Manipulate manipulate_;
					state::CardRef card_ref_;
					state::CardRef target_;
				};

				struct AuraApplyFlagOnBoard
				{
					Manipulate manipulate_;
					state::CardRef card_ref_;
				};
				struct AuraRemoveFlagFromBoard
				{
					Manipulate manipulate_;
					state::CardRef card_ref_;
				};

				struct AuraApplyFlagOnOwnerPlayer
				{
					Manipulate manipulate_;
					state::CardRef card_ref_;
					state::PlayerIdentifier player_;
				};
				struct AuraRemoveFlagFromOwnerPlayer
				{
					Manipulate manipulate_;
					state::CardRef card_ref_;
					state::PlayerIdentifier player_;
				};
			}
		}
	}
}