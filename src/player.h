//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_PLAYER_H_
#define SUPER_MORIO_BROS_PLAYER_H_

#include "moving_object.h"

class Command;

class Player : public MovingObject {
public:
  struct Message {
    Message() { Reset(); }

    void Reset() {
      achieved_goal = is_dead =
          got_coin = got_protein = got_potion = got_drug =
          started_jump = stamped_monster = is_attacked = false;
    }

    bool achieved_goal, is_dead;
    bool got_coin, got_protein, got_potion, got_drug;
    bool started_jump, stamped_monster, is_attacked;
  };

  Player(int id_x, int id_y)
      : MovingObject(kNormalPlayer, 32, 32, id_x, id_y) {}

  virtual void Reset() {
    MovingObject::Reset();
    direction_ = kRight;
    command_ = NULL;
    message_.Reset();
  }

  Message message() const { return message_; }
  void set_type(Type type) { type_ = type; }
  void set_command(const Command& command) { command_ = &command; }

private:
  virtual void PlaySE(Graphic* graphic) const;
  virtual int CalculateImageId() const;
  virtual void CalculateDirection();
  virtual void CalculateForce();
  virtual void Act(
      const std::vector<std::pair<Block*, HitState> >& hitting_blocks,
      const std::vector<std::pair<MovingObject*, HitState> >&
          hitting_moving_objects,
      const Camera& camera,
      int world_height);

  void ActForHittingBlock(
      const std::pair<Block*, HitState>& hitting_block);
  void ActForHittingMovingObject(
      const std::pair<MovingObject*, HitState>& hitting_moving_object);
  void Strengthen();
  void Weaken();
  void UpdateTransparency(bool hide_from_now = false, int duration = 0);

  const Command* command_;
  Message message_;
};

#endif  // SUPER_MORIO_BROS_PLAYER_H_