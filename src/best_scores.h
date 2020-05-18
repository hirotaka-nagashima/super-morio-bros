//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_BEST_SCORES_H_
#define SUPER_MORIO_BROS_BEST_SCORES_H_

#include <vector>
#include <algorithm>   // std::sort()
#include <functional>  // std::greater<>()

// Sorted scores.
class BestScores {
public:
  void UpdateBestScores(int score) {
    AddBestScore(score);
    best_scores_.pop_back();  // Delete the worst score.
  }
  void AddBestScore(int best_score) {
    best_scores_.push_back(best_score);
    std::sort(best_scores_.begin(), best_scores_.end(), std::greater<int>());
  }

  int best_score(int place) const { return best_scores_[place - 1]; }
  int num_best_scores() const { return best_scores_.size(); }

private:
  std::vector<int> best_scores_;
};

#endif  // SUPER_MORIO_BROS_BEST_SCORES_H_
