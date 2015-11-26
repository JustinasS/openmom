//
//  Gfx.cpp
//  OpenMoM
//
//  Created by Jack on 7/18/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "Gfx.h"

#include "Texture.h"
#include "GfxData.h"
#include "Util.h"
#include "SDL.h"

#include "LBX.h"

u32 Gfx::ticks = 0;
u32 Gfx::fticks = 0;
u32 Gfx::fticksr = 0;

SurfaceWrapper* Gfx::buffer = nullptr;
SurfaceWrapper* Gfx::activeBuffer = nullptr;
SurfaceWrapper* Gfx::canvas = nullptr;

const ColorMap* Gfx::map = nullptr;
const Palette* Gfx::palette = nullptr;

const Color Gfx::PALETTE[256] =
{
  RGB(0x0, 0x0, 0x0),
  RGB(0x8, 0x4, 0x4),
  RGB(0x24, 0x1c, 0x18),
  RGB(0x38, 0x30, 0x2c),
  RGB(0x48, 0x40, 0x3c),
  RGB(0x58, 0x50, 0x4c),
  RGB(0x68, 0x60, 0x5c),
  RGB(0x7c, 0x74, 0x70),
  RGB(0x8c, 0x84, 0x80),
  RGB(0x9c, 0x94, 0x90),
  RGB(0xac, 0xa4, 0xa0),
  RGB(0xc0, 0xb8, 0xb4),
  RGB(0xd0, 0xc8, 0xc4),
  RGB(0xe0, 0xd8, 0xd4),
  RGB(0xf0, 0xe8, 0xe4),
  RGB(0xfc, 0xfc, 0xfc),
  RGB(0x38, 0x20, 0x1c),
  RGB(0x40, 0x2c, 0x24),
  RGB(0x48, 0x34, 0x2c),
  RGB(0x50, 0x3c, 0x30),
  RGB(0x58, 0x40, 0x34),
  RGB(0x5c, 0x44, 0x38),
  RGB(0x60, 0x48, 0x3c),
  RGB(0x64, 0x4c, 0x3c),
  RGB(0x68, 0x50, 0x40),
  RGB(0x70, 0x54, 0x44),
  RGB(0x78, 0x5c, 0x4c),
  RGB(0x80, 0x64, 0x50),
  RGB(0x8c, 0x70, 0x58),
  RGB(0x94, 0x74, 0x5c),
  RGB(0x9c, 0x7c, 0x64),
  RGB(0xa4, 0x84, 0x68),
  RGB(0xec, 0xc0, 0xd4),
  RGB(0xd4, 0x98, 0xb4),
  RGB(0xbc, 0x74, 0x94),
  RGB(0xa4, 0x54, 0x7c),
  RGB(0x8c, 0x38, 0x60),
  RGB(0x74, 0x24, 0x4c),
  RGB(0x5c, 0x10, 0x34),
  RGB(0x44, 0x4, 0x20),
  RGB(0xec, 0xc0, 0xc0),
  RGB(0xd4, 0x94, 0x94),
  RGB(0xbc, 0x74, 0x74),
  RGB(0xa4, 0x54, 0x54),
  RGB(0x8c, 0x38, 0x38),
  RGB(0x74, 0x24, 0x24),
  RGB(0x5c, 0x10, 0x10),
  RGB(0x44, 0x4, 0x4),
  RGB(0xec, 0xd4, 0xc0),
  RGB(0xd4, 0xb4, 0x98),
  RGB(0xbc, 0x98, 0x74),
  RGB(0xa4, 0x7c, 0x54),
  RGB(0x8c, 0x60, 0x38),
  RGB(0x74, 0x4c, 0x24),
  RGB(0x5c, 0x34, 0x10),
  RGB(0x44, 0x24, 0x4),
  RGB(0xec, 0xec, 0xc0),
  RGB(0xd4, 0xd4, 0x94),
  RGB(0xbc, 0xbc, 0x74),
  RGB(0xa4, 0xa4, 0x54),
  RGB(0x8c, 0x8c, 0x38),
  RGB(0x74, 0x74, 0x24),
  RGB(0x5c, 0x5c, 0x10),
  RGB(0x44, 0x44, 0x4),
  RGB(0xd4, 0xec, 0xbc),
  RGB(0xb8, 0xd4, 0x98),
  RGB(0x98, 0xbc, 0x74),
  RGB(0x7c, 0xa4, 0x54),
  RGB(0x60, 0x8c, 0x38),
  RGB(0x4c, 0x74, 0x24),
  RGB(0x38, 0x5c, 0x10),
  RGB(0x24, 0x44, 0x4),
  RGB(0xc0, 0xec, 0xc0),
  RGB(0x98, 0xd4, 0x98),
  RGB(0x74, 0xbc, 0x74),
  RGB(0x54, 0xa4, 0x54),
  RGB(0x38, 0x8c, 0x38),
  RGB(0x24, 0x74, 0x24),
  RGB(0x10, 0x5c, 0x10),
  RGB(0x4, 0x44, 0x4),
  RGB(0xc0, 0xec, 0xd8),
  RGB(0x98, 0xd4, 0xb8),
  RGB(0x74, 0xbc, 0x98),
  RGB(0x54, 0xa4, 0x7c),
  RGB(0x38, 0x8c, 0x60),
  RGB(0x24, 0x74, 0x4c),
  RGB(0x10, 0x5c, 0x38),
  RGB(0x4, 0x44, 0x24),
  RGB(0xf4, 0xc0, 0xa0),
  RGB(0xe0, 0xa0, 0x84),
  RGB(0xcc, 0x84, 0x6c),
  RGB(0xc8, 0x8c, 0x68),
  RGB(0xa8, 0x78, 0x54),
  RGB(0x98, 0x68, 0x4c),
  RGB(0x8c, 0x60, 0x44),
  RGB(0x7c, 0x50, 0x3c),
  RGB(0xc0, 0xd8, 0xec),
  RGB(0x94, 0xb4, 0xd4),
  RGB(0x70, 0x98, 0xbc),
  RGB(0x54, 0x7c, 0xa4),
  RGB(0x38, 0x64, 0x8c),
  RGB(0x24, 0x4c, 0x74),
  RGB(0x10, 0x38, 0x5c),
  RGB(0x4, 0x24, 0x44),
  RGB(0xc0, 0xc0, 0xec),
  RGB(0x98, 0x98, 0xd4),
  RGB(0x74, 0x74, 0xbc),
  RGB(0x54, 0x54, 0xa4),
  RGB(0x3c, 0x38, 0x8c),
  RGB(0x24, 0x24, 0x74),
  RGB(0x10, 0x10, 0x5c),
  RGB(0x4, 0x4, 0x44),
  RGB(0xd8, 0xc0, 0xec),
  RGB(0xb8, 0x98, 0xd4),
  RGB(0x98, 0x74, 0xbc),
  RGB(0x7c, 0x54, 0xa4),
  RGB(0x60, 0x38, 0x8c),
  RGB(0x4c, 0x24, 0x74),
  RGB(0x38, 0x10, 0x5c),
  RGB(0x24, 0x4, 0x44),
  RGB(0xec, 0xc0, 0xec),
  RGB(0xd4, 0x98, 0xd4),
  RGB(0xbc, 0x74, 0xbc),
  RGB(0xa4, 0x54, 0xa4),
  RGB(0x8c, 0x38, 0x8c),
  RGB(0x74, 0x24, 0x74),
  RGB(0x5c, 0x10, 0x5c),
  RGB(0x44, 0x4, 0x44),
  RGB(0xd8, 0xd0, 0xd0),
  RGB(0xc0, 0xb0, 0xb0),
  RGB(0xa4, 0x90, 0x90),
  RGB(0x8c, 0x74, 0x74),
  RGB(0x78, 0x5c, 0x5c),
  RGB(0x68, 0x4c, 0x4c),
  RGB(0x5c, 0x3c, 0x3c),
  RGB(0x48, 0x2c, 0x2c),
  RGB(0xd0, 0xd8, 0xd0),
  RGB(0xb0, 0xc0, 0xb0),
  RGB(0x90, 0xa4, 0x90),
  RGB(0x74, 0x8c, 0x74),
  RGB(0x5c, 0x78, 0x5c),
  RGB(0x4c, 0x68, 0x4c),
  RGB(0x3c, 0x5c, 0x3c),
  RGB(0x2c, 0x48, 0x2c),
  RGB(0xc8, 0xc8, 0xd4),
  RGB(0xb0, 0xb0, 0xc0),
  RGB(0x90, 0x90, 0xa4),
  RGB(0x74, 0x74, 0x8c),
  RGB(0x5c, 0x5c, 0x78),
  RGB(0x4c, 0x4c, 0x68),
  RGB(0x3c, 0x3c, 0x5c),
  RGB(0x2c, 0x2c, 0x48),
  RGB(0xd8, 0xdc, 0xec),
  RGB(0xc8, 0xcc, 0xdc),
  RGB(0xb8, 0xc0, 0xd4),
  RGB(0xa8, 0xb8, 0xcc),
  RGB(0x9c, 0xb0, 0xcc),
  RGB(0x94, 0xac, 0xcc),
  RGB(0x88, 0xa4, 0xcc),
  RGB(0x88, 0x94, 0xdc),
  RGB(0xfc, 0xf0, 0x90),
  RGB(0xfc, 0xe4, 0x60),
  RGB(0xfc, 0xc8, 0x24),
  RGB(0xfc, 0xac, 0xc),
  RGB(0xfc, 0x78, 0x10),
  RGB(0xd0, 0x1c, 0x0),
  RGB(0x98, 0x0, 0x0),
  RGB(0x58, 0x0, 0x0),
  RGB(0x90, 0xf0, 0xfc),
  RGB(0x60, 0xe4, 0xfc),
  RGB(0x24, 0xc8, 0xfc),
  RGB(0xc, 0xac, 0xfc),
  RGB(0x10, 0x78, 0xfc),
  RGB(0x0, 0x1c, 0xd0),
  RGB(0x0, 0x0, 0x98),
  RGB(0x0, 0x0, 0x58),
  RGB(0xfc, 0xc8, 0x64),
  RGB(0xfc, 0xb4, 0x2c),
  RGB(0xec, 0xa4, 0x24),
  RGB(0xdc, 0x94, 0x1c),
  RGB(0xcc, 0x88, 0x18),
  RGB(0xbc, 0x7c, 0x14),
  RGB(0xa4, 0x6c, 0x1c),
  RGB(0x8c, 0x60, 0x24),
  RGB(0x78, 0x54, 0x24),
  RGB(0x60, 0x44, 0x24),
  RGB(0x48, 0x38, 0x24),
  RGB(0x34, 0x28, 0x1c),
  RGB(0x90, 0x68, 0x34),
  RGB(0x90, 0x64, 0x2c),
  RGB(0x94, 0x6c, 0x34),
  RGB(0x94, 0x70, 0x40),
  RGB(0x8c, 0x5c, 0x24),
  RGB(0x90, 0x64, 0x2c),
  RGB(0x90, 0x68, 0x30),
  RGB(0x98, 0x78, 0x4c),
  RGB(0x60, 0x3c, 0x2c),
  RGB(0x54, 0xa4, 0xa4),
  RGB(0xc0, 0x0, 0x0),
  RGB(0xfc, 0x88, 0xe0),
  RGB(0xfc, 0x58, 0x84),
  RGB(0xf4, 0x0, 0xc),
  RGB(0xd4, 0x0, 0x0),
  RGB(0xac, 0x0, 0x0),
  RGB(0xe8, 0xa8, 0xfc),
  RGB(0xe0, 0x7c, 0xfc),
  RGB(0xd0, 0x3c, 0xfc),
  RGB(0xc4, 0x0, 0xfc),
  RGB(0x90, 0x0, 0xbc),
  RGB(0xfc, 0xf4, 0x7c),
  RGB(0xfc, 0xe4, 0x0),
  RGB(0xe4, 0xd0, 0x0),
  RGB(0xa4, 0x98, 0x0),
  RGB(0x64, 0x58, 0x0),
  RGB(0xac, 0xfc, 0xa8),
  RGB(0x74, 0xe4, 0x70),
  RGB(0x0, 0xbc, 0x0),
  RGB(0x0, 0xa4, 0x0),
  RGB(0x0, 0x7c, 0x0),
  RGB(0xac, 0xa8, 0xfc),
  RGB(0x80, 0x7c, 0xfc),
  RGB(0x0, 0x0, 0xfc),
  RGB(0x0, 0x0, 0xbc),
  RGB(0x0, 0x0, 0x7c),
  RGB(0x30, 0x30, 0x50),
  RGB(0x28, 0x28, 0x48),
  RGB(0x24, 0x24, 0x40),
  RGB(0x20, 0x1c, 0x38),
  RGB(0x1c, 0x18, 0x34),
  RGB(0x18, 0x14, 0x2c),
  RGB(0x14, 0x10, 0x24),
  RGB(0x10, 0xc, 0x20),
  RGB(0xa0, 0xa0, 0xb4),
  RGB(0x88, 0x88, 0xa4),
  RGB(0x74, 0x74, 0x90),
  RGB(0x60, 0x60, 0x80),
  RGB(0x50, 0x4c, 0x70),
  RGB(0x40, 0x3c, 0x60),
  RGB(0x30, 0x2c, 0x50),
  RGB(0x24, 0x20, 0x40),
  RGB(0x18, 0x14, 0x30),
  RGB(0x10, 0xc, 0x20),
  RGB(0x14, 0xc, 0x8),
  RGB(0x18, 0x10, 0xc),
  RGB(0x0, 0x0, 0x0),
  RGB(0x0, 0x0, 0x0),
  RGB(0x0, 0x0, 0x0),
  RGB(0x0, 0x0, 0x0),
  RGB(0x0, 0x0, 0x0),
  RGB(0x0, 0x0, 0x0),
  RGB(0x0, 0x0, 0x0),
  RGB(0x0, 0x0, 0x0),
  RGB(0x0, 0x0, 0x0),
  RGB(0x0, 0x0, 0x0),
  RGB(0x0, 0x0, 0x0),
  RGB(0x0, 0x0, 0x0)
};



