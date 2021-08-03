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
#include <unordered_set>

namespace xsca {
namespace rule {

class MISRAStmtRule : public StmtNullHandler {
public:
  ~MISRAStmtRule() = default;

private:
  std::unordered_set<const clang::Stmt *> _terminates;

  void HasThisFunctionThenReport(const std::vector<std::string> &fid_func, const std::string &str,
                                 const clang::Stmt *stmt, const std::string &std_id, const std::string &info);

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

  /* MISRA
   * Rule: 13.4
   * The result of an assignment operator should not be used
   */
  bool IsAssignmentStmt(const clang::Stmt *stmt);

  void CheckUsingAssignmentAsResult(const clang::ArraySubscriptExpr *stmt);

  void CheckUsingAssignmentAsResult(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 14.4
   * The controlling expression of an if statement and the controlling expression
   * of an iteration-statement shall have essentially Boolean type
   */
  bool CheckControlStmt(const clang::Expr *stmt);
  void CheckControlStmt(const clang::IfStmt *stmt);
  void CheckControlStmt(const clang::WhileStmt *stmt);
  void CheckControlStmt(const clang::DoStmt *stmt);
  void CheckControlStmt(const clang::ForStmt *stmt);

  /* MISRA
   * Rule: 15.2
   * The goto statement shall jump to a label declared later in the same function
   */
  void CheckGotoBackward(const clang::GotoStmt *stmt);

  /* MISRA
   * Rule: 15.3
   * Any label referenced by a goto statement shall be declared in
   * the same block, or in any block enclosing the goto statement
   */
  void CheckLabelNotEncloseWithGoto(const clang::GotoStmt *stmt);

  /* MISRA
   * Rule: 15.4
   * There should be no more than one break or goto statement used to terminate any iteration statement
   */
  void CollectTerminate(const clang::Stmt *stmt);
  void CheckMultiTerminate(const clang::Stmt *stmt);

  template<typename TYPE>
  void CheckMultiTerminate(const TYPE *stmt) {
    _terminates.clear();
    CheckMultiTerminate(stmt->getBody());
  }

  template<>
  void CheckMultiTerminate<clang::IfStmt>(const clang::IfStmt *stmt) {
    _terminates.clear();
    CheckMultiTerminate(stmt->getThen());
    CheckMultiTerminate(stmt->getElse());
  }

  /* MISRA
   * Rule: 16.5
   * A default label shall appear as either the first or the last switch label of a switch statement
   */
  void CheckDefaultStmtPosition(const clang::SwitchStmt *stmt);

  /* MISRA
   * Rule: 16.6
   * Every switch statement shall have at least two switch-clauses
   */
  void CheckCaseStmtNum(const clang::SwitchStmt *stmt);

  /* MISRA
   * Rule: 17.5
   * The function argument corresponding to a parameter declared to have an
   * array type shall have an appropriate number of elements
   */
  void CheckArrayArgumentSize(const clang::CallExpr *stmt);

  /* MISRA
   * Rule: 17.7
   * The value returned by a function having non-void return type shall be used
   */
  void CheckUnusedCallExprWithoutVoidCast(const clang::CallExpr *stmt);

  /* MISRA
   * Rule: 17.8
   * A function parameter should not be modified
   */
  void CheckModifyParameters(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 18.4
   * The +, -, += and -= operators should not be applied to an expression of pointer type
   */
  void CheckAddOrSubOnPointer(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 21.3
   * The memory allocation and deallocation functions of <stdlib.h> shall not be used
   */
  void CheckStdMemoryAllocationFunction(const clang::CallExpr *stmt);

  /* MISRA
   * Rule: 21.7
   * The atof, atoi, atol and atoll functions of <stdlib.h> shall not be used
   */
  void CheckIntConvertFunctionInStdlib(const clang::CallExpr *stmt);

  /* MISRA
   * Rule: 21.8
   * The library functions abort, exit, getenv and system of <stdlib.h> shall not be used
   */
  void CheckSystemFuncInStdlib(const clang::CallExpr *stmt);

  /* MISRA
   * Rule: 21.9
   * The library functions bsearch and qsort of <stdlib.h> shall not be used
   */
  void CheckBsearchAndQsortInStdlib(const clang::CallExpr *stmt);


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
    CheckUsingAssignmentAsResult(stmt);
    CheckModifyParameters(stmt);
    CheckAddOrSubOnPointer(stmt);
  }

  void VisitCompoundAssignOperator(const clang::CompoundAssignOperator *stmt) {
    CheckArithmeticWithDifferentType(stmt);
    CheckAddOrSubOnPointer(stmt);
  }

  void VisitCallExpr(const clang::CallExpr *stmt) {
    CheckStringLiteralToNonConstChar(stmt);
    CheckArrayArgumentSize(stmt);
    CheckUnusedCallExprWithoutVoidCast(stmt);
    CheckStdMemoryAllocationFunction(stmt);
    CheckIntConvertFunctionInStdlib(stmt);
    CheckSystemFuncInStdlib(stmt);
    CheckBsearchAndQsortInStdlib(stmt);
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

  void VisitArraySubscriptExpr(const clang::ArraySubscriptExpr *stmt) {
    CheckUsingAssignmentAsResult(stmt);
  }

  void VisitIfStmt(const clang::IfStmt *stmt) {
    CheckControlStmt(stmt);
    CheckMultiTerminate(stmt);
  }

  void VisitWhileStmt(const clang::WhileStmt *stmt) {
    CheckControlStmt(stmt);
    CheckMultiTerminate(stmt);
  }

  void VisitDoStmt(const clang::DoStmt *stmt) {
    CheckControlStmt(stmt);
    CheckMultiTerminate(stmt);
  }

  void VisitForStmt(const clang::ForStmt *stmt) {
    CheckControlStmt(stmt);
    CheckMultiTerminate(stmt);
  }

  void VisitGotoStmt(const clang::GotoStmt *stmt) {
    CheckGotoBackward(stmt);
    CheckLabelNotEncloseWithGoto(stmt);
  }

  void VisitSwitchStmt(const clang::SwitchStmt *stmt) {
    CheckDefaultStmtPosition(stmt);
    CheckCaseStmtNum(stmt);
  }

}; // MISRAStmtRule

}
}
