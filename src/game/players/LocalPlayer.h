//
//  LocalPlayer.h
//  OpenMoM
//
//  Created by Jack on 7/19/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _LOCAL_PLAYER_H_
#define _LOCAL_PLAYER_H_

#include "Common.h"

#include "Spells.h"
#include "Army.h"
#include "Player.h"

#include "Messages.h"

#include "MiniMap.h"

#include <list>

class Game;
class Army;
class Route;


class LocalPlayer : public Player
{
private:
  Position viewport;

  s16 spellBookPage;
  
  Army* selectedArmy;
  const Route* selectedRoute;
  unit_list selectedUnits;
  
  bool drawSelectedArmy = true;
  bool mapGridEnabled = true;
  
  Target target = Target::NONE;
  
  CombatUnit *combatSelectedUnit;
  bool combatCurrentlyPlaying;
  
  MiniMap map;
  
  std::list<const messages::Message*> messages;
  
public:
  LocalPlayer(Game *game, std::string name, const Wizard& wizard, PlayerColor color, const Race& race, u16 mapWidth, u16 mapHeight);

  void selectArmy(Army* army);
  Army* getSelectedArmy() const override { return selectedArmy; }
  const movement_list selectedArmyMovementType();
  bool selectedArmyCanBuildOutpost();
  s16 selectedCount() const override { return selectedUnits.size(); }
  void selectNone();
  void selectAll() override;
  s16 selectedAvailMoves();
  bool wholeSelected() { return selectedCount() == selectedArmy->size(); }
  void resetArmy() { selectedArmy = nullptr; selectedRoute = nullptr; }
  void computeRoute(s16 dx, s16 dy);
  bool consumeRoute();
  Army* splitAndSelect();
  
  void setSpellTarget(Target target) { this->target = target; }
  Target getSpellTarget() const { return target; }
  
  void moveCombatUnit(CombatUnit* unit) { combatSelectedUnit = unit; }
  
  void selectUnit(Unit* unit);
  void deselectUnit(Unit* unit);
  bool isSelectedUnit(Unit* unit);

  
  void combatTurnBegun() { combatCurrentlyPlaying = true; }
  void combatTurnEnded() { combatCurrentlyPlaying = false; }
  
  const MiniMap* miniMap() const { return &map; }
  void discoverTile(const Position& position) override { map.discover(position); }
  void switchPlane() { viewport.plane = viewport.plane == ARCANUS ? MYRRAN : ARCANUS; }
  const Position& getViewport() const { return viewport; }
  void setViewport(s16 x, s16 y);

  void push(Animation* animation) override;

  void send(messages::Message* message) override { messages.push_back(message); }
  bool hasMessage() { return !messages.empty(); }
};

#endif
