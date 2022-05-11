// Copyright 2016 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NINJA_STATUS_H_
#define NINJA_STATUS_H_

#include <map>
#include <string>

#include "build.h"
#include "line_printer.h"

/// Abstract interface to object that tracks the status of a build:
/// completion fraction, printing updates.
struct Status {
  virtual void PlanHasTotalEdges(int total) = 0;
  virtual void BuildEdgeStarted(const Edge* edge, int64_t start_time_millis) = 0;
  virtual void BuildEdgeFinished(Edge* edge, int64_t end_time_millis,
                                 bool success, const std::string& output) = 0;
  virtual void BuildLoadDyndeps() = 0;
  virtual void BuildStarted() = 0;
  virtual void BuildFinished() = 0;

  virtual void Info(const char* msg, ...) = 0;
  virtual void Warning(const char* msg, ...) = 0;
  virtual void Error(const char* msg, ...) = 0;

  virtual ~Status() { }
};

/// Implementation of the Status interface that prints the status as
/// human-readable strings to stdout
struct StatusPrinter : Status {
  explicit StatusPrinter(const BuildConfig& config);
  virtual void PlanHasTotalEdges(int total);
  virtual void BuildEdgeStarted(const Edge* edge, int64_t start_time_millis);
  virtual void BuildEdgeFinished(Edge* edge, int64_t end_time_millis,
                                 bool success, const std::string& output);
  virtual void BuildLoadDyndeps();
  virtual void BuildStarted();
  virtual void BuildFinished();

  virtual void Info(const char* msg, ...);
  virtual void Warning(const char* msg, ...);
  virtual void Error(const char* msg, ...);

  virtual ~StatusPrinter() { }

  /// Format the progress status string by replacing the placeholders.
  /// See the user manual for more information about the available
  /// placeholders.
  /// @param progress_status_format The format of the progress status.
  /// @param status The status of the edge.
  std::string FormatProgressStatus(const char* progress_status_format,
                                   int64_t time_millis) const;

 private:
  void PrintStatus(const Edge* edge, int64_t time_millis);

  const BuildConfig& config_;

  int started_edges_, finished_edges_, total_edges_, running_edges_;
  int64_t time_millis_;

  /// Prints progress output.
  LinePrinter printer_;

  /// The custom progress status format to use.
  const char* progress_status_format_;

  template<size_t S>
  void SnprintfRate(double rate, char(&buf)[S], const char* format) const {
    if (rate == -1)
      snprintf(buf, S, "?");
    else
      snprintf(buf, S, format, rate);
  }

  struct SlidingRateInfo {
    SlidingRateInfo(int n) : rate_(-1), N(n), last_update_(-1) {}

    double rate() { return rate_; }

    void UpdateRate(int update_hint, int64_t time_millis_) {
      if (update_hint == last_update_)
        return;
      last_update_ = update_hint;

      if (times_.size() == N)
        times_.pop();
      times_.push(time_millis_);
      if (times_.back() != times_.front())
        rate_ = times_.size() / ((times_.back() - times_.front()) / 1e3);
    }

  private:
    double rate_;
    const size_t N;
    std::queue<double> times_;
    int last_update_;
  };

  mutable SlidingRateInfo current_rate_;
};

#endif // NINJA_STATUS_H_
