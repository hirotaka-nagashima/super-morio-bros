//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "moving_object.h"
#include <algorithm>  // std::min()

const Point MovingObject::kGravity(0, 1);

void MovingObject::Draw(Graphic* graphic, const Camera& camera) const {
  if (is_vanished_)
    return;

  PlaySE(graphic);
  int image_id = CalculateImageId();
  graphic->DrawObject(image_id, camera.dp(position()), kWidth, kHeight);
}

void MovingObject::Move(const std::map<Point, Block*>& blocks,
                        const std::vector<MovingObject*>& moving_objects,
                        const Camera& camera, int world_height) {
  if (is_vanished_)
    return;

  // Calculate a next position.
  CalculateDirection();
  CalculateForce();
  Point next_position = SimulateNextPosition();

  // Detect hits.
  std::vector<std::pair<Block*, HitState> > hitting_blocks;
  std::vector<std::pair<MovingObject*, HitState> > hitting_moving_objects;
  DetectHits(next_position, blocks, moving_objects,
             &hitting_blocks, &hitting_moving_objects);
  UpdateHitState(hitting_blocks, hitting_moving_objects);

  // Adjust the next position.
  AdjustPositionToAll(hitting_blocks, hitting_moving_objects, &next_position);

  // Update the position.
  prev_position_ = position_;
  position_ = next_position;

  Act(hitting_blocks, hitting_moving_objects, camera, world_height);
}

void MovingObject::CalculateDirection() {
  direction_ = hit_state_.right && !hit_state_.left ? kLeft :
               hit_state_.left && !hit_state_.right ? kRight : direction_;
}

Point MovingObject::SimulateNextPosition() const {
  // Consider gravity and calculate next position.
  static const int kTerminalVelocityY = 9;
  Point velocity = position_.Subtract(prev_position_);
  Point movement = velocity.Add(force_).Add(kGravity);
  movement.y = std::min(movement.y, kTerminalVelocityY);
  return position_.Add(movement);
}

void MovingObject::UpdateHitState(
    const std::vector<std::pair<Block*, HitState> >& hitting_blocks,
    const std::vector<std::pair<MovingObject*, HitState> >&
        hitting_moving_objects) {
  hit_state_.Reset();
  for (auto hitting_block : hitting_blocks) {
    HitState is_hitting = hitting_block.second;
    hit_state_.Add(is_hitting);
  }
  for (auto hitting_moving_object : hitting_moving_objects) {
    HitState is_hitting = hitting_moving_object.second;
    hit_state_.Add(is_hitting);
  }
}

void MovingObject::AdjustPositionToAll(
    const std::vector<std::pair<Block*, HitState> >& hitting_blocks,
    const std::vector<std::pair<MovingObject*, HitState> >&
        hitting_moving_objects,
    Point* position) const {
  for (auto hitting_block : hitting_blocks) {
    Block* target = hitting_block.first;
    HitState is_hitting = hitting_block.second;
    AdjustPosition(is_hitting, target->position(),
                   target->kWidth, target->kHeight, position);
  }
  for (auto hitting_moving_object : hitting_moving_objects) {
    MovingObject* target = hitting_moving_object.first;
    if (IsPlayer(type()) && IsItem(target->type()))
      continue;

    HitState is_hitting = hitting_moving_object.second;
    AdjustPosition(is_hitting, target->position(),
                   target->kWidth, target->kHeight, position);
  }
}

void MovingObject::AdjustPosition(const HitState& is_hitting,
                                  const Point& target_position,
                                  int target_width, int target_height,
                                  Point* position) const {
  int target_top = target_position.y;
  int target_bottom = target_top + target_height;
  int target_left = target_position.x;
  int target_right = target_left + target_width;
  if (is_hitting.top)    position->y = target_bottom;
  if (is_hitting.bottom) position->y = target_top - kHeight;
  if (is_hitting.left)   position->x = target_right;
  if (is_hitting.right)  position->x = target_left - kWidth;
}

void MovingObject::DetectHits(
    const Point& next_position,
    const std::map<Point, Block*>& blocks,
    const std::vector<MovingObject*>& moving_objects,
    std::vector<std::pair<Block*, HitState> >* hitting_blocks,
    std::vector<std::pair<MovingObject*, HitState> >*
        hitting_moving_objects) const {
  // Vanished object hits nothing.
  if (is_vanished_) return;

  // Against blocks.
  for (auto block = blocks.begin(); block != blocks.end(); ++block) {
    Block* target = block->second;
    HitState hit_state = CheckHit(next_position, target->position(),
                                  target->kWidth, target->kHeight);
    if (hit_state.IsHitting())
      hitting_blocks->push_back(std::make_pair(target, hit_state));
  }

  // Transparent object hits only blocks. 
  if (is_transparent_) return;
  
  // Against moving objects such as items, monsters.
  for (auto target : moving_objects) {
    if (target->is_vanished_ || target->is_transparent_ || target == this)
      continue;
    if (IsItem(type()) && IsItem(target->type()))
      continue;

    HitState hit_state = CheckHit(next_position, target->position(),
                                  target->kWidth, target->kHeight);
    if (hit_state.IsHitting())
      hitting_moving_objects->push_back(std::make_pair(target, hit_state));
  }
}

MovingObject::HitState MovingObject::CheckHit(const Point& next_position,
                                              const Point& target_position,
                                              int target_width,
                                              int target_height) const {
  // http://monooki.ldblog.jp/archives/35959913.html
  Point s(position_);
  Point e(position_);
  e.x += kWidth; e.y += kHeight;
  Point sn(next_position);
  Point en(next_position);
  en.x += kWidth; en.y += kHeight;
  Point s2(target_position);
  Point e2(target_position);
  e2.x += target_width; e2.y += target_height;

  Point ma(e.x - s2.x, e.y - s2.y);
  Point na(e2.x - s.x, e2.y - s.y);
  Point man(en.x - s2.x, en.y - s2.y);
  Point nan(e2.x - sn.x, e2.y - sn.y);

  // Four corners.
  // NOTE: Without these codes, worked successfully.
  //if (ma.x <= 0 && ma.y <= 0 && 0 < man.x && 0 < man.y)
  //  (man.y <= man.x) ? hit_state.bottom : hit_state.right = true;
  //if (na.x <= 0 && ma.y <= 0 && 0 < nan.x && 0 < man.y)
  //  (man.y <= nan.x) ? hit_state.bottom : hit_state.left = true;
  //if (ma.x <= 0 && na.y <= 0 && 0 < man.x && 0 < nan.y)
  //  (nan.y <= man.x) ? hit_state.top : hit_state.right = true;
  //if (na.x <= 0 && na.y <= 0 && 0 < nan.x && 0 < nan.y)
  //  (nan.y <= nan.x) ? hit_state.top : hit_state.left = true;

  // Four directions.
  HitState hit_state;
  if (0 < ma.x && 0 < na.x) {
    hit_state.top |= (0 < nan.y && s2.y < s.y);
    hit_state.bottom |= (0 < man.y && e.y < e2.y);
  }
  if (0 < ma.y && 0 < na.y) {
    hit_state.left |= (0 < nan.x && s2.x < s.x);
    hit_state.right |= (0 < man.x && e.x < e2.x);
  }

  return hit_state;
}