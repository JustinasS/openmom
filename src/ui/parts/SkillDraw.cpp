#include "SkillDraw.h"

#include "Gfx.h"
#include "Font.h"
#include "Localization.h"
#include "Items.h"

void SkillDraw::openHelpForSkill(const Unit* unit, int i)
{
  // TODO
  /*Skill s = u.getAbility(i);
  LocalGame.i.currentPlayer.send(new HelpSkillMessage(s));
  GameViewManager.instance.switchOverView(ViewType.MESSAGE);*/
  
  // TODO manage skills which are not real skills (xp, items, etc)
}

void SkillDraw::drawSkill(s16 index, SpriteInfo sprite, const std::string& text, s16 sx, s16 sy)
{
  s16 x = spX(index,sx);
  s16 y = spY(index,sy);
  
  Gfx::draw(LSI(UNITVIEW,5).relative(index), x-1, y-1);
  Gfx::draw(sprite, x, y);
  Fonts::drawString(text, FontFaces::Medium::TEAL_STROKE, x+18, y+4, ALIGN_LEFT);
}

void SkillDraw::draw(const Unit* unit)
{
  s16 curOffset = 0;
  
  // if the unit is a hero and we are on first page we should draw the experience and the items
  if (unit->type() == Productable::Type::HERO && page == 0)
  {
    const Hero* hero = static_cast<const Hero*>(unit);
    
    drawSkill(curOffset++, unit->level->index(), i18n::s(unit->level->name)+Fonts::format(" (%u xp)",unit->getExperience()), base.x, base.y);
    for (int i = 0; i < 3; ++i, ++curOffset)
    {
      if (!hero->itemAt(i))
      {
        // TODO: fix with new management
        //drawSkill(curOffset, items::Slots::slotsFor(hero->getSpec()->items)->sprites[i], "", base.x, base.y);
      }
    }
  }
  else
  {
    s16 p = unit->type() == Productable::Type::HERO ? page - 1 : page;
    
    if (unit->type() != Productable::Type::SUMMON && page == 0)
      drawSkill(curOffset++, unit->level->index(), i18n::s(unit->level->name)+Fonts::format(" (%u xp)",unit->getExperience()), base.x, base.y);
    
    for (int i = p*8; i < unit->skills()->size(); ++i)
    {
      const Skill* s = unit->skills()->get(i);
      drawSkill(curOffset++, s->icon(), s->name(), base.x, base.y);
    }
  }
}

void SkillDraw::draw(const UnitSpec* spec)
{
  s16 curOffset = 0;
  
  for (auto skill : spec->skills)
    drawSkill(curOffset++, skill->icon(), skill->name(), base.x, base.y);
}
