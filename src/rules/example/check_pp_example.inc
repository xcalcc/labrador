/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
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
class CheckPPExample : public PPNullHandler {
public:
  ~CheckPPExample() {}

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

}; // CheckPPExample
