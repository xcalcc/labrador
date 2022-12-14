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
// GJB5369_stmt_rule.h
// ====================================================================
//
// implement all stmt related rules in GJB5369
//

#include <map>
#include "GJB5369_enum.inc"
#include "stmt_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class GJB5369StmtRule : public StmtNullHandler {
public:
  ~GJB5369StmtRule() {}

  GJB5369StmtRule() {
    _enable = XcalCheckerManager::GetEnableOption().getValue().find("GJB5369") != std::string::npos;
  }

private:
  bool _in_loop_now;

  bool _is_single_asm_stmt;

  bool _func_has_return_stmt;

  // check add overflow
  bool AddOverflowed(int a, int b);

  // check mul overflow
  bool MulOverflowed(int a, int b);

  // check if it start with parenthese
  bool CheckExprParentheses(const clang::Expr *expr);

  // check if it is case stmt
  bool IsCaseStmt(const clang::Stmt *stmt);

  // check if it contains assignment stmt
  bool HasAssignmentSubStmt(const clang::Stmt *stmt);

  // check if it contains bitwise
  bool HasBitwiseSubStmt(const clang::Stmt *stmt);

  // check if it contains call expr
  bool HasCallExpr(const clang::Stmt *stmt);

  // get builtin type
  clang::BuiltinType::Kind GetBuiltinTypeKind(const clang::QualType type);

  const clang::FunctionDecl * GetCalleeDecl(const clang::CallExpr *stmt);

  /*
   * GJB5369 4.1.1.4
   * Check multiple consecutive labels.
   */
  void CheckConsecutiveLabels(const clang::LabelStmt *stmt);

  /*
   * GJB5369: 4.2.1.2
   * the loop must be enclosed in braces
   */
  void CheckLoopBrace(const clang::WhileStmt *stmt);

  void CheckLoopBrace(const clang::ForStmt *stmt);

  /*
   * GJB5369: 4.2.1.3
   * if/else block must be enclosed in braces
   * INFO: MOVED TO MISRA 6-4-1
   */

  /*
   * GJB5369: 4.2.1.4
   * logic expression should be enclosed in parentheses
   */
  void CheckLogicExprParen(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.2.1.7
   * the assemble procedure should be pure assemble
   */
  void CheckAsmInProcedure(const clang::Stmt *stmt);

  void CheckAsmInProcedure(const clang::GCCAsmStmt *stmt);

  /*
   * GJB5369: 4.2.1.9
   * '\' used alone in a string is forbidden
   */
  void CheckStringLiteralEnd(const clang::StringLiteral *stmt);

  /*
   * GJB5369: 4.3.1.1
   * non-statement is forbidden as the conditional
   * judgement is true:
   * 1. if (...) else
   * 2. if (...) {} else
   * 3. if (...) {;} else
   *
   * GJB5369: 4.3.1.3
   * else block should not be empty
   * 1. else;
   * 2. else {}
   * 3. else {;}
   */
  void CheckEmptyIfElseStmt(const clang::IfStmt *stmt);

  /*
   * GJB5369: 4.3.1.2
   * 'else' must be used in the "if...else if" statement
   * INFO: MOVED TO MISRA 15.7
   */


  /*
   * GJB5369: 4.3.1.4
   * "default" statement should be used in the "switch" statement
   * INFO: MOVED TO MISRA
   *
   * GJB5369: 4.3.1.6
   * "switch" only containing "default" is forbidden
   */
  void CheckSwitchWithoutDefaultStmt(const clang::SwitchStmt *stmt);

  /*
   * GJB5369: 4.3.1.5
   * "switch" without statement is forbidden
   */
  bool CheckEmptySwitch(const clang::SwitchStmt *stmt);

  /*
   * GJB5369: 4.3.1.7
   * "case" statement without "break" is forbidden
   * INFO: MOVED TO MISRA 16.3
   */
  bool HasBreakStmt(const clang::Stmt *stmt);
  void CheckCaseEndWithBreak(const clang::SwitchStmt *stmt);

  /*
   * GJB5369: 4.3.1.8
   * the empty "case" statement is forbidden
   */
  void CheckEmptyCaseStmt(const clang::SwitchCase *stmt);

  /*
   * GJB5369: 4.4.1.2
   * pointer's pointer nested more than two levels is forbidden
   */
  void CheckPointerNestedLevel(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.4.2.1
   * comparing two pointer should be careful
   */
  void CheckPointerCompareStmt(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.4.2.2
   * using pointer in the algebraic operation should be careful
   */
  void CheckPointerCalculateStmt(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.5.2.1
   * setjmp/longjmp is forbidden
   */
  void CheckSetjumpAndLongjump(const clang::CallExpr *stmt);

  /*
   * GJB5369: 4.6.1.1
   * "=" used in non-assignment statement is forbidden
   */
  void CheckAssignInCondition(const clang::IfStmt *stmt);

  /*
   * GJB5369: 4.6.1.2
   * using array out of boundary is forbidden
   */
  void CheckArrayOutOfBoundary(const clang::ArraySubscriptExpr *stmt);

  /*
   * GJB5369: 4.6.1.3
   * bit shift used on signed-number is forbidden
   */
  template<typename TYPE>
  void CheckShiftOnSignedNumber(const TYPE *stmt) {
    if (!stmt->isShiftOp() && !stmt->isShiftAssignOp()) return;
    auto lhs = stmt->getLHS();
    auto lhs_type = lhs->getType();
    if (lhs_type->isSignedIntegerType()) {
      auto src_mgr = XcalCheckerManager::GetSourceManager();
      auto location = stmt->getBeginLoc();
      REPORT("GJB5396:4.6.1.3: bit shift used on signed-number is forbidden: %s\n",
             location.printToString(*src_mgr).c_str());
    }
  }

  /*
   * GJB5369: 4.6.1.4
   * shifting variable should not overflow
   */
  template<typename TYPE>
  void CheckShiftOverflow(const TYPE *stmt) {
    if (!stmt->isShiftOp() && !stmt->isShiftAssignOp()) return;
    auto lhs = stmt->getLHS();
    auto rhs = stmt->getRHS();
    auto lhs_type = lhs->getType();
    auto rhs_type = rhs->getType();
    if (lhs_type->isIntegerType()) {
      if (rhs_type->isIntegerType()) {
        if (clang::dyn_cast<clang::IntegerLiteral>(rhs)) {
          int oprand = clang::dyn_cast<clang::IntegerLiteral>(rhs)->getValue().getZExtValue();
          if (oprand > 32) {
            auto src_mgr = XcalCheckerManager::GetSourceManager();
            auto location = stmt->getBeginLoc();
            REPORT("GJB5396:4.6.1.4: shifting variable should not overflow: %s\n",
                   location.printToString(*src_mgr).c_str());
          }
        }
      }
    }
  }

  /*
   * GJB5369: 4.6.1.5
   * assigning negative number to unsigned-variable is forbidden
   */
  void CheckAssignNegativetoUnsigned(const clang::BinaryOperator *stmt) {
    if (!stmt->isAssignmentOp()) return;
    auto lhs_type = stmt->getLHS()->getType();
    if (lhs_type->isUnsignedIntegerType()) {
      auto rhs = clang::dyn_cast<clang::UnaryOperator>(stmt->getRHS());
      if (rhs && rhs->isArithmeticOp(clang::UnaryOperator::Opcode::UO_Minus)) {
        auto src_mgr = XcalCheckerManager::GetSourceManager();
        auto location = stmt->getBeginLoc();
        REPORT("GJB5396:4.6.1.5: assigning negative number to unsigned-variable is forbidden: %s\n",
               location.printToString(*src_mgr).c_str());
      }
    }
  }

  /*
   * GJB5369: 4.6.1.8
   * The value assigned to a variable should be the same type
   * as the variable
   */
  void CheckDifferentTypeAssign(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.6.1.11
   * logic non on const value is forbidden
   */
  void CheckNonOperationOnConstant(const clang::UnaryOperator *stmt);

  /*
   * GJB5369: 4.6.1.12
   * bit-wise operation on signed-int is forbidden
   */
  void CheckBitwiseOperationOnSignedValue(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.6.1.13
   *  using enumeration types beyond the limit if forbidden
   *  INFO: MOVED TO AUTOSAR A4-5-1
   */


  /*
   * GJB5369: 4.6.1.14
   * overflow should be avoided
   */
  void CheckArithmOverflow(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.6.1.15
   * '=' used in logical expression is forbidden
   */
  void CheckAssignInLogicExpr(const clang::IfStmt *stmt);

  /*
   * GJB5369: 4.6.1.16
   * "&&" or "||" used with "=" is forbidden
   */
  void CheckLogicalOpFollowedByAssign(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.6.1.17
   * bit-wise operation on bool is forbidden
   */
  void CheckBitwiseOpOnBool(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.6.1.18
   * bit-wise operation is forbidden in the boolean expression
   */
  void CheckBitwiseOpInBooleanExpr(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.6.2.1
   * avoid using ',' operator
   */
  void CheckCommaStmt(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.6.2.2
   * "sizeof()" should be used carefully
   */
  void CheckSizeofOnExpr(const clang::UnaryExprOrTypeTraitExpr *stmt);

  /*
   * GJB5369: 4.6.2.3
   * different types of variable mixed operation should be carefully
   */
  void CheckDifferentTypeArithm(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.6.2.4
   * dead code is forbidden
   */
  void CheckFalseIfCondition(const clang::IfStmt *stmt);

  /*
   * GJB5369: 4.7.1.6
   * Only one function call could be contain within one single statement
   */
  void CheckMultiCallExprInSingleStmt(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.7.1.7
   * function return void used in statement is forbidden
   */
  void CheckVoidReturnType(const clang::CallExpr *stmt);

  /*
   * GJB5369: 4.7.1.9
   * formal and real parameters' type should be the same
   */
  void CheckParamTypeMismatch(const clang::CallExpr *stmt);

  /*
   * GJB5369: 4.7.2.2
   * using function not by calling is forbidden
   */
  void CheckUsingFunctionNotByCalling(const clang::IfStmt *stmt);

  /*
   * GJB5369: 4.7.2.3
   * use abort/exit carefully
   */
  void CheckExitAndAbortFunction(const clang::CallExpr *stmt);

  /*
   * GJB5369: 4.8.2.1
   * avoid using += or -=
   */
  void CheckCompoundAssignOperator(const clang::CompoundAssignOperator *stmt);

  /*
   * GJB5369: 4.8.2.2
   * using ++ or -- should be carefully
   */
  void CheckPreIncrementAndPostIncrement(const clang::UnaryOperator *stmt);

  /*
   * GJB5369: 4.8.2.3
   * avoid using continue statement
   */
  void CheckContinueStmt(const clang::ContinueStmt *stmt);

  /*
   * GJB5369: 4.8.2.4
   * Binocular operation should be using carefully
   */
  void CheckBinocularOper(const clang::ConditionalOperator *stmt);

  /*
   * GJB5369: 4.8.2.6
   * avoid using Null statements
   */
  void CheckNullStmt(const clang::NullStmt *stmt);

  /*
   * GJB5369: 4.9.1.1
   * return statement is necessary for a function
   * @param: initial
   *  if "initial" is true, it means that it's caller is VisitFunctionBody().
   *  Otherwise, its caller is VisitAtFunctionExit().
   */
  void CheckReturnStmt(const clang::Stmt *stmt, bool initial);

  void CheckReturnStmt(const clang::ReturnStmt *stmt);

  /*
   * GJB5369: 4.9.1.4
   * type of return value should stay the same
   */
  void CheckReturnType(const clang::ReturnStmt *stmt);

  /*
   * GJB5369: 4.11.1.1
   * Inappropriate loop value type is forbidden
   * GJB5369: 4.11.1.2
   * loop value should be local value
   */
  void CheckLoopVariable(const clang::ForStmt *stmt);

  /*
  * GJB5369: 4.11.2.1
  * avoid using infinite loop
  * GJB5369: 4.11.2.3
  * avoid using uncontrollable loop value
  */
  template<typename _STMT_CLASS>
  void CheckInfiniteLoop(_STMT_CLASS *stmt) {
    bool need_report = false;
    const clang::Expr *cond = nullptr;
    auto ctx = XcalCheckerManager::GetAstContext();

    if (auto for_stmt = clang::dyn_cast<clang::ForStmt>(stmt)) {
      cond = for_stmt->getCond();
    } else if (auto while_stmt = clang::dyn_cast<clang::WhileStmt>(stmt)) {
      cond = while_stmt->getCond();
    } else if (auto do_stmt = clang::dyn_cast<clang::DoStmt>(stmt)) {
      cond = do_stmt->getCond();
    } else {
      DBG_WARN(1, "Cast loop-cond failed");
    }

    if (cond == nullptr) need_report = true;
    else if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(cond->IgnoreParenImpCasts())) {
      {
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();

        issue = report->ReportIssue(GJB5369, G4_11_2_3, stmt);
        std::string ref_msg = "Avoid using uncontrollable loop value";
        issue->SetRefMsg(ref_msg);
      }

      int value;
      clang::Expr::EvalResult eval_result;

      // try to fold the const expr
      if (literal->EvaluateAsInt(eval_result, *ctx)) {
        value = eval_result.Val.getInt().getZExtValue();
      } else {
        value = literal->getValue().getZExtValue();
      }
      if (value == 1) {
        need_report = true;
      }
    }

    if (need_report) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB5369, G4_11_2_1, stmt);
      std::string ref_msg = "Avoid using infinite loop";
      issue->SetRefMsg(ref_msg);
    }
  }

  /*
   * GJB5369: 4.11.2.2
   * avoid using break in a loop
   * TODO: break in SwitchStmt will be reported mistakenly
   */
  void CheckBreakInLoop(const clang::BreakStmt *stmt);
//  void CheckBreakInLoop

  /*
   * GJB5369: 4.12.1.1
   * cast on pointer is forbidden
   */
  void CheckPointerCast(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.12.2.2
   * avoid using unnecessary cast
   */
  void CheckUnnessaryCast(const clang::CStyleCastExpr *stmt);

  /*
   * GJB5369: 4.12.2.3
   * pay attention to the type of operator of binocular operation
   */
  void CheckConditionalOperType(const clang::ConditionalOperator *stmt);

  /*
   * GJB5369: 4.13.1.2
   * initial value type of struct should stay the same with struct
   */
  void CheckRecordInitType(const clang::InitListExpr *stmt);

  /*
   * GJB5369: 4.14.1.1
   * avoid comparing two real numbers
   */
  void CheckComparingRealNumber(const clang::BinaryOperator *stmt);

  /*
   * GJB5369: 4.14.1.2
   * Logical discriminant can only be logical expression
   * TODO: false positive (i == 0) || (j == 2)
   */
  template<typename _STMT_CLASS>
  void CheckLogicalDiscriminant(_STMT_CLASS *stmt) {
    bool need_report = false;
    const clang::Expr *cond = nullptr;

    if (auto for_stmt = clang::dyn_cast<clang::ForStmt>(stmt)) {
      cond = for_stmt->getCond()->IgnoreParenImpCasts();
    } else if (auto while_stmt = clang::dyn_cast<clang::WhileStmt>(stmt)) {
      cond = while_stmt->getCond()->IgnoreParenImpCasts();
    } else if (auto do_stmt = clang::dyn_cast<clang::DoStmt>(stmt)) {
      cond = do_stmt->getCond()->IgnoreParenImpCasts();
    } else if (auto if_stmt = clang::dyn_cast<clang::IfStmt>(stmt)) {
      cond = if_stmt->getCond()->IgnoreParenImpCasts();
    }
    else {
      DBG_WARN(1, "Cast loop-cond failed");
    }

    if (cond == nullptr) return;
    if (!cond->getType()->isBooleanType()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB5369, G4_14_1_2, stmt);
      std::string ref_msg = "Logical discriminant can only be logical expression";
      issue->SetRefMsg(ref_msg);
    }
  }

  /*
   * GJB5369: 4.14.1.3
   * logical expression is forbidden in switch statement
   */
  void CheckLogicalStmtInSwitchCond(const clang::SwitchStmt *stmt);

  /*
   * GJB5369: 4.14.2.1
   * avoid using complex logical expression
   */
  void CheckComplexLogicalExpr(const clang::UnaryOperator *stmt);


public:
  void Finalize() {}

  void VisitLabelStmt(const clang::LabelStmt *stmt) {
    CheckConsecutiveLabels(stmt);
  }

  void VisitWhileStmt(const clang::WhileStmt *stmt) {
    CheckLoopBrace(stmt);
    CheckInfiniteLoop<const clang::WhileStmt>(stmt);
    CheckLogicalDiscriminant<const clang::WhileStmt>(stmt);
  }

  void VisitForStmt(const clang::ForStmt *stmt) {
    CheckLoopBrace(stmt);
    CheckLoopVariable(stmt);
    CheckInfiniteLoop<const clang::ForStmt>(stmt);
    CheckLogicalDiscriminant<const clang::ForStmt>(stmt);
  }

  void VisitDoStmt(const clang::DoStmt *stmt) {
    CheckInfiniteLoop<const clang::DoStmt>(stmt);
    CheckLogicalDiscriminant<const clang::DoStmt>(stmt);
  }

  void VisitIfStmt(const clang::IfStmt *stmt) {
    CheckEmptyIfElseStmt(stmt);
    CheckAssignInLogicExpr(stmt);
    CheckFalseIfCondition(stmt);
    CheckAssignInCondition(stmt);
    CheckUsingFunctionNotByCalling(stmt);
    CheckLogicalDiscriminant<const clang::IfStmt>(stmt);
  }

  void VisitBinaryOperator(const clang::BinaryOperator *stmt) {
    CheckLogicExprParen(stmt);
    CheckPointerCompareStmt(stmt);
    CheckPointerCalculateStmt(stmt);
    CheckShiftOnSignedNumber<clang::BinaryOperator>(stmt);
    CheckShiftOverflow<clang::BinaryOperator>(stmt);
    CheckAssignNegativetoUnsigned(stmt);
    CheckDifferentTypeAssign(stmt);
    CheckBitwiseOperationOnSignedValue(stmt);
    CheckArithmOverflow(stmt);
    CheckLogicalOpFollowedByAssign(stmt);
    CheckBitwiseOpOnBool(stmt);
    CheckBitwiseOpInBooleanExpr(stmt);
    CheckCommaStmt(stmt);
    CheckDifferentTypeArithm(stmt);
    CheckMultiCallExprInSingleStmt(stmt);
    CheckPointerCast(stmt);
    CheckComparingRealNumber(stmt);
    CheckPointerNestedLevel(stmt);
  }

  void VisitFunctionBody(const clang::Stmt *stmt) {
    CheckAsmInProcedure(stmt);
    CheckReturnStmt(stmt, true);
  }

  void VisitGCCAsmStmt(const clang::GCCAsmStmt *stmt) {
    CheckAsmInProcedure(stmt);
  }

  void VisitStringLiteral(const clang::StringLiteral *stmt) {
    CheckStringLiteralEnd(stmt);
  }

  void VisitSwitchStmt(const clang::SwitchStmt *stmt) {
    CheckSwitchWithoutDefaultStmt(stmt);
    CheckCaseEndWithBreak(stmt);
    CheckLogicalStmtInSwitchCond(stmt);
  }

  void VisitGotoStmt(const clang::GotoStmt *stmt) {
  }

  void VisitCallExpr(const clang::CallExpr *stmt) {
    CheckSetjumpAndLongjump(stmt);
    CheckParamTypeMismatch(stmt);
    CheckVoidReturnType(stmt);
    CheckExitAndAbortFunction(stmt);
  }

  void VisitCompoundAssignOperator(const clang::CompoundAssignOperator *stmt) {
    CheckShiftOnSignedNumber<clang::CompoundAssignOperator>(stmt);
    CheckShiftOverflow<clang::CompoundAssignOperator>(stmt);
    CheckCompoundAssignOperator(stmt);
  }

  void VisitUnaryOperator(const clang::UnaryOperator *stmt) {
    CheckNonOperationOnConstant(stmt);
    CheckPreIncrementAndPostIncrement(stmt);
    CheckComplexLogicalExpr(stmt);
  }

  void VisitUnaryExprOrTypeTraitExpr(const clang::UnaryExprOrTypeTraitExpr *stmt) {
    CheckSizeofOnExpr(stmt);
  }

  void VisitContinueStmt(const clang::ContinueStmt *stmt) {
    CheckContinueStmt(stmt);
  }

  void VisitConditionalOperator(const clang::ConditionalOperator *stmt) {
    CheckBinocularOper(stmt);
    CheckConditionalOperType(stmt);
  }

  void VisitNullStmt(const clang::NullStmt *stmt) {
    CheckNullStmt(stmt);
  }

  void VisitReturnStmt(const clang::ReturnStmt *stmt) {
    CheckReturnStmt(stmt);
    CheckReturnType(stmt);
  }

  void VisitCompoundStmt(const clang::CompoundStmt *stmt) {
  }

  void VisitAtFunctionExit(const clang::Stmt *stmt) {
    _current_function_decl = nullptr;
    CheckReturnStmt(stmt, false);
  }

  void VisitBreakStmt(const clang::BreakStmt *stmt) {
    CheckBreakInLoop(stmt);
  }

  void VisitCStyleCastExpr(const clang::CStyleCastExpr *stmt) {
    CheckUnnessaryCast(stmt);
  }

  void VisitInitListExpr(const clang::InitListExpr *stmt) {
    CheckRecordInitType(stmt);
  }

  void VisitArraySubscriptExpr(const clang::ArraySubscriptExpr *stmt) {
    CheckArrayOutOfBoundary(stmt);
  }

}; // GJB5369StmtRule

}
}
