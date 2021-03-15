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

class GJB5369StmtRule : public StmtNullHandler {
public:
  ~GJB5369StmtRule() {}

private:
  bool _is_single_asm_stmt;

  bool CheckExprParentheses(const clang::Expr *expr) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto expr_loc = expr->getBeginLoc();
    const char *start = src_mgr->getCharacterData(expr_loc);
    if (*start != '(') { return true; }
    return false;
  }

  /*
   * GJB5369 4.1.1.4
   * Check multiple consecutive labels.
   */
  void CheckConsecutiveLabels(const clang::LabelStmt *stmt) {
    if (clang::dyn_cast<clang::LabelStmt>(stmt->getSubStmt())) {
      REPORT("GJB5396:4.1.1.4: Multiple consecutive labels: %s and %s\n",
             stmt->getDecl()->getNameAsString().c_str(),
             clang::dyn_cast<clang::LabelStmt>(stmt->getSubStmt())
                 ->getDecl()
                 ->getNameAsString()
                 .c_str());
    }
  }

  /*
   * GJB5369: 4.2.1.2
   * the loop must be enclosed in braces
   */
  void CheckLoopBrace(const clang::WhileStmt *stmt) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto body_loc = stmt->getBody()->getBeginLoc();
    const char *start = src_mgr->getCharacterData(body_loc);
    if (*start != '{') {
      REPORT("GJB5396:4.2.1.2: The while-loop must be enclosed in braces: %s\n",
             body_loc.printToString(*src_mgr).c_str());
    }
  }

  void CheckLoopBrace(const clang::ForStmt *stmt) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto body_loc = stmt->getBody()->getBeginLoc();
    const char *start = src_mgr->getCharacterData(body_loc);
    if (*start != '{') {
      REPORT("GJB5396:4.2.1.2: The for-loop must be enclosed in braces: %s\n",
             body_loc.printToString(*src_mgr).c_str());
    }
  }

  /*
   * GJB5369: 4.2.1.3
   * if/else block must be enclosed in braces
   */
  void CheckIfBrace(const clang::IfStmt *stmt) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    const char *start;
    for (const auto &it : stmt->children()) {

      if (clang::dyn_cast<clang::IfStmt>(it) ||
          it == stmt->getCond()) {
        continue;
      }
      auto body_loc = it->getBeginLoc();
      start = src_mgr->getCharacterData(body_loc);
      if (*start != '{') {
        REPORT("GJB5396:4.2.1.3: if/else block must be enclosed in braces: %s\n",
               body_loc.printToString(*src_mgr).c_str());
      }
    }
  }

  /*
   * GJB5369: 4.2.1.4
   * logic expression should be enclosed in parentheses
   */
  void CheckLogicExprParen(const clang::BinaryOperator *stmt) {
    if (!stmt->isLogicalOp()) return;
    auto lhs = stmt->getLHS();
    auto rhs = stmt->getRHS();
    auto src_mgr = XcalCheckerManager::GetSourceManager();

    if (CheckExprParentheses(lhs)) {
      auto lhs_loc = lhs->getBeginLoc();
      REPORT("GJB5396:4.2.1.4: logic expression should be enclosed in parentheses: %s\n",
             lhs_loc.printToString(*src_mgr).c_str());
    }

    if (CheckExprParentheses(rhs)) {
      auto rhs_loc = rhs->getBeginLoc();
      REPORT("GJB5396:4.2.1.4: logic expression should be enclosed in parentheses: %s\n",
             rhs_loc.printToString(*src_mgr).c_str());
    }
  }

  /*
   * GJB5369: 4.2.1.7
   * the assemble procedure should be pure assemble
   */
  void CheckAsmInProcedure(const clang::Stmt *stmt) {
    int stmt_num = 0;
    for (const auto &it : stmt->children()) {
      stmt_num++;
      if (it->getStmtClass() == clang::Stmt::StmtClass::GCCAsmStmtClass) {
        if (stmt_num > 1) {
          _is_single_asm_stmt = false;
          return;
        }
      } else {
        _is_single_asm_stmt = false;
        return;
      }
    }
  }

  void CheckAsmInProcedure(const clang::GCCAsmStmt *stmt) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto location = stmt->getBeginLoc();
    if (!_is_single_asm_stmt) {
      REPORT("GJB5396:4.2.1.7: The assemble procedure should be pure assemble:"
             " %s\n",
             location.printToString(*src_mgr).c_str());
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
  }

  void VisitBinaryOperator(const clang::BinaryOperator *stmt) {
    CheckLogicExprParen(stmt);
  }

  void VisitFunctionBody(const clang::Stmt *stmt) {
    CheckAsmInProcedure(stmt);
  }

  void VisitGCCAsmStmt(const clang::GCCAsmStmt *stmt) {
    CheckAsmInProcedure(stmt);
  }
}; // GJB5369StmtRule

