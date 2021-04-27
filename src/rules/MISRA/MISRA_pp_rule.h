/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
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
private:

#if 0
  std::hash<const clang::MacroDirective *> _hash_gen;
  std::set<unsigned long> _all_macros={};
  std::set<unsigned long> _used_macros;

  void RecordMacro(const clang::Token &MacroNameTok, const clang::MacroDirective *MD);

  /* MISRA
   * Rule: 2.5
   * A project should not contain unused type declarations
   */
  void RecordUsedMacro(const clang::Token &MacroNameTok, const clang::MacroDirective *MD);

public:
  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) {
    RecordMacro(MacroNameTok, MD);
  }

  void MacroExpands(const clang::Token &MacroNameTok,
                    const clang::MacroDefinition &MD,
                    clang::SourceRange Range,
                    const clang::MacroArgs *Args
  ) {
    TRACE0();
    RecordUsedMacro(MacroNameTok, MD.getLocalDirective());
  }
#endif

}; // MISRAPPRule

}
}