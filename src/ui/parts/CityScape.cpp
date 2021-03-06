#include "CityScape.h"

#include "Texture.h"
#include "Buildings.h"
#include "City.h"
#include "Gfx.h"
#include "GfxData.h"
#include "common/Util.h"

#include <set>

using namespace std;

map<const City*, CityLayout*> CityLayout::layouts;

/*
 4x4  4x4  3x4  4x4
 2x3s 4x3  3x3c 4x3  2x2
 4x4  3x4  4x4  3x4
 */
static const CityLayout::LayoutZone zone_coords[] = {

  CityLayout::LayoutZone( 30, 129, 4, 4),
  CityLayout::LayoutZone( 78, 129, 4, 4),
  CityLayout::LayoutZone(126, 129, 3, 4),
  CityLayout::LayoutZone(164, 129, 4, 4),
  
  CityLayout::LayoutZone( 27, 152, 2, 3, false, true),
  CityLayout::LayoutZone( 55, 152, 4, 3),
  CityLayout::LayoutZone(103, 152, 3, 3, true, false),
  CityLayout::LayoutZone(141, 152, 4, 3),
  CityLayout::LayoutZone(185, 157, 2, 2),
  
  CityLayout::LayoutZone( 37, 170, 4, 4),
  CityLayout::LayoutZone( 85, 170, 3, 4),
  CityLayout::LayoutZone(123, 170, 4, 4),
  CityLayout::LayoutZone(172, 170, 3, 4)
};

//static constexpr size_t total_zones = 4*4 + 4*4 + 3*4 + 4*4 + 2*3 + 4*3 + 3*3 + 4*3 + 2*2 + 4*4 + 3*4 + 4*4 + 3*4;

/* TODO: zones change according to city size as a smaller city doesn't have some road intersections */
const std::vector<CityLayout::LayoutZone> CityLayout::getZones(const City* city)
{
  return std::vector<CityLayout::LayoutZone>(begin(zone_coords), end(zone_coords));
}

enum
{
  by_river = 0,
  by_ocean,
  
  normal_bg,
  flying_bg,
  famine_bg, // TODO: maybe it's also desert bg
  cursed_lands_bg,
  gaias_bg,
  
  plains_bg,
  hills_bg,
  mountains_bg,
  
  chaos_rift_bg,
  heavenly_light_bg,
  cloud_of_shadow_bg,
  
  wall_stone,
  wall_fire,
  wall_darkness,
  //wall_stine // TODO: ?????
};

static constexpr SpriteInfo::data_type city_statics[][PLANE_COUNT] = {
  { LBXI(CITYSCAP, 3), LBXI(CITYSCAP, 115) },
  { LBXI(CITYSCAP, 4), LBXI(CITYSCAP, 116) },
  
  { LBXF(CITYSCAP, 0, 4), LBXF(CITYSCAP, 8, 4) },
  { LBXF(CITYSCAP, 0, 1), LBXF(CITYSCAP, 8, 1) },
  { LBXF(CITYSCAP, 0, 2), LBXF(CITYSCAP, 8, 2) },
  { LBXF(CITYSCAP, 0, 0), LBXF(CITYSCAP, 8, 0) },
  { LBXF(CITYSCAP, 0, 3), LBXF(CITYSCAP, 8, 3) },
  
  { LBXI(CITYSCAP, 7), LBXI(CITYSCAP, 11) },
  { LBXI(CITYSCAP, 1), LBXI(CITYSCAP, 9) },
  { LBXI(CITYSCAP, 2), LBXI(CITYSCAP, 10) },
  
  { LBXI(CITYSCAP, 92), LBXI(CITYSCAP, 112) },
  { LBXI(CITYSCAP, 93), LBXI(CITYSCAP, 113) },
  { LBXI(CITYSCAP, 91), LBXI(CITYSCAP, 111) },
  
  { LBXI(CITYSCAP, 76), LBXI(CITYSCAP, 76) },
  { LBXI(CITYSCAP, 77), LBXI(CITYSCAP, 77) },
  { LBXI(CITYSCAP, 79), LBXI(CITYSCAP, 79) },
  //{ LBXI(CITYSCAP, 80), LBXI(CITYSCAP, 80) },


};

