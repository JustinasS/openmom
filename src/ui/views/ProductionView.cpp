
#include "ProductionView.h"

#include "Gfx.h"
#include "Font.h"
#include "ViewManager.h"
#include "CityView.h"
#include "CityScape.h"
#include "UnitDraw.h"
#include "SkillDraw.h"
#include "CommonDraw.h"
#include "Buttons.h"

#include "Game.h"
#include "Player.h"
#include "City.h"
#include "UnitSpec.h"

#include "Localization.h"

#include <sstream>


using namespace std;

ProductionView::ProductionView(ViewManager *gvm) : View(gvm), city(nullptr), selected(0), which(SIDE_LEFT), unlockedProductableString("")
{
  buttons.resize(BUTTON_COUNT);
  
  buttons[CANCEL] = TristateButton::build("Cancel", 99, 181, TextureID::UNIT_DETAIL_BUTTONS, 2);
  buttons[OK] = TristateButton::build("Ok", 172, 181, TextureID::UNIT_DETAIL_BUTTONS, 1);
  
  buttons[CANCEL]->setAction([this,gvm](){ gvm->closeOverview(); });
  buttons[OK]->setAction([this,gvm]() {
    const Productable* production = which == SIDE_LEFT ? *next(left.begin(),selected) : *next(right.begin(),selected);
    
    if (production != city->getProductable())
    {
      city->setProductable(production);
      gvm->cityView()->buttonAt(CityView::Button::BUY)->activateIf(g->cityMechanics.isProductionBuyable(city));
    }
    left.clear();
    right.clear();
    gvm->closeOverview();
    
  });
}

void ProductionView::draw()
{
  Gfx::draw(TextureID::CITY_PRODUCTION_BACKDROP, 160 - 172/2, 0);
  
  const Productable* product = which == SIDE_LEFT ? *next(left.begin(),selected) : *next(right.begin(),selected);
  u16 cost = product->productionCost();
  
  if (which == SIDE_LEFT)
  {
    CityLayout::drawBuildingCentered(static_cast<const Building*>(product), 82, 38);
    
    Fonts::drawString(Fonts::format("Cost %u",cost), FontFaces::Small::TEAL, 128, 33, ALIGN_LEFT);
    Fonts::drawStringBounded(i18n::s(static_cast<const Building*>(product)->desc), FontFaces::Serif::TEAL, 87, 106, 150, ALIGN_LEFT);
    
    Fonts::drawString("Maintenance", FontFaces::Serif::TEAL, 85, 46, ALIGN_LEFT);
    Fonts::drawString("Allows", FontFaces::Serif::TEAL, 85, 46+11, ALIGN_LEFT);
    //TODO: print 0 for housing and trade goods
    CommonDraw::drawUpkeep(product->productionUpkeep(), 167, 49);
    
    if (unlockedProductableString != "")
    {
      Fonts::setFace(FontFaces::Medium::TEAL_STROKE);
      Fonts::setVerSpace(-1);
      Fonts::drawStringBounded(unlockedProductableString, 85+52, 46+12, 100, ALIGN_LEFT);
    }
  }
  else
  {
    const UnitSpec* spec = static_cast<const UnitSpec*>(product);
    Fonts::drawString("Moves", FontFaces::Small::TEAL, 128, 19, ALIGN_LEFT);
    Fonts::drawString("Upkeep", FontFaces::Small::TEAL, 128, 26, ALIGN_LEFT);
    CommonDraw::drawMovement(spec->movement, 165, 19, 0);
    CommonDraw::drawUpkeep(spec->upkeep, 164, 26);
    UnitDraw::drawUnitIso(spec, 83, 5, nullptr);
    CommonDraw::drawUnitProps(spec, 128, 47, 10);
    Fonts::drawString(Fonts::format("Cost %u(%u)", cost, cost), FontFaces::Small::TEAL, 128, 33, ALIGN_LEFT);
    
    SkillDraw::i.draw(spec);
  }
  
  Fonts::drawString(product->productionName(), FontFaces::Serif::TEAL, 128, 5, ALIGN_LEFT);
  
  
  int i = 0;
  for (const Productable* p : left)
  {
    Gfx::draw(TextureID::CITY_PRODUCTION_ITEMS, 0, 0, 0, 4 + 14*i);
    Fonts::drawString(p->productionName(), i == selected && which == SIDE_LEFT ? FontFaces::Medium::TEAL_BRIGHT : FontFaces::Medium::BLACK, 1, 4 + 14*i, ALIGN_LEFT);
    ++i;
  }
  i = 0;
  for (const Productable* p : right)
  {
    Gfx::draw(TextureID::CITY_PRODUCTION_ITEMS, 1, 0, 320 - 80, 4 + 14*i);
    Fonts::drawString(p->productionName(), i == selected && which == SIDE_RIGHT ? FontFaces::Medium::TEAL_BRIGHT : FontFaces::Medium::BLACK, 320 - 81 + 3, 4 + 14*i, ALIGN_LEFT);
    
    ++i;
  }
}

void ProductionView::mousePressed(u16 x, u16 y, MouseButton b)
{
  // 80 - 13
  if (b == BUTTON_LEFT)
  {
    if (x >= 0 && x < 80 && y >= 4 && y < left.size()*14 + 4)
    {
      selected = (y - 4) / 14;
      which = SIDE_LEFT;
      
      const Productable* product = *next(left.begin(),selected);
      
      auto unlockedItems = g->cityMechanics.itemsUnlockedByBuilding(static_cast<const Building*>(product), city);
      
      if (!unlockedItems.empty())
      {
        ostringstream ss;

        int i = 0;
        for (auto item : unlockedItems)
        {
          ss << item->productionName();
          
          if (i == unlockedItems.size() - 2) ss << " and ";
          else if (i == unlockedItems.size() - 1) ss << '.';
          else ss << ", ";
          
          ++i;
        }
        
        unlockedProductableString = ss.str();
      }
      else
        unlockedProductableString = "";
      
      
    }
    if (x >= 320-80 && x < 320 && y >= 4 && y < right.size()*14 + 4)
    {
      selected = (y - 4) / 14;
      which = SIDE_RIGHT;
    }
  }
}

void ProductionView::setCty(City *city)
{
  this->city = city;
  
  auto buildings = city->getOwner()->game()->cityMechanics.availableBuildings(city); //TODO: should use inner game, not the player one
  auto units = city->getOwner()->game()->cityMechanics.availableUnits(city);
  
  left.clear();
  right.clear();
  
  for (auto b : buildings) left.push_back(b);
  for (auto u : units) right.push_back(u);
  
  u16 i = 0;
  for (auto b : left)
  {
    if (b == city->getProductable())
    {
      which = SIDE_LEFT;
      selected = i;
      break;
    }
    ++i;
  }
  
  i = 0;
  for (auto u : right)
  {
    if (u == city->getProductable())
    {
      which = SIDE_RIGHT;
      selected = i;
      break;
    }
    ++i;
  }
}
