#include "stopwatch.hpp"

namespace cges {

StopWatch::StopWatch() noexcept
    : start()
    , elapsedTime(std::chrono::microseconds(0)) {}

void StopWatch::Start() noexcept {
  start = std::chrono::system_clock::now();
}

void StopWatch::Stop() noexcept {
  using namespace std::chrono;
  elapsedTime += duration_cast<microseconds>(system_clock::now() - start);
}

void StopWatch::Reset() noexcept {
  using namespace std::chrono;
  elapsedTime = microseconds(0);
}

uint32_t StopWatch::GetMicroseconds() const noexcept {
  using namespace std::chrono;
  return static_cast<uint32_t>(elapsedTime.count());
}

uint32_t StopWatch::GetMilliseconds() const noexcept {
  return GetMicroseconds() / 1000;
}

}// namespace cges