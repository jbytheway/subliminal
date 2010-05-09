#include <cmath>
#include <array>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iostream>

#include <boost/foreach.hpp>

int main(int argc, char const* const* const argv)
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " FREQS" << std::endl;
    return 1;
  }

  std::ifstream freqs(argv[1]);
  if (!freqs.is_open()) {
    std::cerr << argv[1] << ": couldn't read" << std::endl;
    return 1;
  }

  std::unordered_map<std::uint8_t, size_t> frequency_map;
  size_t total = 0;
  std::string line;
  while (std::getline(freqs, line)) {
    std::istringstream is(line);
    char c;
    is.get(c);
    size_t count;
    if (!(is >> count)) {
      std::cerr << "malformed line: " << line << std::endl;
      return 1;
    }
    frequency_map[c] = count;
    total += count;
  }

  // Convert frequency map to score, with laplacian smoothing
  std::array<double, 256> scores;
  std::fill(scores.begin(), scores.end(), 0.0);
  BOOST_FOREACH(auto const& p, frequency_map) {
    scores[p.first] = std::log(p.second+1);
  }

  // Now read the input, and score it
  char c;
  double score = 0.0;
  while (std::cin.get(c)) {
    score += scores[std::uint8_t(c)];
  }

  std::cout << score << std::endl;
  return 0;
}

