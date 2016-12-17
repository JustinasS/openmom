//
//  ColorMap.h
//  OpenMoM
//
//  Created by Jack on 7/18/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _COLOR_MAP_H_
#define _COLOR_MAP_H_

#include "Common.h"

#include <initializer_list>
#include <unordered_set>
#include <unordered_map>

#include <set>

class ColorMap
{
  public:
    virtual u32 get(u32 k) const = 0;
};

using color_list = std::initializer_list<Color>;
using color_map = std::unordered_map<Color, Color>;

class BlinkMap : public ColorMap
{
  private:
    const s16 s[3];
    const s16 d[3];
    const u16 ticks, updatedForTick = -1;
  
    std::unordered_set<Color> set;
    //Color color;
  
  
  public:
    BlinkMap(color_list colors, u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2, u16 ticks) :
      set(colors), s{r1,g1,b1}, d{static_cast<s16>(r2-r1),static_cast<s16>(g2-g1),static_cast<s16>(b2-b1)}, ticks(ticks) { }
  
    u32 get(u32 k) const override;
  
};

class HashColorMap : public ColorMap
{
  private:
    color_map map;
  
  public:
    HashColorMap(color_list data)
    {
      color_list::iterator it = data.begin();
      
      while (it != data.end())
      {
        Color src = *it++;
        Color dst = *it++;
        map[src] = dst;
      }
    }
  
    HashColorMap(color_list from, color_list to)
    {
      color_list::iterator it = from.begin(), it2 = to.begin();
      while (it != from.end())
        map[*it++] = *it2++;
    }

    u32 get(u32 k) const override
    {
      if (k != 0)
      {
        //printf("%d %d %d %d\n", k >> 24, (k>>16)&0xff, (k>>8)&0xff, k&0xff);
      
        color_map::const_iterator it = map.find(k);
      
        return it != map.end() ? it->second : k;
      }
      
      return k;
    }
};

class MiscMaps
{
public:
  static const Color SCHOOL_GLOW_COLORS[][5];
  static constexpr u8 SCHOOL_GLOW_COUNT = sizeof(SCHOOL_GLOW_COLORS[0])/sizeof(SCHOOL_GLOW_COLORS[0][0]);
  
  static const HashColorMap FLAG_COLORS_MAP[6];
  static constexpr u8 FLAG_COLORS_COUNT = sizeof(FLAG_COLORS_MAP)/sizeof(FLAG_COLORS_MAP[0]);
  
  class GrayscaleMap : public ColorMap
  {
    public:
      GrayscaleMap() { }
      
      u32 get(u32 k) const override
      {
        if (k & 0xFF000000)
        {
          u8 r = (k >> 16) & 0xFF;
          u8 g = (k >> 8) & 0xFF;
          u8 b = k & 0xFF;
          
          u8 v = static_cast<u8>(0.21f*r + 0.72f*g + 0.07*b);
          return 0xFF000000 | (v << 16) | (v << 8) | v;
        }
        else
          return k;
        
      };
  };
  
  
  static const GrayscaleMap GRAYSCALE;
};

class Palette
{
public:
  virtual Color get(u8 index) const = 0;
  virtual ~Palette() { }
};

class IndexedPalette : public Palette
{
private:
  Color *colors;
public:
  IndexedPalette(Color* colors) : colors(colors) { }
  IndexedPalette(u8 size) : colors(new Color[size]) { }
  
  IndexedPalette(color_list colors) : colors(new Color[colors.size()]) { setPalette(colors); }
  
  void setPalette(color_list colors) const
  {
    for (int i = 0; i < colors.size(); ++i)
      this->colors[i] = *std::next(colors.begin(), i);
  }
  
  void set(u8 index, Color color) { colors[index] = color; }
  Color get(u8 index) const override { return colors[index]; }
  
  ~IndexedPalette() { delete [] colors; }
};

class BlinkingPalette : public Palette
{
private:
  const s16 s[3];
  const s16 d[3];
  const u16 ticks, updatedForTick = -1;
  
  std::set<u8> indices;
  
public:
  BlinkingPalette(std::initializer_list<u8> indices, u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2, u16 ticks) : indices(indices), s{r1,g1,b1}, d{static_cast<s16>(r2-r1),static_cast<s16>(g2-g1),static_cast<s16>(b2-b1)}, ticks(ticks)
  {

  }

  Color get(u8 index) const override;
};

#endif
