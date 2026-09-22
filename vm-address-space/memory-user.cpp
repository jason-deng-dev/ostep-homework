#include <assert.h>
#include <charconv>
#include <cstddef>
#include <iostream>
#include <string_view>
#include <unistd.h>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Need to pass in paramter\n";
    return 1;
  }

  int mb = 0;
  std::string_view sv = argv[1];
  auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), mb);

  // ec != std::errc{} catches overflow / invalid input
  // ptr != sv.data() + sv.size() ensures whole string was consumed (rejects
  // "12abc")
  if (ec != std::errc{} || ptr != sv.data() + sv.size()) {
    std::cerr << "Not valid integer:" << argv[1] << '\n';
    return 1;
  }

  // compute element count in size_t, since mb*1024*1024 overflows int
  const size_t bytes = static_cast<size_t>(mb) * 1024 * 1024;
  const size_t elements = bytes / sizeof(int);

  // allocate
  std::vector<int> arr(elements, 0);

  std::cout << "PID " << getpid() << " using " << mb << " MB (" << elements
            << " ints)\n";

  size_t passes = 0;
  while (true) {
    for (auto &x : arr)
      x += 1; // touches every page, every pass
    if (++passes % 100 == 0)
      std::cout << "pass " << passes
                << '\n'; // liveness + observable side effect
  }
}
