 // ged.cpp

 // Copyright 2019 Michele Schimd

 // Licensed under the Apache License, Version 2.0 (the "License");
 // you may not use this file except in compliance with the License.
 // You may obtain a copy of the License at

 //   http://www.apache.org/licenses/LICENSE-2.0

 // Unless required by applicable law or agreed to in writing, software
 // distributed under the License is distributed on an "AS IS" BASIS,
 // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 // See the License for the specific language governing permissions and
 // limitations under the License.


#include <io/stream_map.hpp>
#include <str/distance.hpp>
#include <str/kmer.hpp>
#include <btl/io.hpp>

#include <iostream>
#include <random>
#include <map>

/// This software takes as input a genome in the fasta format and
/// produces as output a csv file that contains N lines. Each line
/// contains three elements: two positions where corresponding to
/// starting position for substrings of the original genome. The third
/// element in each line is the edit distance between the two
/// substrings. Users can control, using the proper argument line
/// parmeters, the following aspects: the fasta file, the size of
/// substrings, the number of substrings and the total number of pairs
/// of substrings that are generated.

struct Options
{
  std::string fasta_path;
  std::size_t read_length;
  std::size_t read_count;
  std::size_t read_overlap;
  int         verbosity;

  Options(int argc, char** argv)
    : fasta_path {""}, read_length {10}, read_count {1},
      read_overlap {0}, verbosity {0}
  {
    // when only one paramter is given it assumed to be a key=value
    // file, otherwise there is a specific order in which parameters
    // must be specified
    if (argc == 2) {
      std::ifstream is {argv[1]};
      auto kv_map = ctl::stream_to_map<std::string, std::string>(is,'=','#');
      auto it_end = kv_map.end();
      if (kv_map.find("fasta") != it_end) {
	fasta_path = kv_map["fasta"];
      }
      else {
	std::cout << "Configuration file error\n";
	exit(1);
      }
      if(kv_map.find("n") != it_end) {
	read_length = ctl::from_string<std::size_t>(kv_map["n"]);
      }
      if (kv_map.find("N") != it_end) {
	read_count = ctl::from_string<std::size_t>(kv_map["N"]);     
      }
      if (kv_map.find("s") != it_end) {
	read_overlap = ctl::from_string<std::size_t>(kv_map["s"]);
      }
      if(kv_map.find("verbosity") != it_end) {
	verbosity = ctl::from_string<int>(kv_map["verbosity"]);
      }
      
    } else {
    

      //arguments check and conversion
      if (argc < 3) {
	std::cout << "Error in invocation\n";
	std::cout << "Usage:\n\tged fasta [m N s]\n\n"; 
	exit(1);
      }
      fasta_path = argv[1];
      read_length = ctl::from_string<size_t>(std::string(argv[2]));
      if (argc >= 4) {
	read_count = ctl::from_string<size_t>(std::string(argv[3]));
      }
      if (argc >= 5) {
	read_overlap = ctl::from_string<size_t>(std::string(argv[4]));
      }
    }
  }

  void
  print(std::ostream& os) {
    os << "Options\n";
    os << "-------\n\n";
    os << "  File          " << fasta_path   << "\n";
    os << "  Read len   n= " << read_length  << "\n";
    os << "  Read count N= " << read_count   << "\n";
    os << "  Overlap       " << read_overlap << "\n";
    os << "  Verbosity     " << verbosity    << "\n";
    os << "\n";
  }
};

// good candidate for templating:
//    std::string, size_t (returned pair) --> ContType, IntType
template <typename _RandDist, typename _RandDev>
std::pair<std::string, size_t>
random_subsequence_position(const std::string& seq, size_t m, _RandDist& dist, _RandDev& dev)
{
  size_t j = dist(dev);
  return std::make_pair(std::string(seq.begin() + j, seq.begin() + j + m), j);
}

// returns s1, s2, pos1
template <typename _RandDist, typename _RandDev>
std::tuple<std::string, std::string, size_t>
random_overlapping_subsequences(const std::string& seq, size_t m, size_t w, _RandDist& dist, _RandDev& dev)
{
  size_t j = dist(dev);
  return std::make_tuple(
    std::string(seq.begin() + j, seq.begin() + j + m),
    std::string(seq.begin() + j + w, seq.begin() + j + w + m), j);
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


/// \brief Computes edit distane between several pairs of substrings
/// of a given string (the 'genome').
///
/// \param genome the string to produces substrings.
/// \param m the length of the substrings.
/// \param N the number of pairs of substrings.
/// \param s the overlap between pairs (when 0 pairs are ranodmly generate)
/// \param wf the edit distance algorithm.
/// \param rdev
/// \param out
/// \param header

template <typename AlgED_, typename RandD_, typename OutT_>
void
compute(const std::string& genome, size_t m, size_t N, size_t s,
	AlgED_& wf,  RandD_& rdev, OutT_& out, bool header = true)
{
  size_t slack = s>0 ? m-s : 0;
  auto dist = std::uniform_int_distribution<>(0, genome.size()-m-1-slack);
  if (header) {
    out << "position1,position2,distance\n";
  }
  std::pair<std::string, size_t> p1, p2;
  for (size_t i = 0; i < N; ++i) {
    p1 = random_subsequence_position(genome, m, dist, rdev);
    if (s > 0) {
      p2.second = p1.second + m - s;
      p2.first = std::string(genome.begin() + p2.second, genome.begin() + p2.second + m);
    } else {
      p2 = random_subsequence_position(genome, m, dist, rdev);
    }
    out << p1.second << "," << p2.second << "," << wf(p1.first, p2.first) << "\n";
  }
}


int
main(int argc, char** argv)
{
  std::cerr << "\n";

  Options opts(argc, argv);
  if (opts.verbosity >= 2) {
    opts.print(std::cerr);
  }
  
  // Initializations
  btl::HeaderGenomePair hgp = btl::read_fasta(opts.fasta_path);
  std::random_device rd;
  auto rdev = std::mt19937(rd());

  if (opts.verbosity >= 1) {
    // print some information on the input
    std::cerr << "GENOME:  " << hgp.first << "\n"
	      << "SIZE:    " << hgp.second.size() << "\n";
    std::map<std::string, std::size_t> mm;
    ctl::kmer_statistics(hgp.second, 1, mm);
    std::cerr << "BASE DISTRIBUTION:\n";
    for (auto p : mm) {
      std::cerr << "        " << p.first << ": " << p.second << " "
		<< (static_cast<double>(p.second) / hgp.second.size()) << "\n";
    }
    std::cerr << "\n";
  }

  // actual computation
  size_t m = opts.read_length;
  size_t N = opts.read_count;
  size_t s = opts.read_overlap;
  auto wf = ctl::make_wf_alg(m,m);
  compute(hgp.second, m, N, s, wf, rdev, std::cout);

  std::cerr << "\n";
  
  return 0;
}
