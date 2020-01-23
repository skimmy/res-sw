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

// TODOs
// - Length distribution 
// - Parameter file
// - Distribution of errors as input parameter
// - Quality values
// - Edit script in verbose mode

template <typename StrT, typename IterT, typename RandD>
StrT
edit_error(IterT b, IterT e, std::vector<double> p, RandD& rd) {
  p.push_back(1.0 - p[0] - p[1] - p[2]);
  std::string op_str = "SDIM";  
  std::map<char, std::string> subs_map = {
    {'A', "CGT"},
    {'C', "AGT"},
    {'G', "ACT"},
    {'T', "ACG"},
    {'*', "ACGT"}
  };

  std::discrete_distribution<int> op_dist(p.begin(), p.end());
  std::uniform_int_distribution<int> sub_dist(0,2);
  std::uniform_int_distribution<int> ins_dist(0,3);

  // TODO: keep track of the script to put on verbose out
  StrT out;
  while (b != e) {
    auto c = *b;
    char op = op_str[op_dist(rd)];
    switch(op) {
    case 'M':
      out.push_back(c);
      ++b;
      break;
    case 'S':
      out.push_back(subs_map[c][sub_dist(rd)]);
      ++b;
      break;
    case 'I':
      out.push_back(subs_map['*'][ins_dist(rd)]);
      ++b;
      break;
    case 'D':
      ++b;
      break;
    default:
      ++b;
      break;
    }
  }
  return out;
}

template <typename RandD>
std::string
string_hamming_error(const std::string& r, double pe, RandD& rd) {
  return edit_error<std::string, decltype(r.begin()), RandD>(r.cbegin(), r.cend(), {pe, 0.0, 0.0}, rd);
}

template <typename RandD>
std::string
string_edit_error(const std::string& r, std::vector<double> pe, RandD& rd) {
  return edit_error<std::string, decltype(r.begin()), RandD>(r.cbegin(), r.cend(), pe, rd);
}

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

  bool verbose_out = true;
  
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
    std::string ver_h = (verbose_out) ? (" (" + r + ") ") : "";
    std::cout << ">id=" << i << " j=" << j << ver_h << "\n";
    // Error
    switch (error) {
    case 1: // hamming error
      r = string_hamming_error(r, 0.1, rd);
      break;
    case 2: // edit error
      r = string_edit_error(r, {0.1, 0.05, 0.05}, rd);
      break;
    default:
      break;
    }
    std::cout << r << "\n";
  }
  
  return 0;
}
