/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_rule_meta.h
// ====================================================================
//
// interface for XSCA rule meta information:
//   rule set: a collection of rules belongs to the same standard
//   rule: a rule in the standard
//

#ifndef XSCA_RULE_META_INCLUDED
#define XSCA_RULE_META_INCLUDED

#include <stdint.h>

namespace xsca {

class XcalSCARule {
};  // XcalSCARule

// class XcalSCARuleManager
class XcalSCARuleManager {
public:
  template<typename RS>
  static int32_t RegisterRuleSet(const char* name) {
  }
};  // XcalSCARuleManager

}  // name xsca


#endif  // XSCA_RULE_META_INCLUDED
