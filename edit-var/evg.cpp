#include <ctl.h>

#include <string>
#include <vector>
#include <list>
#include <iostream>

template <typename PosT_, typename StrT_, typename TypeT_ = int>
class local_var
{
  PosT_  j;
  StrT_  v;
  TypeT_ type;
  
};

using Variation = local_var<std::size_t, std::string>;
using VariationList = std::list<Variation>;

template <typename VarsT_, typename TagT_>
class sequence_var
{
  TagT_  tag;
  VarsT_ variations;
};

using SequenceVariant = sequence_var<VariationList, int>;


template <typename StrT_, typename SeqVarsT_>
class edit_var
{
  StrT_     backbone;
  SeqVarsT_ variations;
};

// Preferred variations structure has
//   backbone   : std::string
//   variations : voctor of SequenceVariation
using EditVars = edit_var<std::string,
			  std::vector<SequenceVariant>>;

int
main(int argc, char** argv)
{
  EditVars evg;
  return 0;
}
