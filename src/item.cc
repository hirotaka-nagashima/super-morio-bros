//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "item.h"

void Item::CalculateForce() {
  // Calculate force of x.
  static const int kMovingVelocityX = 1;
  Point velocity = position_.Subtract(prev_position_);
  force_.x = direction_ * (state_ == kMoving ? kMovingVelocityX : 0) -
      velocity.x;

  // Calculate force of y.
  force_.y = 0;
  if (state_ == kPushedOut) {
    static const int kPushedOutForceY = -15;
    force_.y = kPushedOutForceY;
    state_ = kMoving;
  }
}