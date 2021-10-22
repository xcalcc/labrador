/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB8114_pp_rule.h
// ====================================================================
//
// implement preprocess related rules in GJB8114
//

#include "pp_null_handler.h"
#include "xsca_checker_manager.h"

#include <unordered_set>
#include <vector>
#include <clang/Lex/Preprocessor.h>

namespace xsca {
namespace rule {
class GJB8114PPRule : public PPNullHandler {
public:
  ~GJB8114PPRule() {}

  GJB8114PPRule() {
    _included_file.clear();
    _disabled = XcalCheckerManager::GetDisableOption().getValue().find("GJB8114") != std::string::npos;
  }

private:
  std::unordered_set<std::string> _included_file;

  /*
   * GJB5111: 5.1.1.1
   * Changing the definition of basic type or keywords by macro is forbidden
   */
  void CheckRedefineKeywordsByMacro(const clang::MacroDirective *MD);

  /*
   * GJB8114: 5.1.1.22
   * Head file being re-included is forbidden
   */
  void CheckReIncludeHeadFile(clang::SourceLocation Loc, llvm::StringRef IncludedFilename);

  /*
   * GJB8114: 6.9.2.4
   * Don't use a header file with .h as suffix
   */
  void CheckHeadSuffix(clang::SourceLocation Loc, llvm::StringRef IncludedFilename);

public:

  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) {
    CheckRedefineKeywordsByMacro(MD);
  }

  void InclusionDirective(clang::SourceLocation DirectiveLoc,
                          const clang::Token &IncludeToken, llvm::StringRef IncludedFilename,
                          bool IsAngled, clang::CharSourceRange FilenameRange,
                          const clang::FileEntry *IncludedFile, llvm::StringRef SearchPath,
                          llvm::StringRef RelativePath, const clang::Module *Imported,
                          clang::SrcMgr::CharacteristicKind FileType) {
    CheckReIncludeHeadFile(DirectiveLoc, IncludedFilename);
    CheckHeadSuffix(DirectiveLoc, IncludedFilename);
  }

}; // GJB8114PPRule
}
}
