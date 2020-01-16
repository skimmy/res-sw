#include <ctl.h>
#include <data_structure/matrix.hpp>
#include <iterator/string_iterator.hpp>
#include <str/distance.hpp>

#include <cmath>
#include <string>
#include <list>
#include <iostream>

// This function should eventually become a template that allows
// allows generic DP algorithms by specifying a matrix an an update
// rule. Then, the function will be inserted into ctl.

// For now this computes the DP matrix for probabilies of all scripts
// Templatable parts:
//   x,y strings -> StrT
//   ps vector<double> ProbsT
//   return type double -> ValT
double
ed_scripts_probability(const std::string& x, const std::string& y,
		       const std::vector<double> ps) {
  size_t n = x.size();
  size_t m = y.size();
  double pM = ps[0];
  double pS = ps[1];
  double pD = ps[2];
  double pI = ps[3];
  ctl::_2D_array<double> dpm(n+1 ,m+1);
  // initialization 
  dpm(0,0) = 1;
  for (size_t i = 1; i<n+1; ++i) {
    dpm(i,0) = dpm(i-1,0) * pD;
  }
  for (size_t j = 1; j<m+1; ++j) {
    dpm(0,j) = dpm(0,j-1) * pI;
  }

  for (size_t i = 1; i<n+1; ++i) {
    for (size_t j = 1; j<m+1; ++j) {
      double pMS = (x[i-1]==y[j-1]) ? pM : pS;
      dpm(i,j) = dpm(i-1,j)*pD + dpm(i,j-1)*pI + dpm(i-1,j-1)* pMS;
    }
  }
  
  return dpm(n,m);
}

template <typename ItT>
std::size_t script_cost(ItT b, ItT e) {
  std::size_t cost = 0;
  while(b != e) {
    char e = *b;
    if (e == 'D' or e == 'I' or e == 'S') {
      cost++;
    }
    ++b;
  }
  return cost;
}

using Script = std::string;
using RunningScript = std::pair<Script, std::pair<std::size_t, std::size_t>>;

RunningScript
make_running_script(const std::string& script, std::size_t i, std::size_t j) {
  return std::make_pair(script, std::make_pair(i,j));
}

template <typename ListT, typename StrT>
ListT
script_list(const StrT& x, const StrT& y, std::size_t n, std::size_t m)
{
  ListT tmp_list;
  tmp_list.push_back(make_running_script("",0,0));
  ListT out_list;
  ListT next_list;
  while (!tmp_list.empty()) {
    next_list.clear();
    RunningScript rho = tmp_list.back();
    tmp_list.pop_back();
    StrT E = rho.first;
    std::size_t i = rho.second.first;
    std::size_t j = rho.second.second;
    if (i == n) {
      next_list.push_back(make_running_script(E+"I", i, j+1));
    }
    if (j == m) {
      next_list.push_back(make_running_script(E+"D", i+1, j));
    }
    if (i<n and j<m) {
      next_list.push_back(make_running_script(E+"I", i, j+1));
      next_list.push_back(make_running_script(E+"D", i+1, j));
      char SM = (x[i]==y[j]) ? 'M' : 'S';
      next_list.push_back(make_running_script(E+SM, i+1, j+1));
    }
    for (RunningScript r : next_list) {
      if (r.second.first == n and r.second.second == m) {
	out_list.push_back(r);
      } else {
	tmp_list.push_back(r);
      }
    }
  }
  return out_list;
}

int
main(int argc, char** argv)
{
  std::string x = "ACGG";
  std::string y = "GG";
  // ToDo: the output list needs not to have indexes since they
  // all will be (n,m)
  auto L = script_list<std::list<RunningScript>, std::string>(x,y,x.size(), y.size());
  for (RunningScript s : L) {
    std::cout << s.first << "\t(" << s.second.first << "," << s.second.second
	      << ")\t" << script_cost(s.first.begin(), s.first.end()) << "\n";
  }
  std::cout << "Total scripts: " << L.size() << "\n";


  std::vector<std::string> strs { "AAAA", "TTTT", "GGGG", "CCCC", "ACGT", "ACAT"};
  std::vector<double> probs {0.6,0.2,0.1,0.1};
  ctl::SigmaNIterator strIt(4,"ACGT");
  ctl::SigmaNIterator strItEnd;
//  for (std::string yy : strs) {
  auto wf = ctl::make_wf_alg(5,5);
  wf.init();
  x = "ACTA";
  while(strIt != strItEnd) {
    std::string yy = *strIt;
    double PE = ed_scripts_probability(x,yy, probs);
//    std::cout << "P(" << yy << ")\t" << PE << "\n";
    std::cout << yy << "," << PE << "," << wf(x,yy) << "\n";
    ++strIt;
  }
  
  return 0;
}
