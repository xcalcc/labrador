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

  // Check if stmt contains bitwise operator
  bool HasBitwiseSubStmt(const clang::Stmt *stmt);

  bool HasPrefixOrPostfixSubStmt(const clang::Stmt *stmt);

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

  /*
   * GJB8114: 5.4.2.1
   * Branches nested more than 7 level is forbidden
   */
  void CheckBranchNestedTooMuch(const clang::IfStmt *stmt);

  /*
   * GJB8114: 5.6.1.4
   * Bitwise operator within logic statement is forbidden
   */
  void CheckBitwiseOpInLogicStmt(const clang::IfStmt *stmt);

  /*
   * GJB8114: 5.6.1.5
   * Using ++ or -- in arithmetic statement or function parameters is forbidden
   */
  void CheckIncOrDecUnaryInStmt(const clang::UnaryOperator *stmt);

  /*
   * GJB8114: 5.6.1.10
   * Performing logic-not on ingeter literal is forbidden
   */
  void CheckNotOperatorOnConstant(const clang::UnaryOperator *stmt);

  /*
   * GJB8114: 5.6.1.11
   * Enum value used by non-enum variable is forbidden
   */
  void CheckUsingEnumByOtherTypeVar(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.6.1.18
   * Using gets function is forbidden
   */
  void CheckUsingGetsFunction(const clang::CallExpr *stmt);

  /*
   * GJB8114: 5.6.2.2
   * Be careful with the division of integer and integer
   */
  void CheckIntegerDivision(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.6.2.4
   * Avoid using strcpy function
   */
  void CheckUsingStrcpy(const clang::CallExpr *stmt);

  /*
   * GJB8114: 5.6.2.5
   * Avoid using strcat function
   */
  void CheckUsingStrcat(const clang::CallExpr *stmt);

  /*
   * GJB8114: 5.7.1.11
   * void is required as the function which has return value is called but the return value is not used
   */
  void CheckUnusedFunctionCast(const clang::CallExpr *stmt);

  /*
   * GJB8114: 5.7.1.12
   * Void is not required as the function which is void type is called
   */
  void CheckNotRequiredFunctionCast(const clang::CallExpr *stmt);

  /*
   * GJB8114: 5.8.1.5
   * Suffix of number must use upper case letters
   */
  template<typename TYPE>
  void CheckLiteralSuffix(const TYPE *stmt) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();

    char ch;
    auto data = src_mgr->getCharacterData(stmt->getBeginLoc());
    do {
      ch = *data++;
      if (ch == '.') continue;
      if (std::isdigit(ch)) {
        continue;
      }
      if (std::isalpha(ch)) {
        if (std::isupper(ch)) return;
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();
        issue = report->ReportIssue(GJB8114, G5_8_1_5, stmt);
        std::string ref_msg = "Suffix of number must use upper case letters";
        issue->SetRefMsg(ref_msg);
      }
      break;
    } while (true);
  }

public:
  void VisitIfStmt(const clang::IfStmt *stmt) {
    CheckBranchNestedTooMuch(stmt);
    CheckBitwiseOpInLogicStmt(stmt);
  }

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
    CheckUsingEnumByOtherTypeVar(stmt);
    CheckIntegerDivision(stmt);
  }

  void VisitUnaryOperator(const clang::UnaryOperator *stmt) {
    CheckIncOrDecUnaryInStmt(stmt);
    CheckNotOperatorOnConstant(stmt);
  }

  void VisitSwitchStmt(const clang::SwitchStmt *stmt) {
    CheckDifferentHierarchySwitchCase(stmt);
  }

  void VisitCallExpr(const clang::CallExpr *stmt) {
    CheckUsingGetsFunction(stmt);
    CheckUnusedFunctionCast(stmt);
    CheckNotRequiredFunctionCast(stmt);
  }

  void VisitIntegerLiteral(const clang::IntegerLiteral *stmt) {
    CheckLiteralSuffix<clang::IntegerLiteral>(stmt);
  }

  void VisitFloatingLiteral(const clang::FloatingLiteral *stmt) {
    TRACE0();
    CheckLiteralSuffix<clang::FloatingLiteral>(stmt);
  }

}; // GJB8114StmtRule
}
}
