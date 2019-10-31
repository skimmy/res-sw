// stream_map.hpp

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

#include <ctl.h>

#include <string>
#include <vector>
#include <list>
#include <iostream>

// To be moved in a common header

// These are the type of variation as int
constexpr int SubstitutionType = 1;
constexpr int InsertionType    = 2;
constexpr int DeletionType     = 3;

// TODO: Study and, if appropriate, support to rGFA
// https://github.com/lh3/gfatools/blob/master/doc/rGFA.md
// by Heng Li

// TODO: The order in which insertions and deletions are applied is
// not unimportant. We must find a way to make the act of a script
// consisten regardless such order. Probably we must go through an
// intermediate representation.

// TODO: It is about time to split the file into different units each
// with header and source (if appropriate)

template <typename PosT_, typename StrT_, typename TypeT_ = int>
class local_var
{
private:
  PosT_  j;
  StrT_  v;
  TypeT_ t;
public:
  local_var(PosT_ j_, StrT_ v_, TypeT_ t_)
    : j(j_), v(v_), t(t_) { }

  PosT_ position() const { return j; }
  StrT_ variation() const { return v; }
  TypeT_ type() const { return t; }
  
};


template <typename VarsT_, typename TagT_>
class sequence_var
{
private:

  using VarT_ = typename VarsT_::value_type;
  
  TagT_  tag;
  VarsT_ variations;
  
public:
  sequence_var(TagT_ t_) : tag(t_), variations()
    { }

  sequence_var(VarsT_ v_, TagT_ t_) : tag(t_), variations(v_)
    { }

  void
  add_variation(VarT_ v_) { variations.push_back(v_); }

  // !! Probably this should go outside this class
  template <typename StrT_>
  StrT_
  apply(const StrT_& ref)
  {
    StrT_ str(ref);
    for(VarT_ var : variations)
    {
      
      auto type = var.type();
      auto pos  = var.position();
      auto vstr = var.variation();
      // !! There must implicit conversion of type to integer

      // Substitution
      if (type == SubstitutionType) {
	std::copy(vstr.begin(), vstr.end(), str.begin()+pos);
      }
      // Deletion
      // Mark deleted symbols with '-'
      
      // Insertion Insert position always refer to original
      // string. Thus if j is found, it means insert BEFORE ref[j]
      // j=0,...,n. When j=n menas insert at the end.
    }

    // copy without '-' (deletions)
    return str;
  }
  
};


template <typename StrT_, typename SeqVarsT_>
class edit_var
{
private:

  using VarsT_ = typename SeqVarsT_::value_type;
  
  StrT_     backbone;
  SeqVarsT_ variants;

public:
  edit_var(StrT_ bb) :
    backbone(bb), variants() {}

  void
  add_variant(VarsT_ v)
  {
    variants.push_back(v);
  }

  // !! If the string is big, it may be better to return a reference
  // !! and not a copy of the whole backbone
  StrT_
  backbone_sequence() const { return backbone; }

  std::size_t
  variant_count() const { return variants.size(); }

};



// Variation is represented as: position, text and type
using Variation = local_var<std::size_t, std::string, int>;
// Conveniente alias for a list of Variation
using VariationList = std::list<Variation>;
// A variant is a list of variation with an associated tag
using SequenceVariant = sequence_var<VariationList, int>;
// Preferred variations structure has
//   backbone   : std::string
//   variations : vector of SequenceVariation
using EditVars = edit_var<std::string,
			  std::vector<SequenceVariant>>;


// ---------------------------------------------------------
//                     FACTORY FUNCTIONS 
// ---------------------------------------------------------


// ------------------------ Variation ----------------------

/// Makes a substitution variation
Variation
make_sub_var(std::size_t j, const std::string& v)
{
  return Variation(j, v, SubstitutionType);
}

Variation
make_ins_var(std::size_t j, const std::string& v)
{
  return Variation(j, v, InsertionType);
}

Variation
make_del_var(std::size_t j, std::size_t d)
{
  // !! Inefficient representation of deletions
  return Variation(j, std::string(d, '-'), DeletionType);
}

// --------------------- VariationList ---------------------
VariationList
make_empty_variation_list()
{
  return std::list<Variation>();
}

template <typename It_>
VariationList
make_variation_list_from_iterator(It_ begin, It_ end)
{
  VariationList vlist = make_empty_variation_list();
  for (; begin != end; ++begin) {
    vlist.push_back(*begin);
  }
  return std::move(vlist);
}

// -------------------- SequenceVariant --------------------
SequenceVariant
make_empty_sequence_variant(int tag)
{
  return SequenceVariant(tag);
}

SequenceVariant
make_sequence_variant(VariationList vlist, int tag)
{
  return SequenceVariant(vlist, tag);
}


// !!! Elements in 'v' must be sorted wrt the position.
// In all other cases the output is meaningless (it may
// even lead to unexpected behavior, crash included).
std::string
apply_script_to_string(const VariationList& v, const std::string& x)
{
  if (v.size() == 0) {
    return std::string(x);
  }
  // iterator through all variation
  auto vl_b = v.begin();
  std::string y;
  std::size_t n = x.size();
  std::size_t j = 0;
  while (j<n && vl_b != v.end()) {
    std::size_t p = vl_b->position();
    std::string w = vl_b->variation();
    int t = vl_b->type();
    std::cout << "\t(" << p << "," << w << "," << t << ")\n";
    while(j<p) {
      y += x[j];
      ++j;
    }
    if (t == SubstitutionType) {
      y += w;
      j += w.size();
    } 
    if(t == InsertionType) {
	y += w;
    } 
    if (t == DeletionType) {
      j += w.size();
    }
    ++vl_b;
  }
  // there may be some match remaining
  while(j<n) {
    y += x[j];
    ++j;
  }
  return y;
}

int
main(int argc, char** argv)
{
  std::string ref {"ACGACTACCACACAT"};
  EditVars evg {ref};
  Variation vs  = make_sub_var(0, "TG");
  Variation vs2 = make_del_var(5,2);
  // ACGACTACCACACAT
  // tg   --
  // TGGACCCACACAT
  std::vector<Variation> v { vs, vs2 };
  auto v2 = make_variation_list_from_iterator(v.begin(), v.end());
  SequenceVariant var1 = make_empty_sequence_variant(100);
  SequenceVariant var2 = make_sequence_variant(v2, 2);
//  var.add_variation(vs);
  evg.add_variant(var1);
  evg.add_variant(var2);

  std::cout << "Backbone Sequence\n " << evg.backbone_sequence()
	    << "\nNumber of variants: " << evg.variant_count()
	    << "\n";

  std::cout << "[REMOve] V2 -> " << var2.apply<std::string>(ref)
	    << "\n";

  std::cout << "Var -> " << apply_script_to_string(v2, ref)
	    << "\n";
  return 0;
}
