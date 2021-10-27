/*
   Copyright (C) 2021 Xcalibyte (Shenzhen) Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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
