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
// base_diagnostic.h
// ====================================================================
//
// the base diagnostic collector for checkers
//

#ifndef LABRADOR_BASE_DIAGNOSTIC_H
#define LABRADOR_BASE_DIAGNOSTIC_H

#include "diagnostic_dispatcher.h"

namespace xsca {
namespace rule {

class RuleBaseDiagnostic : public clang::IgnoringDiagConsumer {
protected:
  bool _enable;

public:

  inline bool Enable() const { return _enable; }

  RuleBaseDiagnostic() = default;

  virtual void AddIssue(const char *rule, const std::string &ref_msg,
                        clang::SourceLocation location) = 0;
};

}
}
#endif //LABRADOR_BASE_DIAGNOSTIC_H