void CityLayout::draw(const City *city, LocalPlayer *player)
{
  Plane p = city->getPosition().plane;
  Gfx::draw(city_statics[normal_bg][p], 3, 101);
  
  //TODO: if (!city->hasSpell(Spells::FLYING_FORTRESS)
  {
  // draw correct far sight according to city features: grasslands, hills, mountain */
  if (city->hasPlacement(CITY_BY_MOUNTAIN))
    Gfx::draw(city_statics[mountains_bg][p], 3, 101);
  else if (city->hasPlacement(CITY_BY_HILL))
    Gfx::draw(city_statics[hills_bg][p], 3, 101);
  else
    Gfx::draw(city_statics[plains_bg][p], 3, 101);
  }
  
  // draw correct far spell effects for cloud of darkness / heavenly light / etc
//  if (city->hasSpell(Spells::CLOUD_OF_SHADOW))   // TODO: MISSING SPELL
  //  Gfx::draw(TextureID::CITY_LANDSCAPE, 6+3*p, 0, 3, 101);
  
  
  Gfx::draw(TSI(CITY_ROADS, 0, 0), 5, 118);
  
  const auto& houseSpec = GfxData::raceHouseGfxSpec(GfxData::raceGfxSpec(city->race).houseType);
  
  const CityLayout* layout = layouts[city];
  
  if (city->hasPlacement(CITY_BY_SEA))
    Gfx::drawAnimated(city_statics[by_ocean][p], 4, 100);
  else if (!city->hasPlacement(CITY_BY_SEA) && city->hasPlacement(CITY_BY_RIVER))
    Gfx::drawAnimated(city_statics[by_river][p], 4, 100, 0, 5);
  
  for (auto &p : layout->positions)
  {
    if (p.building)
      drawBuilding(p.building, p.x, p.y);
    else
    {
      const SpriteInfo houseGfx = houseSpec.cityScapeHouse.relative(p.house);
      Gfx::draw(houseGfx, p.x, p.y - houseGfx.sh());
    }
  }
  
  //draw walls
  //TODO: other kinds of walls + city wall spell
  if (city->hasBuilding(Building::CITY_WALLS))
    Gfx::drawAnimated(city_statics[wall_stone][p], 3, 183);
  /*if (c->hasSpell(Spells::WALL_OF_DARKNESS))   //TODO MISSING SPELL
    Gfx::drawAnimated(city_statics[wall_darkness][p], 3, 185);
   if (c->hasSpell(Spells::WALL_OF_FIRE)) // TODO MISSING SPELL
    Gfx::drawAnimated(city_statics[wall_fire][p], 3, 185);*/
}

void CityLayout::drawBuilding(const Building *building, s16 x, s16 y)
{
  const BuildingGfxSpec& spec = GfxData::buildingGfx(building);
  drawBuildingSprite(spec.gfx, x, y);
}

void CityLayout::drawBuildingSprite(SpriteInfo info, s16 x, s16 y)
{
  int height = info.sh();
  Gfx::drawAnimated(info, x, y - height, 0);
}

void CityLayout::drawBuildingCentered(const City* city, const Building *building, s16 x, s16 y)
{
  if (building == Building::HOUSING)
  {
    const auto& houseGfx = GfxData::raceHouseGfxSpec(GfxData::raceGfxSpec(city->race).houseType);
    drawBuildingSprite(houseGfx.housingBuilding, x, y);
  }
  else if (building == Building::TRADE_GOODS || building == Building::CITY_WALLS)
    drawBuilding(building, x, y);
  else
  {
    const BuildingGfxSpec& spec = GfxData::buildingGfx(building);
    int height = spec.gfx.sh();

    Gfx::drawAnimated(spec.gfx, x + (40 - spec.width)/2, y - height, 0);
  }
}


void CityLayout::deploy()
{
  positions.clear();
  zones = getZones(city);
  
  set<const Building*> buildings;
  
  for (auto b : city->buildings)
    if (b != Building::CITY_WALLS)
      buildings.insert(b);
  
  if (buildings.find(Building::MAGE_FORTRESS) != buildings.end())
  {
    positions.push_back(createPosition(zones[6], 0, 0, Building::MAGE_FORTRESS));
    zones.erase(zones.begin()+6);
    buildings.erase(Building::MAGE_FORTRESS);
  }
  if (buildings.find(Building::SHIP_WRIGHTS_GUILD) != buildings.end())
  {
    positions.push_back(createPosition(zones[4], 0, 0, Building::SHIP_WRIGHTS_GUILD));
    zones.erase(zones.begin()+4);
  }
  else if (buildings.find(Building::MARITIME_GUILD) != buildings.end())
  {
    positions.push_back(createPosition(zones[4], 0, 0, Building::MARITIME_GUILD));
    zones.erase(zones.begin()+4);
  }
  if (buildings.find(Building::SHIP_YARD) != buildings.end())
  {
    positions.push_back(createPosition(zones[4], 0, 0, Building::SHIP_YARD));
    zones.erase(zones.begin()+4);
  }
  
  vector<const Building*> vbuildings;
  vbuildings.reserve(buildings.size());

  auto GFX_COMPARATOR = [] (const Building *b1, const Building* b2)
  {
    const auto& s1 = GfxData::buildingGfx(b1);
    const auto& s2 = GfxData::buildingGfx(b2);

    s32 t1 = s1.slotSize.w*s1.slotSize.h;
    s32 t2 = s2.slotSize.w*s2.slotSize.h;

    return t1 < t2;
  };
  
  std::copy(buildings.begin(), buildings.end(), std::back_inserter(vbuildings));
  std::sort(vbuildings.begin(), vbuildings.end(), GFX_COMPARATOR);
  
  for (auto b : vbuildings)
  {
    auto suitable = findSuitable(b);
    
    if (suitable.empty())
    {
      deploy();
      return;
    }
    
    const LayoutZone& zone = Util::randomElement(suitable);
    auto it = std::find(zones.begin(), zones.end(), zone);
    placeAndSplit(b, it);
  }
  
  // TODO: lower when building amount increases
  int houses = 20 + Math::randomIntUpTo(10);
  for (int i = 0; i < houses; ++i)
  {
    zone_iterator zone = Util::randomElementIterator(zones);
    placeAndSplit(nullptr, zone);
  }

  sort(positions.begin(), positions.end());
}

