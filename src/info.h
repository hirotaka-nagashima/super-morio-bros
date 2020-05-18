//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_INFO_H_
#define SUPER_MORIO_BROS_INFO_H_

#include "type.h"

class Info {
public:
  Info(int default_num_lives, int default_time)
      : kDefaultNumLives(default_num_lives),
        kDefaultTime(default_time) {
    Reset();
  }

  void Reset() {
    score_ = 0;
    num_coins_ = 0;
    num_lives_ = kDefaultNumLives;
    remaining_time_ = kDefaultTime;
    player_type_ = kNormalPlayer;
  }
  void AddScore(int add_score) { score_ += add_score; }
  void IncrementCoin() {
    if (kMaxNumCoins <= ++num_coins_) {
      IncrementLife();
      num_coins_ = 0;
    }
    AddScore(200);
  }
  void IncrementLife() { ++num_lives_; }
  void DecrementLife() { --num_lives_; }
  void ResetTime() { remaining_time_ = kDefaultTime; }
  // Return true if a player is dead.
  bool DecrementTime() { return (--remaining_time_ <= 0); }
  void Strengthen() { player_type_ = kFirePlayer; AddScore(1000); }
  void Weaken() { player_type_ = kNormalPlayer; }
  void ConvertTimeToScore() {
    AddScore(remaining_time() * 50);
    remaining_time_ = 0;
  }

  int score() const { return score_; }
  int num_coins() const { return num_coins_; }
  int num_lives() const { return num_lives_; }
  int remaining_time() const { return remaining_time_; }
  Type player_type() const { return player_type_; }

private:
  static const int kMaxNumCoins = 100;

  const int kDefaultNumLives;
  const int kDefaultTime;
  int score_;
  int num_coins_;
  int num_lives_;
  int remaining_time_;
  Type player_type_;
};

#endif  // SUPER_MORIO_BROS_INFO_H_