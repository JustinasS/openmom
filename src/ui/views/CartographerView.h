//
//  CartographerView.h
//  OpenMoM
//
//  Created by Jack on 7/28/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#pragma once

#include "common/Common.h"
#include "View.h"

class CartographerView : public View
{
private:
  Plane plane;
  
  void draw() override;
  void drawPost() override { }
  
public:
  CartographerView(ViewManager* gvm);
  
  void activate() override;
  void deactivate() override { }
};
