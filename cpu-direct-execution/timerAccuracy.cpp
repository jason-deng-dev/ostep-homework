#define _GNU_SOURCE
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <limits>
#include <vector>
#include <fcntl.h>
#include <cstdio>

void getTimerAccuracy() {
  const int M = 100000;
  std::vector<long long> deltas;
  deltas.reserve(M);
  for (int i = 0; i < M; ++i) {
    auto t1 = std::chrono::steady_clock::now();
    auto t2 = std::chrono::steady_clock::now();
    auto d =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    deltas.push_back(d);
  }

  std::sort(deltas.begin(), deltas.end());
  auto mid = deltas.size() / 2;
  double median = deltas.size() % 2 == 0 ? (deltas[mid - 1] + deltas[mid]) / 2.0
                                         : deltas[mid];

  int zeroCount = 0;
  long long minNonZero = std::numeric_limits<long long>::max();

  std::for_each(deltas.begin(), deltas.end(), [&](auto delta) {
    if (delta == 0)
      ++zeroCount;
    else
      minNonZero = std::min(minNonZero, delta);
  });
  std::cout << "Median:" << median << " Zero count:" << zeroCount
            << " minNonZero:" << minNonZero << "\n";
}

int main() {

  getTimerAccuracy();

  return 0;
}
