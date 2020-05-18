//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_WORLD_H_
#define SUPER_MORIO_BROS_WORLD_H_

#include <map>
#include <vector>
#include "point.h"
#include "player.h"
#include "block.h"

class MovingObject;
class Camera;
class Graphic;

class World {
public:
  World(int id, int width, int height, Player* player,
        const std::map<Point, Block*>& blocks,
        const std::vector<MovingObject*>& moving_objects)
      : kID(id),
        kWidth(Block::kWidth * width),
        kHeight(Block::kHeight * height),
        player_(player),
        blocks_(blocks),
        moving_objects_(moving_objects) {}
  ~World();

  void Draw(const Camera& camera, Graphic* graphic) const;
  Player::Message PlayIn(const Command& command, Camera* camera);
  void Reset(Type succeeded_player_type);

  int id() const { return kID; }
  double progress() const { return 1.0 * player_->position().x / kWidth; }

private:
  bool IsInPreloadRadius(const Camera& camera, int object_x) const;

  const int kID;
  const int kWidth;
  const int kHeight;
  Player* player_;
  std::map<Point, Block*> blocks_;
  std::vector<MovingObject*> moving_objects_;
};

#endif  // SUPER_MORIO_BROS_WORLD_H_