CityLayout::LayoutPosition CityLayout::createPosition(const LayoutZone& zone, s16 ox, s16 oy, const Building *building)
{
  int sx = zone.x + ox;
  int sy = zone.y + oy + (building ? GfxData::buildingGfx(building).slotSize.h - 1 : 0);
  
  int fx = zone.coords.x + sx*U*2 - sy*U;
  int fy = zone.coords.y + sy*U;
  
  if (building)
    return LayoutPosition(fx, fy, building);
  else
    return LayoutPosition(fx, fy, Math::randomIntUpTo(5));
}

const vector<CityLayout::LayoutZone> CityLayout::findSuitable(const Building* building)
{
  vector<LayoutZone> zn;
  copy_if(zones.begin(), zones.end(), back_inserter(zn), [&](const LayoutZone& z) { return z.w >= GfxData::buildingGfx(building).slotSize.w && z.h >= GfxData::buildingGfx(building).slotSize.h; });
  return zn;
}

void CityLayout::placeAndSplit(const Building *b, zone_iterator it)
{
  int bw = 1, bh = 1;

  if (b)
  {
    const auto& spec = GfxData::buildingGfx(b);
    bw = GfxData::buildingGfx(b).slotSize.w;
    bh = GfxData::buildingGfx(b).slotSize.h;
  }

  LayoutZone z = *it;
  positions.push_back(createPosition(z,0,0,b));
  zones.erase(it);
  
  LOGD2("[city-scape] placing at (%u,%u), size (%u,%u)", z.x, z.y, bw, bh);
  LOGD2("[city-scape]   splitting zone at (%u,%u), offset: (%u,%u), size: (%u,%u)", z.coords.x, z.coords.y, z.x, z.y, z.w, z.h);

  if (bw == z.w && bh == z.h)
    /* do nothing, there's no need to split */;
  else if (z.w > bw && z.h > bh)
  {
    /* split in 3 zones
       ZZZ    KKY
       XXZ ->   U
       XXZ      U
     */
    LOGD2("[city-scape]    into offset: (%u,%u), size: (%u,%u)", z.x + bw, z.y, z.w - bw, bh);
    LOGD2("[city-scape]         offset: (%u,%u), size: (%u,%u)", z.x, z.y + bh, bw, z.h - bh);
    LOGD2("[city-scape]         offset: (%u,%u), size: (%u,%u)", z.x + bw, z.y + bh, z.w - bw, z.h - bh);

    
    zones.emplace_back(z.coords, z.x + bw, z.y, z.w - bw, bh);
    zones.emplace_back(z.coords, z.x, z.y + bh, bw, z.h - bh);
    zones.emplace_back(z.coords, z.x + bw, z.y + bh, z.w - bw, z.h - bh);
  }
  else if (z.w > bw)
  {
    LOGD2("[city-scape]    into offset: (%u,%u), size: (%u,%u)", z.x + bw, z.y, z.w - bw, z.h);
    zones.emplace_back(z.coords, z.x + bw, z.y, z.w - bw, z.h);
  }
  else if (z.h > bh)
  {
    LOGD2("[city-scape]    into offset: (%u,%u), size: (%u,%u)", z.x, z.y + bh , z.w, z.h - bh);
    zones.emplace_back(z.coords, z.x, z.y + bh, z.w, z.h - bh);
  }
  
  LOGD2("[city-scape] free zones: %zu", zones.size())
}


