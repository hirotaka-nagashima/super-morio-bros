//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS
#include "file_handler.h"
#include "status.h"
#include "world.h"
#include "best_scores.h"
#include "block.h"
#include "player.h"
#include "monster.h"
#include "turtle_monster.h"
#include "item.h"

const char FileHandler::kWorldsUrlBeforeID[] = "resources/world_";
const char FileHandler::kWorldsUrlAfterID[] = ".bmp";
const char FileHandler::kScoresUrl[] = "resources/scores.dat";

Status FileHandler::LoadWorld(int world_id, World** world) const {
  // Open a world file.
  char url[256];
  sprintf(url, "%s%d%s", kWorldsUrlBeforeID, world_id, kWorldsUrlAfterID);
  SDL_Surface* image_world = SDL_LoadBMP(url);
  if (!image_world)
    return kError;

  const int kWidth = image_world->w;
  const int kHeight = image_world->h;
  Player* player = NULL;
  std::map<Point, Block*> blocks;
  std::vector<MovingObject*> moving_objects;

  // Load a world.
  SDL_LockSurface(image_world);
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      // Get color of a pixel.
      Uint8 index = GetPixelAddress(image_world, x, y);
      const SDL_Color kColor = image_world->format->palette->colors[index];
      if (IsSameColor(kColor, 255, 255, 255))  // Nothing.
        continue;

      // Create a object corresponding to the color.
      Point position(x, y);
      if (IsSameColor(kColor, 224, 96, 0)) {            // Bricks.
        blocks[position] = new Block(kBricks, x, y);
      } else if (IsSameColor(kColor, 255, 255, 0)) {    // Coin.
        std::vector<Item*> items;
        items.push_back(new Item(kCoin, x, y));
        moving_objects.push_back(items[0]);
        blocks[position] = new Block(kBox, x, y, items);
      } else if (IsSameColor(kColor, 0, 192, 0)) {      // Protein.
        std::vector<Item*> items;
        items.push_back(new Item(kProtein, x, y));
        moving_objects.push_back(items[0]);
        blocks[position] = new Block(kBox, x, y, items);
      } else if (IsSameColor(kColor, 160, 160, 0)) {    // Coins.
        static const int kMaxNumCoins = 10;
        const int kNumCoins = rand() % kMaxNumCoins;
        std::vector<Item*> items;
        for (int i = 0; i < kNumCoins; ++i) {
          items.push_back(new Item(kCoin, x, y));
          moving_objects.push_back(items[i]);
        }
        blocks[position] = new Block(kBricks, x, y, items);
      } else if (IsSameColor(kColor, 224, 0, 128)) {    // Drug.
        std::vector<Item*> items;
        items.push_back(new Item(kDrug, x, y));
        moving_objects.push_back(items[0]);
        blocks[position] = new Block(kBox, x, y, items);
      } else if (IsSameColor(kColor, 0, 0, 192)) {      // Potion.
        std::vector<Item*> items;
        items.push_back(new Item(kPotion, x, y));
        moving_objects.push_back(items[0]);
        blocks[position] = new Block(kInvisible, x, y, items);
      } else if (IsSameColor(kColor, 0, 255, 0)) {      // Player.
        if (!player)
          player = new Player(x, y);
      } else if (IsSameColor(kColor, 255, 0, 0)) {      // Monster.
        moving_objects.push_back(new Monster(x, y));
      } else if (IsSameColor(kColor, 0, 0, 255)) {      // Turtle monster.
        moving_objects.push_back(new TurtleMonster(x, y));
      } else if (IsSameColor(kColor, 128, 128, 128)) {  // Stone.
        blocks[position] = new Block(kStone, x, y);
      } else {                                          // Cracked Stone.
        blocks[position] = new Block(kCrackedStone, x, y);
      }
    }
  }
  SDL_UnlockSurface(image_world);
  SDL_FreeSurface(image_world);

  *world = new World(world_id, kWidth, kHeight,
                     player, blocks, moving_objects);
  return kSuccess;
}

Status FileHandler::LoadBestScores(BestScores* scores) const {
  FILE* scores_file = fopen(kScoresUrl, "r");
  if (scores_file == NULL)
    return kError;

  // To set an upper limit on the number of loaded scores,
  // we prevent stack overflow.
  for (int i = 0; !feof(scores_file); ++i) {
    int score;
    fscanf(scores_file, "%d ", &score);
    scores->AddBestScore(score);
  }

  fclose(scores_file);
  return kSuccess;
}

void FileHandler::SaveBestScores(const BestScores& scores) const {
  FILE* scores_file = fopen(kScoresUrl, "w");
  for (int i = 0; i < scores.num_best_scores(); ++i)
    fprintf(scores_file, "%d ", scores.best_score(i + 1));
  fclose(scores_file);
}

Uint32 FileHandler::GetPixelAddress(SDL_Surface* surface,
                                    const int x, const int y) const {
  Uint8 bytes_per_pixel = surface->format->BytesPerPixel;
  Uint8* p = static_cast<Uint8*>(surface->pixels) +
      y * surface->pitch + x * bytes_per_pixel;

  switch (bytes_per_pixel) {
    case 1:
      return *p;
    case 2:
      return *reinterpret_cast<Uint16*>(p);
    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return p[0] << 16 | p[1] << 8 | p[2];
      else
        return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
      return *reinterpret_cast<Uint32*>(p);
    default:
      return 0;
  }
}