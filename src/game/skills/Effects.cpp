#include "Effects.h"

#include "Unit.h"

value_t ModifierValue::transformValue(value_t previous, const Unit* unit) const
{
  if (type == Type::ADDITIVE)
    return previous + value;
  else if (type == Type::ADDITIVE_LEVEL_BASED)
    return previous + static_cast<value_t>(std::floor((unit->experienceMultiplier())*multiplier));
  else if (type == Type::MULTIPLICATIVE)
    return static_cast<value_t>(std::floor(previous * multiplier));
  else if (type == Type::FIXED)
    return value;
  else
  {
    assert(false);
    return 0;
  }
}

template class PropertyModifierEffect<WizardAttribute, SkillEffect::Type::WIZARD_BONUS>;
template class PropertyModifierEffect<Property, SkillEffect::Type::UNIT_BONUS>;

bool ArmyBonus::applicableOn(const Unit* unit) const
{
  return target == Type::WHOLE_ARMY || (target == Type::NORMAL_UNITS && unit->type() == Productable::Type::UNIT);
}

value_t ArmyBonus::getValue(const Unit* unit) const {
  return applicableOn(unit) ? value : 0;
}
value_t ArmyLevelBonus::getValue(const Unit* unit) const
{
  return applicableOn(unit) ? static_cast<value_t>(std::floor((unit->getExperienceLevel()->index()+1)*multiplier)) : 0;
  // TODO: is +1 intended behavior? According to OSG it is.
}






effect_list effect_list::actuals(const Unit* unit) const
{
  std::unordered_multimap<const SkillEffectGroup*, const SkillEffect*> byGroup;

  using pair_t = const decltype(byGroup)::value_type;
  const auto sorterByMagnitude = [unit](const pair_t& e1, const pair_t& e2) { return e1.second->compare(unit, e2.second) == Order::LESSER; };
  static const auto sorterByPriority = [](const pair_t& e1, const pair_t& e2) { return e1.second->groupParam() < e2.second->groupParam(); };

  /* group effects by group */
  std::transform(data.begin(), data.end(), std::inserter(byGroup, byGroup.begin()), [] (const SkillEffect* effect) { return std::make_pair(effect->group(), effect); });
  
  effect_list actuals;
  
  for (auto it = byGroup.begin(); it != byGroup.end(); /**/)
  {
    const SkillEffectGroup* group = it->first;
    
    auto pair = byGroup.equal_range(group);
    
    if (!group || group->mode()== SkillEffectGroup::Mode::KEEP_ALL)
      std::transform(pair.first, pair.second, std::back_inserter(actuals), [] (const pair_t& entry) { return entry.second; });
    else
    {
      //TODO: write tests in Data YAML loading for preconditions about these modes
      switch (group->mode())
      {
        case SkillEffectGroup::Mode::UNIQUE:
        {
          /* just push one, they should be all equal for the same group */
          actuals.push_back(pair.first->second);
          break;
        }
          
        case SkillEffectGroup::Mode::KEEP_GREATER:
        {
          auto max = std::max_element(pair.first, pair.second, sorterByMagnitude);
          actuals.push_back(max->second);
          break;
        }
          
        case SkillEffectGroup::Mode::KEEP_LESSER:
        {
          auto min = std::min_element(pair.first, pair.second, sorterByMagnitude);
          actuals.push_back(min->second);
          break;
        }

        case SkillEffectGroup::Mode::PRIORITY:
        {
          /* higher has priority */
          auto min = std::min_element(pair.first, pair.second, sorterByPriority);
          actuals.push_back(min->second);
          break;
        }
          
        case SkillEffectGroup::Mode::KEEP_ALL:
        {
          /* already handled in general case outside switch */
          break;
        }
      }
    }
    
    it = pair.second;
  }

  return actuals;
}

/* flatten nested effects */
effect_list effect_list::flatten()
{
  std::vector<const SkillEffect*> data;
  std::copy(dbegin(), dend(), std::back_inserter(data));

  return data;
}
