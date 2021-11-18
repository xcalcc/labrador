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

#include "autosar_stmt_rule.h"

namespace xsca {
namespace rule {


void AUTOSARStmtRule::CheckLambdaImplicitlyCaptured(const clang::LambdaExpr *stmt) {
  if (!stmt->implicit_captures().empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A5_1_2, stmt);
    std::string ref_msg = "Variables shall not be implicitly captured in a lambda expression.";
    issue->SetRefMsg(ref_msg);
  }
}

void AUTOSARStmtRule::CheckLambdaParameterList(const clang::LambdaExpr *stmt) {
  if (stmt->hasExplicitParameters()) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A5_1_3, stmt);
  std::string ref_msg = "Parameter list (possibly empty) shall be included in every lambda expression.";
  issue->SetRefMsg(ref_msg);
}

void AUTOSARStmtRule::CheckLambdaExplictReturnType(const clang::LambdaExpr *stmt) {
  if (stmt->hasExplicitResultType()) return;

  clang::QualType result_type = stmt->getLambdaClass()->getLambdaCallOperator()->getType();
  DBG_ASSERT(result_type.isNull() == false, "get lambda return type failed");

  if (auto lambda_type = clang::dyn_cast<clang::FunctionProtoType>(result_type)) {
    if (lambda_type->getReturnType()->isVoidType()) return;
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A5_1_6, stmt);
  std::string ref_msg = "Return type of a non-void return type lambda expression should be explicitly specified.";
  issue->SetRefMsg(ref_msg);
}

void AUTOSARStmtRule::CheckLambdaInTypeidtype(const clang::CXXTypeidExpr *stmt) {
  auto op = stmt->getExprOperand();
  if (auto ref_expr = clang::dyn_cast<clang::DeclRefExpr>(op)) {
    auto ref_type = ref_expr->getType();

    auto auto_type = clang::dyn_cast<clang::AutoType>(ref_type);
    if (!auto_type) return;

    auto record_type = auto_type->getAs<clang::RecordType>();
    if (!record_type || !record_type->getDecl()->isLambda()) return;

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A5_1_7, stmt);
    std::string ref_msg = "A lambda shall not be an operand to decltype or typeid.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A5-1-8
 * Lambda expressions should not be defined inside another lambda expression.
 */
void AUTOSARStmtRule::CheckNestedLambdaExpr(const clang::LambdaExpr *stmt) {
  if (this->LambdaDepth() > 1) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A5_1_8, stmt);
    std::string ref_msg = "Lambda expressions should not be defined inside another lambda expression.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A5-16-1
 * The ternary conditional operator shall not be used as a sub-expression.
 */
void AUTOSARStmtRule::CheckConditionalOperatorAsSubExpr(const clang::ConditionalOperator *stmt) {
  auto ctx = XcalCheckerManager::GetAstContext();
  auto parents = ctx->getParents(*stmt);
  if (!parents.empty()) {
    auto parent_decl = parents[0].get<clang::Decl>();
    if (parent_decl && clang::isa<clang::VarDecl>(parent_decl)) return;

    auto parent_stmt = parents[0].get<clang::Stmt>();
    if (parent_stmt) {
      if (auto bin_stmt = clang::dyn_cast<clang::BinaryOperator>(parent_stmt)) {
        if (bin_stmt->isAssignmentOp() || bin_stmt->isCompoundAssignmentOp()) return;
      } else if (clang::isa<clang::CompoundStmt>(parent_stmt)) {
        return;
      } else if (clang::isa<clang::ReturnStmt>(parent_stmt)) {
        return;
      } else {
        if (clang::isa<clang::ParenExpr>(parent_stmt) || clang::isa<clang::ImplicitCastExpr>(parent_stmt)) {
          const clang::Stmt *tmp = parent_stmt;
          do {
            parents = ctx->getParents(*tmp);
            if (parents.empty()) return;
            tmp = parents[0].get<clang::Stmt>();
            if (tmp == nullptr) return;
          } while (clang::isa<clang::ParenExpr>(tmp) || clang::isa<clang::ImplicitCastExpr>(tmp));

          switch (tmp->getStmtClass()) {
            case clang::Stmt::StmtClass::ReturnStmtClass:
              case clang::Stmt::StmtClass::CompoundStmtClass:
                return;
            case clang::Stmt::StmtClass::BinaryOperatorClass: {
              auto bin = clang::dyn_cast<clang::BinaryOperator>(tmp);
              if (bin->isAssignmentOp() || bin->isCompoundAssignmentOp()) return;
            }
            default:
              break;
          }
        }
      }
    }

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A5_16_1, stmt);
    std::string ref_msg = "The ternary conditional operator shall not be used as a sub-expression.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A7-1-7 (partial)
 * Each expression statement and identifier declaration shall be placed on a separate line.
 */
void AUTOSARStmtRule::CheckDeclsInSameLine(const clang::DeclStmt *stmt) {
  if (stmt->isSingleDecl()) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A7_1_7, stmt);
  std::string ref_msg = "Each expression statement and identifier declaration shall be placed on a separate line.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A7-5-2
 * Functions shall not call themselves, either directly or indirectly.
 */
void AUTOSARStmtRule::CheckFunctionCallThemselves(const clang::CallExpr *stmt) {
  if (stmt->getCalleeDecl() == this->_current_function_decl) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A7_5_2, stmt);
    std::string ref_msg = "Functions shall not call themselves, either directly or indirectly.";
    issue->SetRefMsg(ref_msg);
  }
}

}
}
