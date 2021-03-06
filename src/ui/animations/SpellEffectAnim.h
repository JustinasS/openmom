#pragma once

#include "Animations.h"
#include "common/Common.h"
#include "CombatUtils.h"

class SummonSpec;



namespace anims
{
  class SpellEffect : public DiscreteAnimation
  {
  public:
    constexpr static u32 TICKS_PER_FRAME = 2;
    
  private:
    SpriteInfo effect;
    bool force;

    Point coords;
    u16 counter;
    u16 frame;
    
  public:
    SpellEffect(LocalPlayer* player, SpriteInfo effectId, Position tile);
    SpellEffect(SpriteInfo effectId, combat::CombatCoord tile);
    SpellEffect(LocalPlayer* player, SpriteInfo effectId, int tx, int ty);
    
    void step() override;
    
    bool hasFinished() override
    {
      return force || DiscreteAnimation::hasFinished();
    }
  };
  
  class SummonAnimation : public ContinuousEndlessAnimation
  {
    const SummonSpec* const spec;
    SpriteInfo wizard;
    SpriteInfo creature;
    const Palette* palette;
        
  public:
    SummonAnimation(const Wizard* wizard, const SummonSpec* spec);
    void step() override;
    bool mouseReleased(u16 x, u16 y, MouseButton b) override;
  };

  class SpellDiscoverAnimation : public ContinuousEndlessAnimation
  {
  private:
    SpriteInfo wizard;
    SpriteInfo familiar;
    
  public:
    SpellDiscoverAnimation(const Wizard* wizard, School school);
    void step() override;
    bool mouseReleased(u16 x, u16 y, MouseButton b) override;
  };
  
  class GlobalEnchantmentAnimation : public ContinuousEndlessAnimation
  {
  private:
    SpriteInfo wizard;
    SpriteInfo enchant;
    
  public:
    GlobalEnchantmentAnimation(const Wizard* wizard, SpriteInfo enchantment);
    void step() override;
    bool mouseReleased(u16 x, u16 y, MouseButton b) override;
  };
  
  
};
