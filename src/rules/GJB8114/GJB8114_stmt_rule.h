/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB8114_stmt_rule.h
// ====================================================================
//
// implement all stmt related rules in GJB8114
//

#include "GJB8114_enum.inc"
#include "stmt_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class GJB8114StmtRule : public StmtNullHandler {
public:
  ~GJB8114StmtRule() {}

private:

  /*
   * GJB8114: 5.2.1.1
   * Loop body should be enclosed with brace
   */
  void CheckLoopBodyWithBrace(const clang::Stmt *stmt);

  bool CheckStmtWithBrace(const clang::Stmt *stmt);

  /*
   * GJB8114: 5.3.1.3
   * Assigning to pointer parameters is forbidden
   */
  void CheckAssignToPointerParam(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.3.1.9
   * Using NULL to stand a nullptr instead of using 0
   */
  void CheckUsingNullWithPointer(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.4.1.8
   * Cases of switch should have the same hierarchy range
   */
  void CheckDifferentHierarchySwitchCase(const clang::SwitchStmt *stmt);

public:
  void VisitForStmt(const clang::ForStmt *stmt) {
    CheckLoopBodyWithBrace(stmt->getBody());
  }

  void VisitWhileStmt(const clang::WhileStmt *stmt) {
    CheckLoopBodyWithBrace(stmt->getBody());
  }

  void VisitDoStmt(const clang::DoStmt *stmt) {
    CheckLoopBodyWithBrace(stmt->getBody());
  }

  void VisitBinaryOperator(const clang::BinaryOperator *stmt) {
    CheckAssignToPointerParam(stmt);
    CheckUsingNullWithPointer(stmt);
  }

  void VisitSwitchStmt(const clang::SwitchStmt *stmt) {
    CheckDifferentHierarchySwitchCase(stmt);
  }

}; // GJB8114StmtRule
}
}
