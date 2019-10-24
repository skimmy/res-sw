#include <ctl.h>

#include <string>
#include <list>
#include <iostream>

template <typename PosT_, typename StrT_, typename TypeT_ = int>
class local_var
{
  PosT_  j;
  StrT_  v;
  TypeT_ type;
  
};

template <typename StrT_, typename ContT_>
class edit_var
{
  StrT_  backbone;
  ContT_ variations; 
};

using Variations = local_var<std::size_t, std::string>;
using EditVars   = edit_var<std::string, std::list<Variations>>;

int
main(int argc, char** argv)
{
  EditVars evg;
  return 0;
}
