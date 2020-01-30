// ed_score.hpp

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

#include <string>
#include <list>
#include <iostream>

#include <str/distance.hpp>


template <typename ListT>
std::pair<std::string, std::string>
align_from_script(const std::string& x, const std::string& y, const ListT& l) {
  size_t i = 0;
  size_t j = 0;
  std::string xa;
  std::string ya;
  for (auto p : l) {
    size_t ip = p.first;
    size_t jp = p.second;
    char cx = ' ';
    if (ip-i > 0) {
      cx = x[ip-1];
    }
    char cy = ' ';
    if (jp-j > 0) {
      cy = y[jp-1];
    }
    xa = xa + cx;
    ya = ya + cy;
    i = ip;
    j = jp;    
  }
  return std::make_pair(xa,ya);
}

int
main(int argc, char** argv)
{
  std::string x {"AGCTACCGTGAACTGGT"};  
  std::string y {"TACCGTAAAGCTAATTGTAA"};

  //std::string x {"TACGT"};  
  //std::string y {"ACGTA"};
  
  size_t n = x.size();
  size_t m = y.size();
  auto ed = ctl::make_wf_alg<size_t>(n,m);

  // make all possible overlaps x/y
  for (size_t i = 0; i < n-1; ++i) {
    std::string x1 = x.substr(i,n-i);
    std::string y2 = y.substr(0,std::min(n-i,m));
    std::cout << x1 << "  " << y2 << "\t" << ed(x1,y2) / (double)(x1.size()+y2.size()) << "\n";
  }

  using ListPair = std::list<std::pair<size_t,size_t>>;

  ed(x,y);
  auto l = ed.backtrack<ListPair>();
  for (auto p : l) {
    std::cout << "(" << p.first << "," << p.second << ")\n";
  }
  auto pp = align_from_script<ListPair>(x,y,l);
  std::cout << pp.first << "\n" << pp.second << "\n";

  // make all poassible overlaps y/x
  

  return 0;
}
