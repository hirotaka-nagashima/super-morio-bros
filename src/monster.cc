//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "monster.h"
#include "global_variables.h"

void Monster::NotifyAttacked() {
  state_ = kAttacked;
  is_transparent_ = true;
}

int Monster::CalculateImageId() const {
  int image_id = type() + state_;
  if (state_ == kWalking) {
    static const int kNumWalkingImages = kAttacked - kWalking;
    static const int kUnitNumFrames = 10;
    static int image_id_offset = 0;
    static int frame_id_latest_updated = 0;
    if (g_frame_id % kUnitNumFrames == 0 &&
        g_frame_id != frame_id_latest_updated) {
      image_id_offset = (1 + image_id_offset) % kNumWalkingImages;
      frame_id_latest_updated = g_frame_id;
    }
    image_id += image_id_offset;
  }

  // Consider a direction.
  if (direction_ == kLeft)
    image_id += kNumStates;

  return image_id;
}

void Monster::CalculateForce() {
  // Calculate force of x.
  static const int kDefaultWalkingVelocityX = 1;
  Point velocity = position_.Subtract(prev_position_);
  force_.x = direction_ * (
      (state_ == kWalking)  ? kDefaultWalkingVelocityX :
      (state_ == kAttacked) ? 0 : 0) - velocity.x;
}