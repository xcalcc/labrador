/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// rule_set_decl.h
// ====================================================================
//
// define all ruleset supported
//

#ifndef RULE_SET_DECL_INCLUDED
#define RULE_SET_DECL_INCLUDED

// to add a new ruleset, add a line with ruleset name below.

#define ALL_RULE_SETS(RS) \
    RS(GJB5369) \
    RS(GJB8114) \
    RS(MISRA) \
    RS(SJT11682) \
    RS(example)


namespace xsca {

// enum for all ruleset:
// RS_GJB5369, RS_GJB8114, ..., RS_MAX
enum RULE_SET {
#define V(rs)  RS_##rs,
  ALL_RULE_SETS(V)
#undef V
  RS_MAX;
};

// get name for ruleset
// "GJB5369", "GJB8114", ..., "MAX"
inline const char *
RuleSetName(RULE_SET rs) {
  if (rs < 0 || rs >= RS_MAX)
    return "-ERROR-";
  static const char* name[RS_MAX] = {
  #define V(rs) #rs,
    ALL_RULE_SETS(V)
  #undef V
    "MAX"
  };
  return name[rs];
}

}  // namespace xsca

#endif  // RULE_SET_DECL_INCLUDED