void Gfx::init()
{
  buffer = new SurfaceWrapper(320, 200);
  canvas = new SurfaceWrapper(320, 200);
  activeBuffer = canvas;
}

void Gfx::deinit()
{
  delete buffer;
  delete canvas;
}


void Gfx::rect(u16 x, u16 y, u16 w, u16 h, u32 color)
{
  lock(canvas);

  for (int i = 0; i < w; ++i)
  {
    canvas->set(x+i, y, color);
    canvas->set(x+i, y+h, color);
  }
  
  for (int i = 0; i < h; ++i)
  {
    canvas->set(x, y+i, color);
    canvas->set(x+w, y+i, color);
  }
  
  unlock(canvas);
}

void Gfx::alphaBlend(const SDL_Rect& rect, Color color)
{
  lock(canvas);
  
  u8 a = (color & 0xFF000000) >> 24;
  u8 r = (color & 0x00FF0000) >> 16;
  u8 g = (color & 0x0000FF00) >> 8;
  u8 b = (color & 0x000000FF);
  
  for (s32 y = 0; y < rect.h; ++y)
  {
    for (s32 x = 0; x < rect.w; ++x)
    {
      if (x+rect.x < canvas->tw() && y+rect.y < canvas->th() && x+rect.x >= 0 && y+rect.y >= 0)
      {
        u32 ps = canvas->at(rect.x+x, rect.y+y);
        
        u8 a2 = (ps & 0xFF000000);
        u8 r2 = (ps & 0x00FF0000) >> 16;
        u8 g2 = (ps & 0x0000FF00) >> 8;
        u8 b2 = (ps & 0x000000FF);
        
        u16 rd = (r * a) + (r2 * (255-a));
        u16 gd = (g * a) + (g2 * (255-a));
        u16 bd = (b * a) + (b2 * (255-a));
        
        u8 rb = (rd+1 + (rd >> 8)) >> 8;
        u8 gb = (gd+1 + (gd >> 8)) >> 8;
        u8 bb = (bd+1 + (bd >> 8)) >> 8;
        
        canvas->set(rect.x+x, rect.y+y, a2 | (rb << 16) | (gb << 8) | (bb));
      }
    }
  }
  
  unlock(canvas);
}

