#include "squad.h"
#include "agent.h"

const size_t Squad::kMaxSlot = 4;
const size_t Squad::kSlot1 = 0;
const size_t Squad::kSlot2 = 1;
const size_t Squad::kSlot3 = 2;
const size_t Squad::kSlot4 = 3;

//! Default constructor
Squad::Squad() {
    clear();
}

void Squad::clear() {
    for (size_t s=0; s<kMaxSlot; s++) {
        a_members_[s] = NULL;
    }
}
    
//! Returns true if the slot holds an agent and if he's active
bool Squad::isSlotActive(size_t slotId) {
    assert(slotId < kMaxSlot);
    return a_members_[slotId] && a_members_[slotId]->isActive();
}

//! Return the slot that holds the given agent or -1 if ni agent is found
int Squad::getTeamSlot(Agent *pAgent) {
    if (pAgent) {
        for (size_t i=0; i<kMaxSlot; i++) {
            if (pAgent == a_members_[i]) {
                return i;
            }
        }
    }
    return -1;
}