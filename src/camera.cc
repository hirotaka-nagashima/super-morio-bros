//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "camera.h"
#include <algorithm>  // std::min()
#include "block.h"    // Block::kWidth

void Camera::SlideHorizontally(int player_x, int world_width) {
  if (kScreenWidth / 2 < dx(player_x)) {
    // Slide to right.
    int slided_target_x = player_x - kScreenWidth / 2;
    int max_target_x = world_width - kScreenWidth - Block::kWidth;
    target_.x = std::min(slided_target_x, max_target_x);
  } else if (dx(player_x) < 0) {
    // Slide to left.
    target_.x = player_x;
  }
}