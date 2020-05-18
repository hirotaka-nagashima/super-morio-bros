//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_MOVING_OBJECT_H_
#define SUPER_MORIO_BROS_MOVING_OBJECT_H_

#include <map>
#include <vector>
#include "point.h"
#include "camera.h"
#include "graphic.h"
#include "block.h"
#include "type.h"

class MovingObject {
public:
  MovingObject(Type type, int width, int height, int id_x, int id_y)
      : kDefaultType(type),
        kWidth(width),
        kHeight(height),
        kInitialPosition(Block::kWidth * id_x, Block::kHeight * id_y) {
    Reset();
  }
  virtual ~MovingObject() {}

  void Draw(Graphic* graphic, const Camera& camera) const;
  void Move(const std::map<Point, Block*>& blocks,
            const std::vector<MovingObject*>& moving_objects,
            const Camera& camera, int world_height);

  virtual void Reset() {
    type_ = kDefaultType;
    position_ = kInitialPosition;
    prev_position_ = kInitialPosition;
    force_.x = force_.y = 0;
    hit_state_.Reset();
    is_vanished_ = false;
    is_transparent_ = false;
    direction_ = kLeft;
  }
  virtual void NotifyAttacked() { is_vanished_ = true; }

  Type type() const { return type_; }
  Point position() const { return position_; }
  void disable() { is_vanished_ = true; }

protected:
  enum Direction { kLeft = -1, kRight = 1 };
  struct HitState {
    HitState() { Reset(); }

    bool IsHitting() const { return top || bottom || left || right; }
    void Reset() { top = bottom = left = right = false; }
    void Add(const HitState& hit_state) {
      top |= hit_state.top;
      bottom |= hit_state.bottom;
      left |= hit_state.left;
      right |= hit_state.right;
    }

    bool top, bottom, left, right;
  };
  
  static const Point kGravity;

  virtual void PlaySE(Graphic* graphic) const {}
  virtual int CalculateImageId() const { return type(); }
  virtual void CalculateDirection();
  virtual void CalculateForce() = 0;
  virtual void Act(
      const std::vector<std::pair<Block*, HitState> >& hitting_blocks,
      const std::vector<std::pair<MovingObject*, HitState> >&
          hitting_moving_objects,
      const Camera& camera,
      int world_height) {}

  bool is_in_air() const { return !hit_state_.bottom; }

  const int kWidth;
  const int kHeight;
  Type type_;
  Point position_;
  Point prev_position_;
  Point force_;
  HitState hit_state_;
  bool is_vanished_;
  bool is_transparent_;
  Direction direction_;

private:
  Point SimulateNextPosition() const;
  void UpdateHitState(
      const std::vector<std::pair<Block*, HitState> >& hitting_blocks,
      const std::vector<std::pair<MovingObject*, HitState> >&
          hitting_moving_objects);
  void AdjustPositionToAll(
      const std::vector<std::pair<Block*, HitState> >& hitting_blocks,
      const std::vector<std::pair<MovingObject*, HitState> >&
          hitting_moving_objects,
      Point* position) const;
  void AdjustPosition(const HitState& is_hitting,
                      const Point& target_position,
                      int target_width, int target_height,
                      Point* position) const;
  void DetectHits(
      const Point& next_position,
      const std::map<Point, Block*>& blocks,
      const std::vector<MovingObject*>& moving_objects,
      std::vector<std::pair<Block*, HitState> >* hitting_blocks,
      std::vector<std::pair<MovingObject*, HitState> >*
          hitting_moving_objects) const;
  HitState CheckHit(const Point& next_position,
                    const Point& target_position,
                    int target_width,
                    int target_height) const;

  const Type kDefaultType;
  const Point kInitialPosition;
};

#endif  // SUPER_MORIO_BROS_MOVING_OBJECT_H_