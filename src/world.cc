//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "world.h"
#include "moving_object.h"
#include "camera.h"

World::~World() {
  delete player_;
  player_ = NULL;

  // Free blocks.
  for (auto block : blocks_) {
    delete block.second;
    block.second = NULL;
  }

  // Free characters and items.
  for (auto moving_object : moving_objects_) {
    delete moving_object;
    moving_object = NULL;
  }
}

void World::Draw(const Camera& camera, Graphic* graphic) const {
  // Draw blocks and characters, items.
  player_->Draw(graphic, camera);
  for (auto block : blocks_) {
    if (IsInPreloadRadius(camera, block.second->position().x))
      block.second->Draw(graphic, camera);
  }
  for (auto moving_object : moving_objects_) {
    if (IsInPreloadRadius(camera, moving_object->position().x))
      moving_object->Draw(graphic, camera);
  }
}

Player::Message World::PlayIn(const Command& command, Camera* camera) {
  // Move characters and items.
  player_->set_command(command);
  player_->Move(blocks_, moving_objects_, *camera, kHeight);
  for (auto moving_object : moving_objects_) {
    if (IsInPreloadRadius(*camera, moving_object->position().x))
      moving_object->Move(blocks_, moving_objects_, *camera, kHeight);
  }

  // Camera tracks the player.
  camera->SlideHorizontally(player_->position().x, kWidth);
  
  return player_->message();
}

void World::Reset(Type succeeded_player_type) {
  // Reset parameters of blocks and characters, items.
  player_->Reset();
  for (auto block : blocks_)
    block.second->Reset();
  for (auto moving_object : moving_objects_)
    moving_object->Reset();

  player_->set_type(succeeded_player_type);
}

bool World::IsInPreloadRadius(const Camera& camera, int object_x) const {
  int dx_from_center = camera.dx(object_x) - camera.screen_width() / 2;
  const int kRadius = camera.screen_width();
  return -kRadius < dx_from_center && dx_from_center < kRadius;
}