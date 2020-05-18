//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_GAME_H_
#define SUPER_MORIO_BROS_GAME_H_

#include "graphic.h"
#include "file_handler.h"

class Game {
public:
  static const int kFPS = 60;
  static const int kDefaultNumLives = 3;
  static const int kDefaultRemainingTime = 300;

  void Play();

private:
  Status PlayTillGameOver(BestScores* best_scores);
  Status PlayInWorld(int world_id, Info* info);
  Status PlayWithLife(World* world, Info* info);

  Graphic graphic_;
  FileHandler file_handler_;
};

#endif  // SUPER_MORIO_BROS_GAME_H_
