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
   * A string literal shall not be assigned to an object unless the object’s
   * type is “pointer to const-qualified char”
   */
  void CheckStringLiteralToNonConstChar(const clang::BinaryOperator *stmt);

  void CheckStringLiteralToNonConstChar(const clang::CallExpr *stmt);

  /* MISRA
   * Rule: 10.2
   * Expressions of essentially character type shall not be used inappropriately
   * in addition and subtraction operations
   */
  void CheckAddOrSubOnCharacter(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 10.4
   * Both operands of an operator in which the usual arithmetic conversions are performed
   * shall have the same essential type category
   */
  bool IsTypeFit(clang::QualType lhs_type, clang::QualType rhs_type);
  void CheckArithmeticWithDifferentType(const clang::BinaryOperator *stmt);
  void CheckArithmeticWithDifferentType(const clang::CompoundAssignOperator *stmt);

public:

  void VisitBinaryOperator(const clang::BinaryOperator *stmt) {
    CheckStringLiteralToNonConstChar(stmt);
    CheckAddOrSubOnCharacter(stmt);
    CheckArithmeticWithDifferentType(stmt);
  }

  void VisitCompoundAssignOperator(const clang::CompoundAssignOperator *stmt) {
    CheckArithmeticWithDifferentType(stmt);
  }

  void VisitCallExpr(const clang::CallExpr *stmt) {
    CheckStringLiteralToNonConstChar(stmt);
  }

}; // MISRAStmtRule

}
}
