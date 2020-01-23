// read_gen.hpp

// Copyright 2020 Michele Schimd

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

#include <io/stream_map.hpp>

#include <btl/io.hpp>

int
main(int argc, char** argv)
{

  // If only a parameter is given, it must be the configuration file
  if (argc == 2) {
    
  }
  
  if (argc < 4) {
    std::cerr << "Invalid usage\n  read-gen G N L [Err]\n";
  }

  std::string gen_file { argv[1] };
  size_t N = ctl::from_string<size_t>(argv[2]);
  // TODO: add support for distribution of lengths
  size_t L = ctl::from_string<size_t>(argv[3]);
  size_t Lmax = L;

  int error = 0; // No error
  bool circular = true;
  int pos = 0;
  
  if (argc == 5) {
    error = ctl::from_string<int>(argv[4]);
  }

  // load the fasta file
  auto genome = btl::read_fasta(gen_file);
  size_t G = genome.second.size();
  // augment the genome with Lmax to emulate 'circularity'
  genome.second.insert(genome.second.end(), genome.second.begin(), genome.second.begin() + Lmax);

  std::random_device rd;
  std::default_random_engine rdev(rd());
  std::uniform_int_distribution<int> pdist(0,G-1);

  for (size_t i = 0; i < N; ++i) {
    size_t j = pdist(rdev);
    size_t Lj = L;
    std::string r {genome.second.begin() + j, genome.second.begin()+j+Lj};
    // Error
    switch (error) {
    case 1: // hamming error
      break;
    case 2: // edit error
      break;
    default:
      break;
    }
    std::cout << ">id=" << i << " j=" << j << "\n";
    std::cout << r << "\n";
  }
  
  return 0;
}
