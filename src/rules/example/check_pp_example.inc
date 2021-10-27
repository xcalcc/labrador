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
// check_pp_example.inc
// ====================================================================
//
// example to describe how to write a preprocessor callbacks to check
// lexer related rules.
//
// implementation for include-system-header checker based on XSCA pp
// callbacks based checker. this rule means "Do NOT use quota to include
// system header files, use angle brackets":
//
// #include <stdio.h>   // compliance
// #include "stdio.h"   // not compliance
//
#include <clang/Lex/Preprocessor.h>
#include <vector>
class CheckPPExample : public PPNullHandler {
public:
  ~CheckPPExample() {}

private:
  void CheckMultipleSharp(const clang::MacroDirective *MD) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto macro_info = MD->getMacroInfo();
    auto macro_loc = macro_info->getDefinitionLoc();
    auto end_loc = macro_info->getDefinitionEndLoc();

    // return if encounter builtin marco
    if (src_mgr->isWrittenInBuiltinFile(macro_loc)) {
      return;
    }

    const char *start = src_mgr->getCharacterData(macro_loc);
    const char *end = src_mgr->getCharacterData(end_loc);

    std::vector<std::string> tokens;
    while(start != end) {
      if ((*start) != '#') {
        start++;
        continue;
      } else if (*(start+1) == '#') {
        tokens.push_back(std::string("##"));
        start += 2;
      } else {
        tokens.push_back(std::string("#"));
        start++;
      }
    }

    if (tokens.size() >= 2) {
      printf("GJB5396:4.1.1.11: Using '#' and '##' in the same macro is "
             "forbidden: %s\n",
             macro_loc.printToString(*src_mgr).c_str());
    }
  }

public:
  void InclusionDirective(clang::SourceLocation DirectiveLoc,
      const clang::Token &IncludeToken,
      llvm::StringRef IncludedFilename, bool IsAngled,
      clang::CharSourceRange FilenameRange,
      const clang::FileEntry *IncludedFile,
      llvm::StringRef SearchPath,
      llvm::StringRef RelativePath,
      const clang::Module *Imported,
      clang::SrcMgr::CharacteristicKind FileType) {
    printf("Hit include %s.\n", IncludedFilename.str().c_str());
  }

  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) {
    CheckMultipleSharp(MD);
  }

}; // CheckPPExample
