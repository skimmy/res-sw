#include <ctl.h>

#include <string>
#include <list>
#include <iostream>

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
  return 0;
}
