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
// MISRA_pp_rule.h
// ====================================================================
//
// implement preprocess related rules in MISRA-C-2012
//

#include <set>
#include "pp_null_handler.h"
#include "xsca_checker_manager.h"
#include <clang/Lex/Preprocessor.h>


namespace xsca {
namespace rule {
class MISRAPPRule : public PPNullHandler {
public:
  MISRAPPRule() {
    _enable = true;
  }

private:

  /* MISRA
   * Rule: 5.5
   * Identifiers shall be distinct from macro names
   */
  void CheckRecordMacroName(const clang::Token &MacroNameTok,
                            const clang::MacroDirective *MD);
  /* MISRA
   * Rule: 7.1
   * Octal constants shall not be used
   */
  void CheckIfValue(clang::SourceLocation Loc, clang::SourceRange ConditionalRange,
                    clang::PPCallbacks::ConditionValueKind ConditionalValue);

  /* MISRA
   * Rule: 17.1
   * The features of <stdarg.h> shall not be used
   * Rule: 21.5
   * The standard header file <signal.h> shall not be used
   */
  void CheckFidHeaderFile(clang::SourceLocation Loc, llvm::StringRef IncludedFilename);

  /* MISRA
   * Rule: 21.6
   * The Standard Library input/output functions shall not be used
   */
  void CheckIOFunctionInStdio(const clang::Token &MacroNameTok, const clang::MacroDefinition &MD);

public:
  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) {
    CheckRecordMacroName(MacroNameTok, MD);
  }

  void MacroExpands(const clang::Token &MacroNameTok,
                    const clang::MacroDefinition &MD,
                    clang::SourceRange Range,
                    const clang::MacroArgs *Args
  ) {
    CheckIOFunctionInStdio(MacroNameTok, MD);
  }

  void InclusionDirective(clang::SourceLocation DirectiveLoc,
                          const clang::Token &IncludeToken, llvm::StringRef IncludedFilename,
                          bool IsAngled, clang::CharSourceRange FilenameRange,
                          const clang::FileEntry *IncludedFile, llvm::StringRef SearchPath,
                          llvm::StringRef RelativePath, const clang::Module *Imported,
                          clang::SrcMgr::CharacteristicKind FileType) {
    CheckFidHeaderFile(DirectiveLoc, IncludedFilename);
  }

  void If(clang::SourceLocation Loc, clang::SourceRange ConditionalRange,
          clang::PPCallbacks::ConditionValueKind ConditionalValue) {
    CheckIfValue(Loc, ConditionalRange, ConditionalValue);
  }

}; // MISRAPPRule

}
}
