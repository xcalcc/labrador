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

#ifndef LABRADOR_AUTOSAR_PP_RULE_H
#define LABRADOR_AUTOSAR_PP_RULE_H

#include "pp_null_handler.h"
#include "xsca_checker_manager.h"

#include <unordered_set>
#include <vector>
#include <clang/Lex/Preprocessor.h>

namespace xsca {
namespace rule {
class AUTOSARPPRule : public PPNullHandler {
public:
  ~AUTOSARPPRule() {}

  AUTOSARPPRule() {
    _enable = true;
//    _enable = XcalCheckerManager::GetEnableOption().getValue().find("AUTOSAR") != std::string::npos;
  }

private:

public:

  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) {
  }

  void InclusionDirective(clang::SourceLocation DirectiveLoc,
                          const clang::Token &IncludeToken, llvm::StringRef IncludedFilename,
                          bool IsAngled, clang::CharSourceRange FilenameRange,
                          const clang::FileEntry *IncludedFile, llvm::StringRef SearchPath,
                          llvm::StringRef RelativePath, const clang::Module *Imported,
                          clang::SrcMgr::CharacteristicKind FileType) {
  }

}; // GJB8114PPRule
}
}


#endif //LABRADOR_AUTOSAR_PP_RULE_H
