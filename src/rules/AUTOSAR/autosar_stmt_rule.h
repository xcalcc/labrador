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
    _disabled = XcalCheckerManager::GetDisableOption().getValue().find("AUTOSAR") != std::string::npos;
  }

private:

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
      if (parent) parent->dumpColor();
      if (parent && clang::isa<clang::VarDecl>(parent)) return;
    }

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A5_1_1, stmt);
    std::string ref_msg = "Literal values shall not be used apart from type initialization, "
                          "otherwise symbolic names shall be used instead.";
    issue->SetRefMsg(ref_msg);
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

public:
  void VisitBinaryOperator(const clang::BinaryOperator *stmt) {
  }

  void VisitReturnStmt(const clang::ReturnStmt *stmt) {
  }

  void VisitIntegerLiteral(const clang::IntegerLiteral *stmt) {
    CheckLiteralNotWithinInitExpr(stmt);
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
  }

  void VisitCXXTypeidExpr(const clang::CXXTypeidExpr *stmt) {
    CheckLambdaInTypeidtype(stmt);
  }

};

}
}

#endif //LABRADOR_AUTOSAR_STMT_RULE_H
