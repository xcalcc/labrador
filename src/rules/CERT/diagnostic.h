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

#ifndef LABRADOR_CERT_DIAGNOSTIC_H
#define LABRADOR_CERT_DIAGNOSTIC_H

#include "rules/common/base_diagnostic.h"

namespace xsca {
namespace rule {
class CERTDiagnostic : public RuleBaseDiagnostic {
private:

public:
  CERTDiagnostic() = default;

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) override;

  void AddIssue(const char *rule, const std::string &ref_msg, clang::SourceLocation location) override;
};
}
}

#endif //LABRADOR_DIAGNOSTIC_H
