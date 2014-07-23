#include "City.h"

#include "Player.h"
#include "Buildings.h"

City::City(Player *owner, std::string name, u16 population, Position position) :
  owner(owner), name(name), population(population), position(position), race(owner->race), production(nullptr)
{
  if (population < 1000)
    isStillOutpost = true;
  
  /* TODO: calculate max population */
  maxPopulation = 25;
  owner->add(this);
  
  production = Building::get(BuildingID::HOUSING);
  productionPool = 0;
}

bool City::hasBuilding(const BuildingID b) { return std::find_if(buildings.begin(), buildings.end(), [&](const Building* bb) { return bb->ident == b; }) != buildings.end(); }