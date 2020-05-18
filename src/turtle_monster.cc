//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "turtle_monster.h"
#include "global_variables.h"

void TurtleMonster::NotifyAttacked() {
  bool is_moving = (state_ == kWalking || state_ == kAttacking);
  state_ = is_moving ? kGuarding : kAttacking;
}

int TurtleMonster::CalculateImageId() const {
  int image_id = type() + state_;
  if (state_ == kWalking) {
    static const int kNumWalkingImages = kGuarding - kWalking;
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

void TurtleMonster::CalculateForce() {
  // Calculate force of x.
  static const int kDefaultWalkingVelocityX = 1;
  static const int kDefaultAttackingVelocityX = 10;
  Point velocity = position_.Subtract(prev_position_);
  force_.x = direction_ * (
      (state_ == kWalking)   ? kDefaultWalkingVelocityX :
      (state_ == kGuarding)  ? 0 :
      (state_ == kAttacking) ? kDefaultAttackingVelocityX : 0) - velocity.x;
}