#define ALPHA_SHIFT (1)
void Gfx::rawBlit(const SpriteSheet *gsrc, SpriteSheet *gdst, u16 fx, u16 fy, s16 tx, s16 ty, u16 w, u16 h, u16 r, u16 c)
{
  lock(gsrc);
  lock(gdst);

  for (s32 y = 0; y < h; ++y)
  {
    for (s32 x = 0; x < w; ++x)
    {
      if (x+tx < gdst->tw() && y+ty < gdst->th() && x+tx >= 0 && y+ty >= 0)
      {
        u32 ps = gsrc->at(fx+x, fy+y, r, c);
        
        if (ps == 0xFF00FF00)
        {
          Color pd = gdst->at(tx+x,ty+y);
          
          int r = (pd & 0x00FF0000) >> 16;
          int g = (pd & 0x0000FF00) >> 8;
          int b = (pd & 0x000000FF);
          
          r >>= ALPHA_SHIFT;
          g >>= ALPHA_SHIFT;
          b >>= ALPHA_SHIFT;
          
          gdst->set(tx+x, ty+y, 0xFF000000 | (r << 16) | (g << 8) | b);
        }
        else if (ps == 0xFFFF00FF)
          continue;
        else
        {
          // colorMap present
          if (palette)
            ps = palette->get(ps);
          
          if (map)
            ps = map->get(ps);
          
          u32 sa = ((ps & 0xFF000000) >> 24);
          
          if (sa == 0xFF)
            gdst->set(tx+x, ty+y, ps);
          else if (sa == 0x00)
            continue;
          else
          {
            int da = 256 - sa;
            
            Color pd = gdst->at(tx+x, ty+y);
            int r2 = (pd & 0x00FF0000) >> 16;
            int g2 = (pd & 0x0000FF00) >> 8;
            int b2 = (pd & 0x000000FF);
            
            int r1 = (ps & 0x00FF0000) >> 16;
            int g1 = (ps & 0x0000FF00) >> 8;
            int b1 = (ps & 0x000000FF);
            
            gdst->set(tx+x, ty+y,0xFF000000 | (((r2*da + r1*sa) >> 8) << 16) | (((g2*da + g1*sa) >> 8) << 8) | ((b2*da + b1*sa) >> 8));
          }
        }
      }
    }
  }
  
  unlock(gsrc);
  unlock(gdst);
}

