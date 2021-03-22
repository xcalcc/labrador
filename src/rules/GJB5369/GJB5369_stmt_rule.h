/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB5369_stmt_rule.h
// ====================================================================
//
// implement all stmt related rules in GJB5369
//

#include <map>
#include "stmt_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class GJB5369StmtRule : public StmtNullHandler {
public:
  ~GJB5369StmtRule() {}

private:
  bool _is_single_asm_stmt;

  bool CheckExprParentheses(const clang::Expr *expr);

  bool IsCaseStmt(const clang::Stmt *stmt);

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
   */
  void CheckIfBrace(const clang::IfStmt *stmt);

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
   */
  void CheckIfWithoutElseStmt(const clang::IfStmt *stmt);

  /*
   * GJB5369: 4.3.1.4
   * "default" statement should be used in the "switch" statement
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
   */
  void CheckCaseEndWithBreak(const clang::SwitchStmt *stmt);

  /*
   * GJB5369: 4.3.1.8
   * the empty "case" statement is forbidden
   */
  void CheckEmptyCaseStmt(const clang::SwitchCase *stmt);

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
   * GJB5369: 4.5.1.2
   * "goto" statement is forbidden
   */
  void CheckGotoStmt(const clang::GotoStmt *stmt);

  /*
   * GJB5369: 4.5.2.1
   * setjmp/longjmp is forbidden
   */
  void CheckSetjumpAndLongjump(const clang::CallExpr *stmt);

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
          auto oprand_addr = clang::dyn_cast<clang::IntegerLiteral>(rhs)->getValue().getRawData();
          int oprand = *oprand_addr;
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

public:
  void VisitLabelStmt(const clang::LabelStmt *stmt) {
    CheckConsecutiveLabels(stmt);
  }

  void VisitWhileStmt(const clang::WhileStmt *stmt) {
    CheckLoopBrace(stmt);
  }

  void VisitForStmt(const clang::ForStmt *stmt) {
    CheckLoopBrace(stmt);
  }

  void VisitIfStmt(const clang::IfStmt *stmt) {
    CheckIfBrace(stmt);
    CheckEmptyIfElseStmt(stmt);
    CheckIfWithoutElseStmt(stmt);
  }

  void VisitBinaryOperator(const clang::BinaryOperator *stmt) {
    CheckLogicExprParen(stmt);
    CheckPointerCompareStmt(stmt);
    CheckPointerCalculateStmt(stmt);
    CheckShiftOnSignedNumber<clang::BinaryOperator>(stmt);
    CheckShiftOverflow<clang::BinaryOperator>(stmt);
    CheckAssignNegativetoUnsigned(stmt);
  }

  void VisitFunctionBody(const clang::Stmt *stmt) {
    CheckAsmInProcedure(stmt);
  }

  void VisitGCCAsmStmt(const clang::GCCAsmStmt *stmt) {
    CheckAsmInProcedure(stmt);
  }

  void VisitStringLiteral(const clang::StringLiteral *stmt) {
    TRACE0();
//    CheckStringLiteralEnd(stmt);
  }

  void VisitSwitchStmt(const clang::SwitchStmt *stmt) {
    CheckSwitchWithoutDefaultStmt(stmt);
    CheckCaseEndWithBreak(stmt);
  }

  void VisitGotoStmt(const clang::GotoStmt *stmt) {
    CheckGotoStmt(stmt);
  }

  void VisitCallExpr(const clang::CallExpr *stmt) {
    CheckSetjumpAndLongjump(stmt);
  }

  void VisitCompoundAssignOperator(const clang::CompoundAssignOperator *stmt) {
    CheckShiftOnSignedNumber<clang::CompoundAssignOperator>(stmt);
    CheckShiftOverflow<clang::CompoundAssignOperator>(stmt);
  }
}; // GJB5369StmtRule

}
}