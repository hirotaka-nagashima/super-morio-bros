//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_GRAPHIC_H_
#define SUPER_MORIO_BROS_GRAPHIC_H_

#include "window.h"

class Info;
class BestScores;
class World;
class Camera;

class Graphic {
public:
  Graphic()
      : window_(512, 480, "SUPER MORIO BROS.", "resources/icon.png"),
        font_(NULL),
        bgm_(NULL),
        se_jump_(NULL),
        se_coin_(NULL),
        se_smash_(NULL),
        se_up_(NULL),
        se_down_(NULL),
        image_objects_(NULL),
        image_infobar_(NULL),
        image_title_(NULL),
        image_blood_(NULL),
        image_foreground_(NULL) {
    memset(image_background_, NULL, sizeof(image_background_));
  }

  Status Initialize();
  void Terminate();
  void DrawObject(int image_id, const Point& dest,
                  int image_width, int image_height);
  Status DisplayObjects(const Camera& camera, const World& world,
                        const Info& info, int fps);
  Status DisplayTitle(const Info& info);
  Status DisplayWorldInfo(int world_id, const Info& info);
  Status DisplayBlood();
  Status DisplayGameOver();
  Status DisplayResult(int score, const BestScores& best_scores);
  void PlaySEJump() { window_.PlaySEOnce(se_jump_); }
  void PlaySECoin() { window_.PlaySEOnce(se_coin_); }
  void PlaySESmash() { window_.PlaySEOnce(se_smash_); }
  void PlaySEUp() { window_.PlaySEOnce(se_up_); }
  void PlaySEDown() { window_.PlaySEOnce(se_down_); }

  int width() { return window_.width(); }
  int height() { return window_.height(); }

private:
  static const int kNumLayersBackground = 4;

  void DrawInfoBar(const Info& info);
  void DrawForeground(const Camera& camera);
  void DrawBackground(const Camera& camera);
  void StabilizeFPS(int fps) const;

  Window window_;
  // Resources.
  TTF_Font* font_;
  Mix_Music* bgm_;
  Mix_Chunk* se_jump_;
  Mix_Chunk* se_coin_;
  Mix_Chunk* se_smash_;
  Mix_Chunk* se_up_;
  Mix_Chunk* se_down_;
  SDL_Surface* image_objects_;
  SDL_Surface* image_infobar_;
  SDL_Surface* image_title_;
  SDL_Surface* image_blood_;
  SDL_Surface* image_foreground_;
  SDL_Surface* image_background_[kNumLayersBackground];
};

#endif  // SUPER_MORIO_BROS_GRAPHIC_H_