void Gfx::drawPixel(u32 color, u16 x, u16 y)
{
  lock(activeBuffer);
  activeBuffer->set(x, y, color);
  unlock(activeBuffer);
}

void Gfx::drawLine(u32 color, u16 x1, u16 y1, u16 x2, u16 y2)
{
  lock(activeBuffer);
  if (y1 == y2)
  {
    for (u16 x = x1; x < x2; ++x)
      activeBuffer->set(x, y1, color);
  }
  else if (x1 == x2)
  {
    for (u16 y = y1; y < y2; ++y)
      activeBuffer->set(x1, y, color);
  }
  unlock(activeBuffer);
}

void Gfx::resetBuffer(u16 w, u16 h)
{
  lock(buffer);
  for (int yy = 0; yy < h; ++yy)
    for (int xx = 0; xx < w; ++xx)
      buffer->set(xx, yy, 0x00000000);
  unlock(buffer);
}

void Gfx::maskBuffer(TextureID texture, int r, int c)
{
  const Texture* tex = Texture::get(texture);
  
  lock(tex);
  lock(buffer);

  for (int i = 0; i < tex->sw(r,c); ++i)
    for (int j = 0; j < tex->sh(r,c); ++j)
    {
      Color p = tex->at(i,j,c,r);
      
      if ((p & 0xFF000000) == 0)
        buffer->set(i, j, 0x00000000);
    }
  
  unlock(buffer);
  unlock(tex);
}

