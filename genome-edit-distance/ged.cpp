#include <io/stream_map.hpp>
#include <str/distance.hpp>
#include <btl/io.hpp>

#include <iostream>
#include <random>

// good candidate for templating:
//    std::string, size_t (returned pair) --> ContType, IntType
template <typename _RandDist, typename _RandDev>
std::pair<std::string, size_t>
random_subsequence_position(const std::string& seq, size_t m, _RandDist& dist, _RandDev& dev)
{
  size_t j = dist(dev);
  return std::make_pair(std::string(seq.begin() + j, seq.begin() + j + m), j);
}

// TODO: Generation of sequencing indexes
//   1. No constraints
//      a. Linear
//      b. Circular
//   2. Non-overlapping
//      a. Linear
//      b. Circular
//   3. Only overlapping
//      a. Linear
//      b. Circular
//   4. Others
//      a. Exact amount of overlap (>0)


int
main(int argc, char** argv)
{
  std::cerr << "\n";

  // arguments check and conversion
  if (argc < 3) {
    std::cout << "Error in invocation\n";
    return 1;
  }
  std::string path = argv[1];
  size_t m = ctl::from_string<size_t>(std::string(argv[2]));
  size_t N = 1;
  if (argc >= 4) {
    N = ctl::from_string<size_t>(std::string(argv[3]));
  }
  
  // Initializations
  btl::HeaderGenomePair hgp = btl::read_fasta(path);
  std::random_device rd;
  auto rdev = std::mt19937(rd());
  auto unif = std::uniform_int_distribution<>(0, hgp.second.size()-m-1);

  std::cerr << "GENOME:  " << hgp.first << "\n"
	    << "SIZE:    " << hgp.second.size() << "\n\n";

  // can become a templated function with template params:
  //   - ED algorithm
  //   - Random dist and device
  //   - output stream
  
  // actual computationa and saving
  std::cout << "position1,position2,distance\n";
  auto wf = ctl::make_wf_alg(m,m);
  for (size_t i = 0; i < N; ++i) {
    auto p1 = random_subsequence_position(hgp.second, m, unif, rdev);
    auto p2 = random_subsequence_position(hgp.second, m, unif, rdev);    
    std::cout << p1.second << "," << p2.second << "," << wf(p1.first, p2.first) << "\n";
  }

  std::cerr << "\n";
  
  return 0;
}
