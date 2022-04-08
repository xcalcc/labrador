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

#ifndef LABRADOR_AUTOSAR_STMT_RULE_H
#define LABRADOR_AUTOSAR_STMT_RULE_H

#include <clang/AST/ParentMapContext.h>
#include "autosar_enum.inc"
#include "stmt_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class AUTOSARStmtRule : public StmtNullHandler {
public:
  ~AUTOSARStmtRule() = default;

  AUTOSARStmtRule() {
    _enable = true;
//    _enable = XcalCheckerManager::GetEnableOption().getValue().find("AUTOSAR") != std::string::npos;
  }

private:

  bool IsAssign(clang::OverloadedOperatorKind kind) const;

  /* AUTOSAR
   * Rule: A2-13-5
   * Hexadecimal constants should be upper case.
   */
  void CheckHexadecimalUpperCase(const clang::IntegerLiteral *stmt);

  /* AUTOSAR
   * Rule: A4-5-1
   * Expressions with type enum or enum class shall not be used as operands to built-in and overloaded
   * operators other than the subscript operator [ ], the assignment operator =, the equality operators == and ! =,
   * the unary & operator, and the relational operators <, <=, >, >=.
   */
  void CheckEnumBeyondLimit(const clang::BinaryOperator *stmt);

  /* AUTOSAR
   * Rule: M5-0-3
   * A cvalue expression shall not be implicitly converted to a different underlying type.
   */
  void CheckInappropriateCast(const clang::CStyleCastExpr *stmt);

  /*
   * AUTOSAR: A5-1-1
   * Literal values shall not be used apart from type initialization,
   * otherwise symbolic names shall be used instead.
   */
  template<typename Literal>
  void CheckLiteralNotWithinInitExpr(const Literal *stmt) {
    auto ctx = XcalCheckerManager::GetAstContext();
    auto parents = ctx->getParents(*stmt);
    if (!parents.empty()) {
      auto parent = parents[0].template get<clang::Decl>();
      if (parent && clang::isa<clang::VarDecl>(parent)) return;
    }

    bool is_str = clang::isa<clang::StringLiteral>(stmt);

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A5_1_1, stmt);
    std::string ref_msg = "Literal values shall not be used apart from type initialization, "
                          "otherwise symbolic names shall be used instead.";
    issue->SetRefMsg(ref_msg);
    issue->SetIsMaybe(is_str);
  }

  /*
   * AUTOSAR: A5-1-2
   * Variables shall not be implicitly captured in a lambda expression.
   */
  void CheckLambdaImplicitlyCaptured(const clang::LambdaExpr *stmt);

  /*
   * AUTOSAR: A5-1-3
   * Parameter list (possibly empty) shall be included in every lambda expression.
   */
  void CheckLambdaParameterList(const clang::LambdaExpr *stmt);

  /*
   * AUTOSAR: A5-1-6
   * Return type of a non-void return type lambda expression should be explicitly specified.
   */
  void CheckLambdaExplictReturnType(const clang::LambdaExpr *stmt);

  /*
   * AUTOSAR: A5-1-7 (partial)
   * A lambda shall not be an operand to decltype or typeid.
   */
  void CheckLambdaInTypeidtype(const clang::CXXTypeidExpr *stmt);

  /*
   * AUTOSAR: A5-1-8
   * Lambda expressions should not be defined inside another lambda expression.
   */
  void CheckNestedLambdaExpr(const clang::LambdaExpr *stmt);

  /*
   * AUTOSAR: A5-16-1
   * The ternary conditional operator shall not be used as a sub-expression.
   */
  void CheckConditionalOperatorAsSubExpr(const clang::ConditionalOperator *stmt);

  /*
   * AUTOSAR: A7-1-7 (partial)
   * Each expression statement and identifier declaration shall be placed on a separate line.
   */
  void CheckDeclsInSameLine(const clang::DeclStmt *stmt);

  /*
   * AUTOSAR: A7-5-2
   * Functions shall not call themselves, either directly or indirectly.
   */
  void CheckFunctionCallThemselves(const clang::CallExpr *stmt);

  /*
   * AUTOSAR: A9-3-1
   * Member functions shall not return non-const “raw” pointers or references to
   * private or protected data owned by the class.
   */
  void CheckMethodReturnPrivateOrProtectFields(const clang::ReturnStmt *stmt);

  /*
   * AUTOSAR: A13-2-1
   * An assignment operator shall return a reference to “this”.
   */
  void CheckAssignmentOperatorReturnThisRef(const clang::ReturnStmt *stmt);

  /*
   * AUTOSAR: A13-2-2
   * A binary arithmetic operator and a bitwise operator shall return a “prvalue”.
   */
  void CheckBinaryOpOrBitwiseOpReturnPRValue(const clang::ReturnStmt *stmt);

  /*
   * AUTOSAR: A13-2-3
   * A relational operator shall return a boolean value.
   */
  void CheckRelationalOpReturnBool(const clang::ReturnStmt *stmt);

  /*
   * AUTOSAR: A13-5-3
   * User-defined conversion operators should not be used.
   */
  void CheckUsingUserDefinedConversionOp(const clang::CStyleCastExpr *stmt);


public:
  void VisitBinaryOperator(const clang::BinaryOperator *stmt) {
    CheckEnumBeyondLimit(stmt);
  }

  void VisitReturnStmt(const clang::ReturnStmt *stmt) {
    CheckMethodReturnPrivateOrProtectFields(stmt);
    CheckAssignmentOperatorReturnThisRef(stmt);
    CheckBinaryOpOrBitwiseOpReturnPRValue(stmt);
    CheckRelationalOpReturnBool(stmt);
  }

  void VisitIntegerLiteral(const clang::IntegerLiteral *stmt) {
    CheckLiteralNotWithinInitExpr(stmt);
    CheckHexadecimalUpperCase(stmt);
  }

  void VisitCharacterLiteral(const clang::CharacterLiteral *stmt) {
    CheckLiteralNotWithinInitExpr(stmt);
  }

  void VisitStringLiteral(const clang::StringLiteral *stmt) {
    CheckLiteralNotWithinInitExpr(stmt);
  }

  void VisitFloatingLiteral(const clang::FloatingLiteral *stmt) {
    CheckLiteralNotWithinInitExpr(stmt);
  }

  void VisitLambdaExpr(const clang::LambdaExpr *stmt) {
    CheckLambdaImplicitlyCaptured(stmt);
    CheckLambdaParameterList(stmt);
    CheckLambdaExplictReturnType(stmt);
    CheckNestedLambdaExpr(stmt);
  }

  void VisitCXXTypeidExpr(const clang::CXXTypeidExpr *stmt) {
    CheckLambdaInTypeidtype(stmt);
  }

  void VisitConditionalOperator(const clang::ConditionalOperator *stmt) {
    CheckConditionalOperatorAsSubExpr(stmt);
  }

  void VisitDeclStmt(const clang::DeclStmt *stmt) {
    CheckDeclsInSameLine(stmt);
  }

  void VisitCallExpr(const clang::CallExpr *stmt) {
    CheckFunctionCallThemselves(stmt);
  }

  void VisitCStyleCastExpr(const clang::CStyleCastExpr *stmt) {
    CheckUsingUserDefinedConversionOp(stmt);
    CheckInappropriateCast(stmt);
  }
};

}
}

#endif //LABRADOR_AUTOSAR_STMT_RULE_H