void Gfx::colorMapBuffer(int w, int h, ColorMap& map)
{
  lock(buffer);
  for (int yy = 0; yy < h; ++yy)
    for (int xx = 0; xx < w; ++xx)
      buffer->set(xx, yy, map.get(buffer->at(xx,yy)));
  
  unlock(buffer);
}

void Gfx::maskBufferWithImage(TextureID mask, TextureID snd, u16 r, u16 c, u16 r2, u16 c2)
{
  const Texture* tex1 = Texture::get(mask), *tex2 = Texture::get(snd);
  
  lock(tex1);
  lock(tex2);
  lock(buffer);
  
  for (int i = 0; i < tex1->sw(r,c); ++i)
    for (int j = 0; j < tex1->sh(r,c); ++j)
    {
      int p = tex1->at(i,j,c,r);
      
      if ((p & 0xFF000000) == 0)
        buffer->set(i, j, tex2->at(i,j,c,r));
    }

  unlock(tex1);
  unlock(tex2);
  unlock(buffer);
}

/*

void Gfx::drawClippedToWidth(TextureID texture, s16 r, s16 c, s16 x, s16 y, s16 t)
{
  const Texture* tex = Texture::get(texture);
  drawClipped(texture, x, y, r*tex->w, c*tex->h, t, tex->h);
}

void Gfx::drawClippedFromWidth(TextureID texture, s16 r, s16 c, s16 x, s16 y, s16 t)
{
  const Texture* tex = Texture::get(texture);
  drawClipped(texture, x, y, r*tex->w + t, c*tex->h, tex->w - t, tex->h);
}

void Gfx::drawClippedFromHeight(TextureID texture, s16 r, s16 c, s16 x, s16 y, s16 t)
{
  const Texture* tex = Texture::get(texture);
  drawClipped(texture, x, y, r*tex->w, c*tex->h + t, tex->w, tex->h - t);
}*/

