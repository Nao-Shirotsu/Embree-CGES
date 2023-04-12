#pragma once

#include <chrono>
#include <cstdint>

namespace cges {

class StopWatch {
public:
  StopWatch() noexcept;
  void Start() noexcept;
  void Stop() noexcept;
  void Reset() noexcept;
  uint32_t GetMicroseconds() const noexcept;
  uint32_t GetMilliseconds() const noexcept;

private:
  std::chrono::system_clock::time_point start;
  std::chrono::microseconds elapsedTime;
};

} // namespace cges