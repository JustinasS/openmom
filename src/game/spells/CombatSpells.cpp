#include "CombatSpells.h"

#include "Spells.h"
#include "Army.h"
#include "Unit.h"


value_t CombatEnchModifier::apply(const combat::Combat* combat, const CombatEnchCast& cast, const Unit* unit, Property property) const
{
  if (cast.spell()->target == Target::ENEMY_ARMY && cast.caster().player() != unit->getArmy()->getOwner())
    return doApply(unit, property);
  else if (cast.spell()->target == Target::FRIENDLY_ARMY && cast.caster().player() == unit->getArmy()->getOwner())
    return doApply(unit, property);
  else if (cast.spell()->target == Target::BOTH_ARMIES)
    return doApply(unit, property);
  
  return 0;
}