void Gfx::drawClipped(TextureID texture, s16 x, s16 y, s16 fx, s16 fy, s16 w, s16 h, u16 r, u16 c)
{
  const Texture* tex = Texture::get(texture);
  s16 tw = w != 0 ? (w > 0 ? w : tex->sw(r,c) + w - fx) : tex->sw(r,c) - fx;
  s16 th = h != 0 ? (h > 0 ? h : tex->sh(r,c) + h - fy) : tex->sh(r,c) - fy;
  s16 tx = fx;
  s16 ty = fy;
  s16 dx = x;
  s16 dy = y;
  
  blit(tex, activeBuffer, tx, ty, dx, dy, tw, th, r, c);
}



void Gfx::rawDraw(TextureID texture, u16 r, u16 c, u16 x, u16 y)
{
  const Texture* tex = Texture::get(texture);
  blit(tex, canvas, 0, 0, x, y, tex->sw(r,c), tex->sh(r,c), c, r);
}

void Gfx::draw(TextureID texture, u16 x, u16 y)
{
  const Texture* tex = Texture::get(texture);
  blit(tex, activeBuffer, 0, 0, x, y, tex->sw(), tex->sh(), 0, 0);
}

void Gfx::draw(TextureID texture, u16 i, u16 x, u16 y)
{
  const Texture* tex = Texture::get(texture);
  draw(texture, i / tex->cols, i % tex->cols, x, y);
}

void Gfx::draw(TextureID texture, u16 r, u16 c, s16 x, s16 y)
{
  const Texture* tex = Texture::get(texture);
  
  blit(tex, activeBuffer, 0, 0, x, y, tex->sw(r,c), tex->sh(r,c), r, c);
}

void Gfx::draw(const SpriteSheet* sheet, s16 x, s16 y, u16 r, u16 c)
{
  blit(sheet, activeBuffer, 0, 0, x, y, sheet->sw(r,c), sheet->sh(r,c), r, c);
}

