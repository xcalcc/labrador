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

  /* MISRA
   * Rule: 10.5
   * The value of an expression should not be cast to an inappropriate essential type
   */
  void CheckInappropriateCast(const clang::CStyleCastExpr *stmt);

  /* MISRA
   * Rule: 10.6
   * The value of a composite expression shall not be assigned to an object with wider essential type
   */
  void CheckCompositeExprAssignToWiderTypeVar(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 10.7
   * If a composite expression is used as one operand of an operator in which the usual
   * arithmetic conversions are performed then the other operand shall not have wider essential type
   */
  void CheckCompositeMixTypeExpr(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 10.8
   * The value of a composite expression shall not be cast to a different
   * essential type category or a wider essential type
   */
  void CheckCompositeExprCastToWiderType(const clang::CStyleCastExpr *stmt);

  /* MISRA
   * Rule: 11.4
   * A conversion should not be performed between a pointer to object and an integer type
   */
  void CheckCastBetweenIntAndPointer(const clang::CastExpr *stmt);

  /* MISRA
   * Rule: 11.5
   * A conversion should not be performed from pointer to void into pointer to object
   */
  void CheckVoidPointerToOtherTypePointer(const clang::CastExpr *stmt);

  /* MISRA
   * Rule: 11.6
   * A cast shall not be performed between pointer to void and an arithmetic type
   */
  void CheckArithTypeCastToVoidPointerType(const clang::CastExpr *stmt);

  /* MISRA
   * Rule: 11.7
   * A cast shall not be performed between pointer to object and a non-integer arithmetic type
   */
  void CheckCastBetweenPointerAndNonIntType(const clang::CastExpr *stmt);

  /* MISRA
   * Rule: 11.8
   * A cast shall not remove any const or volatile qualification from the type pointed to by a pointer
   */
  void CheckAssignRemoveConstOrVolatile(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 11.9
   * The macro NULL shall be the only permitted form of integer null pointer constant
   */
  void CheckZeroAsPointerConstant(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 12.2
   * The right hand operand of a shift operator shall lie in the range zero to one less than the
   * width in bits of the essential type of the left hand operand
   */
  void CheckShiftOutOfRange(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 12.3
   * The comma operator should not be used
   */
  void CheckCommaStmt(const clang::BinaryOperator *stmt);

public:

  void VisitBinaryOperator(const clang::BinaryOperator *stmt) {
    CheckStringLiteralToNonConstChar(stmt);
    CheckAddOrSubOnCharacter(stmt);
    CheckArithmeticWithDifferentType(stmt);
    CheckCompositeMixTypeExpr(stmt);
    CheckCompositeExprAssignToWiderTypeVar(stmt);
    CheckAssignRemoveConstOrVolatile(stmt);
    CheckZeroAsPointerConstant(stmt);
    CheckShiftOutOfRange(stmt);
    CheckCommaStmt(stmt);
  }

  void VisitCompoundAssignOperator(const clang::CompoundAssignOperator *stmt) {
    CheckArithmeticWithDifferentType(stmt);
  }

  void VisitCallExpr(const clang::CallExpr *stmt) {
    CheckStringLiteralToNonConstChar(stmt);
  }

  void VisitCStyleCastExpr(const clang::CStyleCastExpr *stmt) {
    CheckInappropriateCast(stmt);
    CheckCompositeExprCastToWiderType(stmt);
    CheckCastBetweenIntAndPointer(stmt);
    CheckVoidPointerToOtherTypePointer(stmt);
    CheckArithTypeCastToVoidPointerType(stmt);
    CheckCastBetweenPointerAndNonIntType(stmt);
  }

  void VisitImplicitCastExpr(const clang::ImplicitCastExpr *stmt) {
    CheckCastBetweenIntAndPointer(stmt);
    CheckVoidPointerToOtherTypePointer(stmt);
    CheckArithTypeCastToVoidPointerType(stmt);
    CheckCastBetweenPointerAndNonIntType(stmt);
  }

}; // MISRAStmtRule

}
}
