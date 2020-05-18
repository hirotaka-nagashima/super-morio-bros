//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "player.h"
#include <algorithm>  // std::min()
#include "global_variables.h"
#include "command.h"

void Player::PlaySE(Graphic* graphic) const {
  if (message().got_coin)        graphic->PlaySECoin();
  if (message().got_protein)     graphic->PlaySEUp();
  if (message().got_potion)      graphic->PlaySEUp();
  if (message().got_drug)        graphic->PlaySEDown();
  if (message().started_jump)    graphic->PlaySEJump();
  if (message().stamped_monster) graphic->PlaySESmash();
  if (message().is_attacked)     graphic->PlaySEDown();
}

int Player::CalculateImageId() const {
  int image_id = type();

  Point velocity = position_.Subtract(prev_position_);
  if (is_in_air()) {             // Jumping.
    image_id += 3;
  } else if (velocity.x != 0) {  // Walking.
    // Calculate the unit number of frames.
    static const int kMaxVelocityX = 6;
    int unit_num_frames = 2 + std::max(0, kMaxVelocityX - abs(velocity.x));

    // Switch images as needed.
    static const int kNumWalkingImages = 2;
    static int image_id_offset = 0;
    static int frame_id_latest_updated = 0;
    if (g_frame_id % unit_num_frames == 0 &&
        g_frame_id != frame_id_latest_updated) {
      image_id_offset = (1 + image_id_offset) % kNumWalkingImages;
      frame_id_latest_updated = g_frame_id;
    }
    image_id += image_id_offset;
  }

  // Consider a direction.
  static const int kNumStates = 4;
  if (direction_ == kLeft)
    image_id += kNumStates;

  // Consider a transparency.
  if (is_transparent_)
    image_id += 10;

  return image_id;
}

void Player::CalculateDirection() {
  if (command_->left() == command_->right())
    return;

  // Determine a direction according to the latest input.
  bool a = (command_->left() == 0 || command_->right() == 0);
  bool b = (command_->left() < command_->right());
  direction_ = ((a && b) || (!a && !b)) ? kRight : kLeft;
}

void Player::CalculateForce() {
  message_.started_jump = false;

  Point velocity = position_.Subtract(prev_position_);

  // Calculate force of x.
  static const int kInertia = 6;
  static const int kRunningMaxVelocityX = 6;
  static const int kWalkingMaxVelocityX = 4;
  int force_x = (g_frame_id % kInertia == 0) ? 1 : 0;
  int max_velocity_x = (0 < command_->dash()) ? kRunningMaxVelocityX :
                                               kWalkingMaxVelocityX;
  bool is_accelerateable = (abs(velocity.x) <= max_velocity_x);
  bool is_key_typed = (command_->left() != command_->right());
  bool is_forced_to_move = is_accelerateable && is_key_typed;
  bool is_turning_back = (velocity.x != 0 &&
                          direction_ != velocity.x / abs(velocity.x));
  force_.x = is_forced_to_move ?
      (is_turning_back ? 2 : 1) * direction_ * force_x :
      (velocity.x == 0) ? 0 : (0 < velocity.x) ? -force_x : force_x;

  // Calculate force of y.
  static const int kMaxFramesJump = 15;
  static const int kDefaultElasticity = -9;
  int elasticity = kDefaultElasticity - velocity.x / kWalkingMaxVelocityX;
  message_.started_jump |= (command_->jump() == 1 && !is_in_air());
  is_key_typed = (1 < command_->jump() && command_->jump() < kMaxFramesJump);
  bool is_key_typed_continuously = (is_key_typed && force_.y != 0);
  bool is_during_jump = is_key_typed_continuously && is_in_air();
  force_.y = message().stamped_monster ? elasticity - velocity.y :
             message().started_jump    ? elasticity :
             is_during_jump            ? -kGravity.y : 0;
}

void Player::Act(
    const std::vector<std::pair<Block*, HitState> >& hitting_blocks,
    const std::vector<std::pair<MovingObject*, HitState> >&
        hitting_moving_objects,
    const Camera& camera,
    int world_height) {
  message_.achieved_goal = message_.is_dead = message_.got_coin =
      message_.got_protein = message_.got_potion = message_.got_drug =
      message_.stamped_monster = message_.is_attacked = false;

  UpdateTransparency();

  // Adjust the position.
  // Right edge is shifted +kWidth for goal effects.
  position_.x = std::max(position_.x, camera.left());
  position_.x = std::min(position_.x, camera.right() + kWidth);

  message_.achieved_goal |= (camera.right() + kWidth <= position_.x);
  message_.is_dead |= (world_height <= position_.y);

  for (auto hitting_block : hitting_blocks)
    ActForHittingBlock(hitting_block);
  for (auto hitting_moving_object : hitting_moving_objects)
    ActForHittingMovingObject(hitting_moving_object);
}

void Player::ActForHittingBlock(
    const std::pair<Block*, HitState>& hitting_block) {
  HitState is_hitting = hitting_block.second;
  Block* target = hitting_block.first;

  // Pushed out an item.
  if (is_hitting.top) {
    target->PushOutItem();
    force_.y = 0;                    // Terminate a jump.
    prev_position_.y = position_.y;  // Stop vertically.
  }
}

void Player::ActForHittingMovingObject(
    const std::pair<MovingObject*, HitState>& hitting_moving_object) {
  HitState is_hitting = hitting_moving_object.second;
  MovingObject* target = hitting_moving_object.first;

  // Hit against a monster.
  if (IsMonster(target->type())) {
    if (is_hitting.bottom) {  // Stamping the monster.
      target->NotifyAttacked();
      message_.stamped_monster = true;
    } else {                  // Attacked by the monster.
      Weaken();
    }
  }

  // Got an item.
  if (IsItem(target->type())) {
    target->NotifyAttacked();
    static const int kDrugDuration = 400;
    switch (target->type()) {
      case kCoin:    message_.got_coin = true;                  break;
      case kProtein: message_.got_protein = true;
                     Strengthen();                              break;
      case kPotion:  message_.got_potion = true;                break;
      case kDrug:    message_.got_drug = true;
                     UpdateTransparency(true, kDrugDuration);   break;
    }
  }
}

void Player::Strengthen() {
  type_ = kFirePlayer;
}

void Player::Weaken() {
  if (type() == kNormalPlayer) {
    message_.is_dead = true;
  } else {
    message_.is_attacked = true;
    type_ = kNormalPlayer;

    // Make transparent to avoid next attack by same monster.
    static const int kAvoidingDuration = 100;
    UpdateTransparency(true, kAvoidingDuration);

    // Stop horizontally.
    prev_position_.x = position_.x;
  }
}

void Player::UpdateTransparency(bool hide_from_now, int duration) {
  // Hide.
  static int end_frame_id = g_frame_id;
  if (hide_from_now) {
    is_transparent_ = true;
    end_frame_id = g_frame_id + duration;
  }

  // Show.
  if (end_frame_id <= g_frame_id)
    is_transparent_ = false;
}