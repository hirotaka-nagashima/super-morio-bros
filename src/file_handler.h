//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_FILE_HANDLER_H_
#define SUPER_MORIO_BROS_FILE_HANDLER_H_

#include "window.h"

enum Status;
class World;
class BestScores;

class FileHandler {
public:
  Status LoadWorld(int world_id, World** world) const;
  Status LoadBestScores(BestScores* scores) const;
  void SaveBestScores(const BestScores& scores) const;

private:
  static const char kWorldsUrlBeforeID[];
  static const char kWorldsUrlAfterID[];
  static const char kScoresUrl[];

  Uint32 GetPixelAddress(SDL_Surface* surface,
                         const int x, const int y) const;
  bool IsSameColor(const SDL_Color& color,
                   Uint8 red, Uint8 green, Uint8 blue) const {
    return (color.r == red && color.g == green && color.b == blue);
  }
};

#endif  // SUPER_MORIO_BROS_FILE_HANDLER_H_