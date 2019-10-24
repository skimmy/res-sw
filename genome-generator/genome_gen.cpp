// genome_gen.hpp

// Copyright 2019 Michele Schimd

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <random>
#include <iostream>

#include <btl/generator.hpp>
#include <btl/io.hpp>

#include <io/stream_map.hpp>

int
main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Error in invocation\n";
    exit(1);
  }
  size_t G = ctl::from_string<size_t>(argv[1]);
  // by default the distribution is uniform on {A,C,G,T}
  std::vector<double> dist {1,1,1,1};
  // if only one parameter is given, it represents the encoding of distribution
  if (argc == 3) {
    int dist_param = ctl::from_string<int>(argv[2]);
    switch(dist_param) {
    case 1:
      // GC Rich
      dist = {0.2, 0.3, 0.2, 0.3};
      break;
    case 2:
      // GC Poor
      dist = {0.3, 0.2, 0.3, 0.2};
      break;
    default:
      break;
    }
  }
  // weights of each symbol are given separately
  if (argc == 6) {
    for (size_t i = 0; i < 4; ++i) {
      dist[i] = ctl::from_string<double>(argv[i+2]);
    }
  }

  // constructs header for the fasta file
  std::string header {"> iid genome"};
  header += (" G=" + std::to_string(G));
  header += " dist=(";
  for (auto w : dist) {
    header += std::to_string(w) + ",";
  }
  header += ")";

  std::random_device rd;
  std::default_random_engine rdev(rd());
  std::string genome = btl::random_genome_string(G, dist, rdev);
  btl::write_fasta(std::cout, genome, header);
  return 0;
}
