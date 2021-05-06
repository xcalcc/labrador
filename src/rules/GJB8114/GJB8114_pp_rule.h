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

#include <vector>
#include <clang/Lex/Preprocessor.h>

namespace xsca {
namespace rule {
class GJB8114PPRule : public PPNullHandler {
public:
  ~GJB8114PPRule() {}

private:
  /*
   * GJB5111: 5.1.1.1
   * Changing the definition of basic type or keywords by macro is forbidden
   */
  void CheckRedefineKeywordsByMacro(const clang::MacroDirective *MD);

  /*
   * GJB5111: 5.1.1.2
   * Define other something as keywords is forbidden
   * GJB5369: 4.1.1.13 -> CheckMacroKeywords
   */

public:

  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) {
    CheckRedefineKeywordsByMacro(MD);
  }

}; // GJB8114PPRule
}
}
