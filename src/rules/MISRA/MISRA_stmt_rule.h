/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// MISRA_stmt_rule.h
// ====================================================================
//
// implement all stmt related rules in MISRA-C-2012
//

#include "MISRA_enum.inc"
#include "stmt_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class MISRAStmtRule : public StmtNullHandler {
public:
  ~MISRAStmtRule() = default;

private:

  /* MISRA
   * Rule: 7.4
   * A string literal shall not be assigned to an object unless the object’s type is “pointer to const-qualified char”
   */
  void CheckStringLiteralToNonConstChar(const clang::BinaryOperator *stmt);

  void CheckStringLiteralToNonConstChar(const clang::CallExpr *stmt);

  /* MISRA
   * Rule: 10 .2
   * Expressions of essentially character type shall not be used inappropriately in addition and subtraction operations
   */
  void CheckAddOrSubOnCharacter(const clang::BinaryOperator *stmt);

public:

  void VisitBinaryOperator(const clang::BinaryOperator *stmt) {
    CheckStringLiteralToNonConstChar(stmt);
    CheckAddOrSubOnCharacter(stmt);
  }

  void VisitCallExpr(const clang::CallExpr *stmt) {
    CheckStringLiteralToNonConstChar(stmt);
  }

}; // MISRAStmtRule

}
}
