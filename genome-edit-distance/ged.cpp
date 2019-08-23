/*
 ged.cpp

 Copyright 2019 Michele Schimd

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

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

template <typename AlgED_, typename DistT_, typename RandD_, typename OutT_>
void
compute(const std::string& genome, size_t m, size_t N, AlgED_& wf, DistT_& dist,
	RandD_& rdev, OutT_& out)
{
  out << "position1,position2,distance\n";
  
  for (size_t i = 0; i < N; ++i) {
    auto p1 = random_subsequence_position(genome, m, dist, rdev);
    auto p2 = random_subsequence_position(genome, m, dist, rdev);    
    out << p1.second << "," << p2.second << "," << wf(p1.first, p2.first) << "\n";
  }
}


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

  auto wf = ctl::make_wf_alg(m,m);
  compute(hgp.second, m, N, wf, unif, rdev, std::cout);

  std::cerr << "\n";
  
  return 0;
}
