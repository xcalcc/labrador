/*
   Copyright (C) 2021 Xcalibyte (Shenzhen) Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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

  MISRAStmtRule() {
    _enable = true;
  }

private:
  std::unordered_set<const clang::Stmt *> _terminates;

  clang::QualType GetRawTypeOfTypedef(clang::QualType type);

  void HasThisFunctionThenReport(const std::vector<std::string> &fid_func, const std::string &str,
                                 const clang::CallExpr *stmt, const char *std_id, const std::string &info);

  const clang::FunctionDecl *GetCalleeDecl(const clang::CallExpr *stmt);

  // collect object types within try block
  std::vector <clang::QualType> RecordThrowObjectTypes(const clang::Stmt *stmt);

  clang::BuiltinType::Kind UnifyBTTypeKind(const clang::BuiltinType::Kind &kind);

  // check if the expr is an IntegerLiteral expression
  bool IsIntegerLiteralExpr(const clang::Expr *expr);

  // check if the expr has side effect
  bool HasSideEffect(const clang::Stmt *stmt);

  // check if the expr has Inc/Dec expr
  bool HasIncOrDecExpr(const clang::Stmt *stmt);

  // report template
  void ReportTemplate(const std::string &str, const char *rule, const clang::Stmt *stmt);

  /* MISRA
   * Rule: 4.1
   * Octal and hexadecimal escape sequences shall be terminated
   */
  void CheckOctalAndHexadecimalEscapeWithoutTerminated(const clang::StringLiteral *stmt);

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

  /*
   * MISRA: 10.3
   * The value of an expression shall not be assigned to an object with a narrower
   * essential type or of a different essential type category
   */
  void CheckIntToShorter(const clang::BinaryOperator *stmt);

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
   * MOVED TO AUTOSAR A5-0-3
   */

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
   * Rule: 11.1
   * Conversions shall not be performed between a pointer to a function and any other type
   */
  void CheckCastFunctionPointerType(const clang::CStyleCastExpr *stmt);

  /* MISRA
   * Rule: 11.2
   * Conversions shall not be performed between a pointer to an incomplete type and any other type
   */
  void CheckIncompleteTypePointerCastToAnotherType(const clang::CStyleCastExpr *stmt);

  /* MISRA
   * Rule: 11.3
   * A cast shall not be performed between a pointer to object type and a pointer to a different object type
   */
  void CheckCastPointerToDifferentType(const clang::CStyleCastExpr *stmt);

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
   * Rule: 12.4
   * Evaluation of constant expressions should not lead to unsigned integer wrap-around
   */
  void CheckUnsignedIntWrapAround(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 13.1
   * Initializer lists shall not contain persistent side effects
   */
  void CheckSideEffectWithinInitListExpr(const clang::InitListExpr *stmt);

  /* MISRA
   * Rule: 13.2
   * The value of an expression and its persistent side
   * effects shall be the same under all permitted evaluation orders
   */
  bool isInc(const clang::Expr *expr);
  void CheckSideEffectWithOrder(const clang::BinaryOperator *stmt);
  void CheckSideEffectWithOrder(const clang::CallExpr *stmt);
  void ReportSideEffect(const clang::Stmt *stmt);

  /* MISRA
   * Rule: 13.3
   * A full expression containing an increment (++) or decrement (--) operator should have no other potential side
   * effects other than that caused by the increment or decrement operator
   */
  void CheckMultiIncOrDecExpr(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 13.4
   * The result of an assignment operator should not be used
   */
  bool IsAssignmentStmt(const clang::Stmt *stmt);
  void ReportAssignment(const clang::Stmt *stmt);
  void CheckUsingAssignmentAsResult(const clang::ArraySubscriptExpr *stmt);
  void CheckUsingAssignmentAsResult(const clang::BinaryOperator *stmt);

  template<typename T>
  void CheckUsingAssignmentAsResult(const T *stmt) {
    auto cond = stmt->getCond()->IgnoreParenImpCasts();
    if (cond && IsAssignmentStmt(cond)) ReportAssignment(stmt);
  }

  /* MISRA
   * Rule: 13.5
   * The right hand operand of a logical && or || operator shall not contain
   * persistent side effects
   */
  void CheckRHSOfLogicalOpHasSideEffect(const clang::BinaryOperator *stmt);

  /* MISRA
   * Rule: 13.6
   * The operand of the sizeof operator shall not contain any expression which
   * has potential side effects
   */
  void CheckSideEffectInSizeof(const clang::UnaryExprOrTypeTraitExpr *stmt);

  /* MISRA
   * Rule: 14.1
   * A loop counter shall not have essentially floating type
   */
  void ReportLoopVariable(const clang::Stmt *stmt);
  void CheckLoopVariable(const clang::ForStmt *stmt);
  template <typename T>
  void CheckLoopVariable(const T *stmt) {
    auto cond = stmt->getCond();
    if (auto bin_inst = clang::dyn_cast<clang::BinaryOperator>(cond)) {
      if (bin_inst->getLHS()->IgnoreParenCasts()->getType()->isFloatingType() ||
          bin_inst->getRHS()->IgnoreParenCasts()->getType()->isFloatingType()) {
        ReportLoopVariable(cond);
      }
    }
  }


  /* MISRA
   * Rule: 14.4
   * The controlling expression of an if statement and the controlling expression
   * of an iteration-statement shall have essentially Boolean type
   */
  void CheckControlStmtImpl(const clang::Expr *stmt);

  template<typename T>
  void CheckControlStmt(const T *stmt) {
    CheckControlStmtImpl(stmt->getCond()->IgnoreParenImpCasts());
  }

  /*
   * MISRA: 15.1
   * The goto statement should not be used
   */
  void CheckGotoStmt(const clang::GotoStmt *stmt);

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

  /* MISRA
   * Rule: 15.7
   * All if ... else if constructs shall be terminated with an else statement
   */
  void CheckIfWithoutElseStmt(const clang::IfStmt *stmt);

  /* MISRA
   * Rule: 16.4
   * Every switch statement shall have a default label
   */
  void CheckSwitchWithoutDefault(const clang::SwitchStmt *stmt);

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
   * Rule: 21.6
   * The Standard Library input/output functions shall not be used
   */
  void CheckIOFunctionInStdio(const clang::CallExpr *stmt);

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

  /* MISRA
   * Rule: 21.10
   * The Standard Library time and date functions shall not be used
   */
  void CheckTimeFunctionInStdlib(const clang::CallExpr *stmt);

  /* MISRA
   * Rule: 21.12
   * The exception handling features of <fenv.h> should not be used
   */
  void CheckExceptionFeaturesInFenv(const clang::CallExpr *stmt);

  /* MISRA
   * Rule: 5-2-3
   * cast from base class to derived class cannot have polymorphic type
   */
  void CheckDownCastToDerivedClass(const clang::CastExpr *stmt);

  /* MISRA
   * Rule: 12-1-1
   * ctor and dtor cannot use dynamic type
   */
  template<typename TYPE>
  void CheckDynamicTypeInCtorAndDtor(const TYPE *stmt) {
    ReportDynamicInCTorAndDtor(stmt);
  }
  void CheckDynamicTypeInCtorAndDtor(const clang::CXXMemberCallExpr *stmt);
  void ReportDynamicInCTorAndDtor(const clang::Stmt *stmt);

  /*
   * QUAN ZHI temp
   */
  void CheckBitwiseWithOutParen(const clang::BinaryOperator *stmt);

  /*
   * MISRA: 4-5-1
   * Expressions with type bool shall not be used as operands to built-in operators
   * other than the assignment operator =, the logical operators &&, ||, !, the
   * equality operators == and !=, the unary & operator, and the conditional operator.
   */
  void CheckBoolUsedAsNonLogicalOperand(const clang::BinaryOperator *stmt);

  /*
   * MISRA: 4-10-2
   * Literal zero (0) shall not be used as the null-pointer- constant.
   */
  void CheckUsingNullWithPointer(const clang::BinaryOperator *stmt);

  /*
   * MISRA: 6-4-1
   * An if ( condition ) construct shall be followed by a compound statement.
   * The else keyword shall be followed by either a compound statement, or
   * another if statement.
   */
  void CheckIfBrace(const clang::IfStmt *stmt);

  /*
   * MISRA: 6-5-2
   * If loop-counter is not modified by -- or ++, then, within condition, the loop-counter
   * shall only be used as an operand to <=, <, > or >=.
   */
  void CheckForStmtLoopCounter(const clang::ForStmt *stmt);

  /*
   * MISRA: 15-0-2
   * An exception object should not have pointer type.
   */
  void CheckThrowPointer(const clang::CXXThrowExpr *stmt);

  /*
   * MISRA: 15-1-1
   * The assignment-expression of a throw statement shall not itself cause an exception to be thrown.
   */
  void CheckThrowExceptionItselfHasThrow(const clang::CXXThrowExpr *stmt);

  /*
   * MISRA: 15-1-2
   * NULL shall not be thrown explicitly.
   */
  void CheckThrowNullExpr(const clang::CXXThrowExpr *stmt);

  /*
   * MISRA: 15-1-3
   * An empty throw (throw;) shall only be used in the compound-statement of a catch handler.
   */
  void CheckEmptyThrowInNonCatchBlock(const clang::CXXThrowExpr *stmt);

  /*
   * MISRA: 15-3-2
   * There should be at least one exception handler to catch all otherwise unhandled exceptions
   */
  void CheckTryWithoutDefaultCatch(const clang::CXXTryStmt *stmt);

  /*
   * MISRA: 15-3-4
   * Each specified throw must have a matching catch
   */
  void CheckMissingCatchStmt(const clang::CXXTryStmt *stmt);

  /*
   * MISRA: 15-3-5
   * A class type exception shall always be caught by reference.
   */
  void CheckCatchTypeNotReference(const clang::CXXCatchStmt *stmt);

  /*
   * MISRA: 15-5-1
   * A class destructor shall not exit with an exception.
   */
  void CheckDTorExitWithThrow(const clang::CXXThrowExpr *stmt);

  /*
   * MISRA: 15-5-2
   * Where a function’s declaration includes an exception- specification, the function
   * shall only be capable of throwing exceptions of the indicated type(s).
   */
  void CollectThrowType(const clang::CXXThrowExpr *stmt);
  void CollectThrowType(const clang::CallExpr *stmt);

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
    CheckSideEffectWithOrder(stmt);
    CheckBitwiseWithOutParen(stmt);
    CheckUnsignedIntWrapAround(stmt);
    CheckRHSOfLogicalOpHasSideEffect(stmt);
    CheckMultiIncOrDecExpr(stmt);
    CheckIntToShorter(stmt);
    CheckBoolUsedAsNonLogicalOperand(stmt);
    CheckUsingNullWithPointer(stmt);
  }

  void VisitCompoundAssignOperator(const clang::CompoundAssignOperator *stmt) {
    CheckArithmeticWithDifferentType(stmt);
    CheckAddOrSubOnPointer(stmt);
    CheckModifyParameters(stmt);
  }

  void VisitCallExpr(const clang::CallExpr *stmt) {
    CheckStringLiteralToNonConstChar(stmt);
    CheckArrayArgumentSize(stmt);
    CheckUnusedCallExprWithoutVoidCast(stmt);
    CheckStdMemoryAllocationFunction(stmt);
    CheckIOFunctionInStdio(stmt);
    CheckIntConvertFunctionInStdlib(stmt);
    CheckSystemFuncInStdlib(stmt);
    CheckBsearchAndQsortInStdlib(stmt);
    CheckTimeFunctionInStdlib(stmt);
    CheckExceptionFeaturesInFenv(stmt);
    CheckSideEffectWithOrder(stmt);
    CollectThrowType(stmt);
  }

  void VisitCStyleCastExpr(const clang::CStyleCastExpr *stmt) {
    CheckCompositeExprCastToWiderType(stmt);
    CheckCastBetweenIntAndPointer(stmt);
    CheckVoidPointerToOtherTypePointer(stmt);
    CheckArithTypeCastToVoidPointerType(stmt);
    CheckCastBetweenPointerAndNonIntType(stmt);
    CheckIncompleteTypePointerCastToAnotherType(stmt);
    CheckCastPointerToDifferentType(stmt);
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
    CheckUsingAssignmentAsResult(stmt);
    CheckIfWithoutElseStmt(stmt);
    CheckIfBrace(stmt);
  }

  void VisitWhileStmt(const clang::WhileStmt *stmt) {
    CheckControlStmt(stmt);
    CheckLoopVariable(stmt);
    CheckMultiTerminate(stmt);
    CheckUsingAssignmentAsResult(stmt);
  }

  void VisitDoStmt(const clang::DoStmt *stmt) {
    CheckControlStmt(stmt);
    CheckMultiTerminate(stmt);
    CheckUsingAssignmentAsResult(stmt);
  }

  void VisitForStmt(const clang::ForStmt *stmt) {
    CheckControlStmt(stmt);
    CheckMultiTerminate(stmt);
    CheckLoopVariable(stmt);
    CheckUsingAssignmentAsResult(stmt);
    CheckForStmtLoopCounter(stmt);
  }

  void VisitGotoStmt(const clang::GotoStmt *stmt) {
    CheckGotoStmt(stmt);
    CheckGotoBackward(stmt);
    CheckLabelNotEncloseWithGoto(stmt);
  }

  void VisitSwitchStmt(const clang::SwitchStmt *stmt) {
    CheckSwitchWithoutDefault(stmt);
    CheckDefaultStmtPosition(stmt);
    CheckCaseStmtNum(stmt);
  }

  void VisitCXXTypeidExpr(const clang::CXXTypeidExpr *stmt) {
    CheckDynamicTypeInCtorAndDtor(stmt);
  }

  void VisitCXXDynamicCastExpr(const clang::CXXDynamicCastExpr *stmt) {
    CheckDynamicTypeInCtorAndDtor(stmt);
    CheckDownCastToDerivedClass(stmt);
  }

  void VisitCXXStaticCastExpr(const clang::CXXStaticCastExpr *stmt) {
    CheckDownCastToDerivedClass(stmt);
  };

  void VisitCXXMemberCallExpr(const clang::CXXMemberCallExpr *stmt) {
    CheckDynamicTypeInCtorAndDtor(stmt);
  }

  void VisitAtFunctionExit(const clang::Stmt *stmt) {
    _current_function_decl = nullptr;
  }

  void VisitCXXThrowExpr(const clang::CXXThrowExpr *stmt) {
    CheckThrowPointer(stmt);
    CheckThrowExceptionItselfHasThrow(stmt);
    CheckThrowNullExpr(stmt);
    CheckEmptyThrowInNonCatchBlock(stmt);
    CheckDTorExitWithThrow(stmt);
    CollectThrowType(stmt);
  }

  void VisitCXXTryStmt(const clang::CXXTryStmt *stmt) {
    CheckTryWithoutDefaultCatch(stmt);
    CheckMissingCatchStmt(stmt);
  }

  void VisitCXXCatchStmt(const clang::CXXCatchStmt *stmt) {
    CheckCatchTypeNotReference(stmt);
  }

  void VisitStringLiteral(const clang::StringLiteral *stmt) {
    CheckOctalAndHexadecimalEscapeWithoutTerminated(stmt);
  }

  void VisitInitListExpr(const clang::InitListExpr *stmt) {
    CheckSideEffectWithinInitListExpr(stmt);
  }

  void VisitUnaryExprOrTypeTraitExpr(const clang::UnaryExprOrTypeTraitExpr *stmt) {
    CheckSideEffectInSizeof(stmt);
  }

}; // MISRAStmtRule

}
}