void Gfx::draw(const LBXSpriteInfo& info, s16 x, s16 y, u16 r, u16 c)
{
  draw(LBXRepository::spriteFor(info), x, y, r, c);
}


void Gfx::drawAnimated(TextureID texture, u16 r, u16 x, u16 y, s16 offset)
{
  const Texture* tex = Texture::get(texture);
  
  if (tex->animatedSprites.empty())
    draw(texture, r, (((offset+fticks)/tex->animFactor)%tex->cols), x, y);
  else
    draw(texture, r, (((offset+fticks)/tex->animFactor)%tex->animatedSprites[r]), x, y);
}

void Gfx::draw(const SpriteInfo& info, u16 x, u16 y)
{
  draw(info.texture, info.x, info.y, x, y);
}

void Gfx::drawGlow(const SpriteInfo& info, s16 x, s16 y, School school) { drawGlow(Texture::get(info.texture), x, y, info.x, info.y, school); }

void Gfx::drawGlow(const LBXSpriteInfo& info, s16 x, s16 y, s16 r, s16 c, School color)
{
  drawGlow(LBXRepository::spriteFor(info), x, y, r, c, color);
}

void Gfx::drawGlow(const SpriteSheet* sprite, s16 x, s16 y, s16 r, s16 c, School color)
{
  //TODO: sometimes it looks like it fails (like great drake iso with glow)

  s32 w = sprite->sw(r,c);
  s32 h = sprite->sh(r,c);
  
  resetBuffer((w+2)*2,h+2);
  bindBuffer();
  draw(sprite,1,1,r,c);
  bindCanvas();
  
  lock(buffer);
  const Color* glowColors = MiscMaps::SCHOOL_GLOW_COLORS[color];
  u8 glowLength = MiscMaps::SCHOOL_GLOW_COUNT;

  // TODO: phase is too much linear compared to real one
  int phase = (fticks%50)/3;
  
  for (s16 i = 0; i < w+2; ++i)
  {
    for (s16 j = 0; j < h+2; ++j)
    {
      bool found = false;
      
      for (int k = 0; k < Util::ODIRS_LENGTH; ++k)
      {
        s16 dx = i+Util::ODIRS[k].x, dy = j+Util::ODIRS[k].y;
        
        Color pixel = buffer->at(i,j);
        
        if ((pixel & 0x00FFFFFF) == 0)
        {
          if (dx > 0 && dx < w && dy > 0 && dy < h)
          {
            if ((buffer->at(dx,dy) & 0x00FFFFFF) != 0)
            {
              found = true;
              break;
            }
          }
        }
      }
      
      if (found)
        buffer->set(w+2+i, j, glowColors[(phase+i+j)%glowLength]);
    }
  }
  
  unlock(buffer);
  mergeBuffer(w+2, 0, x-1, y-1, w+2, h+2);
}

void Gfx::drawGlow(TextureID texture, s16 x, s16 y, s16 i, School school)
{
  const Texture* tex = Texture::get(texture);
  drawGlow(tex, x, y, i/tex->cols, i%tex->cols, school);
}

void Gfx::drawGrayScale(TextureID texture, u16 r, u16 c, u16 x, u16 y)
{
  const Texture* tex = Texture::get(texture);
  u16 tw = tex->sw(r,c), th = tex->sh(r,c);
  
  resetBuffer(tw,th);
  bindBuffer();
  draw(texture, r, c, 0, 0);
  
  lock(buffer);
  
  for (s16 xx = 0; xx < tw; ++xx)
    for (s16 yy = 0; yy < th; ++yy)
      buffer->set(xx, yy, MiscMaps::GRAYSCALE.get(buffer->at(xx, yy)));
  
  unlock(buffer);
  mergeBuffer(0, 0, x, y, tw, th);
  bindCanvas();

}

void Gfx::drawGrayScale(const SpriteInfo& info, s16 x, s16 y)
{
  drawGrayScale(info.texture, info.x, info.y, x, y);
}