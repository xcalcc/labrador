/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// SJT11682_stmt_rule.h
// ====================================================================
//
// implement all stmt related rules in SJT11682
//

namespace xsca {
namespace rule {


class SJT11682StmtRule : public StmtNullHandler {
public:
  ~SJT11682StmtRule() {}

private:
  const clang::FunctionDecl *_current_function_decl;

public:

  void VisitAtFunctionExit(const clang::Stmt *stmt) {
    _current_function_decl = nullptr;
    XcalCheckerManager::SetCurrentFunction(nullptr);
  }
}; // SJT11682StmtRule

}
}