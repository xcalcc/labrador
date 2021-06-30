/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB8114_decl_rule.cpp
// ====================================================================
//
// implement Decl related rules for GJB8114
//

#include <unordered_map>
#include <clang/AST/Decl.h>
#include <clang/AST/ASTContext.h>

#include "GJB8114_enum.inc"
#include "GJB8114_decl_rule.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

/*
 * GJB8114: 5.1.1.8
 * identifier is a must in declaration of struct, enum and union
 */
void GJB8114DeclRule::CheckAnonymousRecord(const clang::RecordDecl *decl) {
  auto name = decl->getNameAsString();
  if (name.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_1_1_8, decl);
    std::string ref_msg = "Identifier is a must in declaration of struct, enum and union";
    issue->SetRefMsg(ref_msg);
  }
}

void GJB8114DeclRule::CheckAnonymousEnum(const clang::EnumDecl *decl) {
  auto name = decl->getNameAsString();
  if (name.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_1_1_8, decl);
    std::string ref_msg = "Identifier is a must in declaration of struct, enum and union";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.1.1.9
 * Anonymous struct in struct is forbidden
 */
void GJB8114DeclRule::CheckAnonymousStructInRecord(const clang::RecordDecl *decl) {
  XcalIssue *issue = nullptr;
  std::unordered_map<std::string, clang::RecordDecl *> records;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto &it : decl->decls()) {
    if (auto record = clang::dyn_cast<clang::RecordDecl>(it)) {
      auto name = record->getNameAsString();
      if (name == decl->getNameAsString()) continue;
      records.insert({name, record});
    }
  }

  for (const auto &it : decl->fields()) {
    if (!it->getType()->isRecordType()) continue;
    auto field_name = it->getType().getAsString();
    auto pos = field_name.find("struct");
    auto record_name = field_name.substr(pos + 7);
    auto res = records.find(record_name);
    if (res != records.end()) {
      records.erase(res);
    }
  }

  if (records.empty()) return;
  issue = report->ReportIssue(GJB8114, G5_1_1_8, decl);
  std::string ref_msg = "Anonymous struct in struct is forbidden";
  issue->SetRefMsg(ref_msg);
  for (const auto &it : records) {
    issue->AddDecl(it.second);
  }

}

/*
 * GJB8114: 5.1.1.12
 * Bit-fields should be the same length and within the length of its origin type
 */
void GJB8114DeclRule::CheckUniformityOfBitFields(const clang::RecordDecl *decl) {
  if (decl->field_empty()) return;
  auto ctx = XcalCheckerManager::GetAstContext();
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  bool need_report = false;
  unsigned length = 0;
  for (const auto &it : decl->fields()) {
    if (!it->isBitField()) continue;
    unsigned width = it->getBitWidthValue(*ctx);
    if (length == 0) {
      length = width;
    } else if (length != width) {
      need_report = true;
    } else {
      auto f_type = it->getType();
      auto typeInfo = ctx->getTypeInfo(f_type);
      auto origin_width = typeInfo.Width;
      if (origin_width < width) {
        need_report = true;
      }
    }

    if (need_report) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB8114, G5_1_1_12, decl);
        std::string ref_msg = "Bit-fields should be the same length and within the length of its origin type";
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(it);
      need_report = false;
    }
  }
}

/*
 * GJB8114: 5.1.1.17
 * Using "extern" variable in function is forbidden
 */
void GJB8114DeclRule::CheckExternVariableInFunction(const clang::VarDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  auto decl_ctx = decl->isLocalVarDeclOrParm();
  if (!const_cast<clang::VarDecl *>(decl)->isLocalExternDecl()) return;
  issue = report->ReportIssue(GJB8114, G5_1_1_17, decl);
  std::string ref_msg = "Using \"extern\" variable in function is forbidden";
  issue->SetRefMsg(ref_msg);
}

void GJB8114DeclRule::CheckExternVariableInFunction(const clang::FunctionDecl *decl) {
  auto decl_ctx = decl->getParent();
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  if (!const_cast<clang::FunctionDecl *>(decl)->isLocalExternDecl()) return;
  issue = report->ReportIssue(GJB8114, G5_1_1_17, decl);
  std::string ref_msg = "Using \"extern\" variable in function is forbidden";
  issue->SetRefMsg(ref_msg);
}

/*
 * GJB8114: 5.1.2.4
 * Variables should be declared at the beginning of function body
 */
void GJB8114DeclRule::CheckVariableDeclPosition(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  if (!decl->doesThisDeclarationHaveABody()) return;
  bool hit_stmt = false;
  for (const auto &it : decl->getBody()->children()) {
    if (it->getStmtClass() == clang::Stmt::StmtClass::DeclStmtClass) {
      if (!hit_stmt) continue; // continue if not meet other statement
      else {
        if (issue == nullptr) {
          issue = report->ReportIssue(GJB8114, G5_1_2_4, decl);
          std::string ref_msg = "Variables should be declared at the beginning of function body: ";
          ref_msg += decl->getNameAsString();
          issue->SetRefMsg(ref_msg);
        }
        if (auto decl_stmt = clang::dyn_cast<clang::DeclStmt>(it)) {
          if (decl_stmt->isSingleDecl()) {
            auto it_decl = clang::dyn_cast<clang::DeclStmt>(it)->getSingleDecl();
            issue->AddDecl(it_decl);
          } else {
            for (const auto &it_decl : decl_stmt->decls()) {
              issue->AddDecl(&(*it_decl));
            }
          }
        }
      }
    } else hit_stmt = true;
  }
}

/*
 * GJB8114: 5.1.2.5
 * Struct should not nest more than three levels
 */
void GJB8114DeclRule::CheckNestedStructure(const clang::RecordDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  if (decl->decls_empty()) return;

  for (const auto &it : decl->decls()) {
    if (auto record = clang::dyn_cast<clang::RecordDecl>(it)) {
      for (const auto &sub_it : record->decls()) {
        if (clang::dyn_cast<clang::RecordDecl>(sub_it)) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB8114, G5_1_2_5, decl);
            std::string ref_msg = "Struct should not nest more than three levels: ";
            ref_msg += decl->getNameAsString();
            issue->SetRefMsg(ref_msg);
          }
          issue->AddDecl(sub_it);
        }
      }
    }
  }
}

/*
 * GJB8114: 5.3.1.7
 * Pointers should be initialized as NULL
 */
void GJB8114DeclRule::CheckPointerInitWithNull(const clang::VarDecl *decl) {
  if (!decl->getType()->isPointerType()) return;

  auto ctx = XcalCheckerManager::GetAstContext();

  if (decl->hasInit()) {
    auto init_val = decl->getInit()->IgnoreParenImpCasts();
    if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(init_val)) {
      int value;
      clang::Expr::EvalResult eval_result;

      // try to fold the const expr
      if (literal->EvaluateAsInt(eval_result, *ctx)) {
        value = eval_result.Val.getInt().getZExtValue();
      } else {
        value = literal->getValue().getZExtValue();
      }

      // return if pointer has been initialized with NULL
      if (value == 0) return;
    }
  }

  // report issue
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(GJB8114, G5_3_1_7, decl);
  std::string ref_msg = "Pointers should be initialized as NULL";
  issue->SetRefMsg(ref_msg);
}

/*
 * GJB8114: 5.3.2.2
 * Using void pointer carefully
 */
void GJB8114DeclRule::CheckVoidPointer(const clang::VarDecl *decl) {
  auto type = decl->getType();
  if (!type->isPointerType()) return;
  if (type->getPointeeType()->isVoidType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_3_2_2, decl);
    std::string ref_msg = "Using void pointer carefully: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.3.2.2
 * Using void pointer carefully
 */
void GJB8114DeclRule::CheckVoidPointer(const clang::FunctionDecl *decl) {
  if (decl->param_empty()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto &it : decl->parameters()) {
    if (!it->getType()->isPointerType()) continue;
    if (it->getType()->getPointeeType()->isVoidType()) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB8114, G5_3_2_2, decl);
        std::string ref_msg = "Using void pointer carefully: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(&(*it));
    }
  }
}

/*
 * GJB8114: 5.7.1.13
 * static function must be used
 */
void GJB8114DeclRule::CheckUnusedStaticFunction() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  top_scope->TraverseAll<IdentifierManager::IdentifierKind::FUNCTION,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        auto func = clang::dyn_cast<clang::FunctionDecl>(decl);
        if (!func->isStatic()) return;

        if (!func->isUsed()) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB8114, G5_7_1_13, decl);
            std::string ref_msg = "static function must be used";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/*
 * GJB8114: 5.7.2.1
 * Avoid using too much parameters in function
 */
void GJB8114DeclRule::CheckTooManyParams(const clang::FunctionDecl *decl) {
  if (decl->param_size() > 10) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_7_2_1, decl);
    std::string ref_msg = "Avoid using too much parameters(more than 10) in function";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.8.1.5
 * Suffix of number must use upper case letters
 *
 * GJB8114: 5.8.2.4
 * Using suffix with number is recommended
 */
void GJB8114DeclRule::CheckLiteralSuffixInit(const clang::VarDecl *decl) {
  if (!decl->hasInit()) return;
  auto stmt = decl->getInit();

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  auto src_mgr = XcalCheckerManager::GetSourceManager();

  char ch;
  auto data = src_mgr->getCharacterData(stmt->getBeginLoc());
  do {
    ch = *data++;
    if (ch == '.') continue;
    if (std::isdigit(ch)) {
      continue;
    } else if (std::isalpha(ch)) {
      if (std::isupper(ch)) return;
      issue = report->ReportIssue(GJB8114, G5_8_1_5, stmt);
      std::string ref_msg = "Suffix of number must use upper case letters";
      issue->SetRefMsg(ref_msg);
    } else {
      issue = report->ReportIssue(GJB8114, G5_8_2_4, stmt);
      std::string ref_msg = "Using suffix with number is recommended";
      issue->SetRefMsg(ref_msg);
    }
    break;
  } while (true);
}

/*
 * GJB8114: 5.11.1.2
 * Omitting init value which depends on the system is forbidden
 */
void GJB8114DeclRule::CheckOmitInitValueDependOnSystem() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  // check static local variable
  top_scope->TraverseAll<IdentifierManager::IdentifierKind::VAR,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        auto varDecl = clang::dyn_cast<clang::VarDecl>(decl);
        if (varDecl->isStaticLocal() || varDecl->hasGlobalStorage()) {
          if (!varDecl->hasInit()) {
            if (issue == nullptr) {
              issue = report->ReportIssue(GJB8114, G5_11_1_2, decl);
              std::string ref_msg = "Omitting init value which depends on the system is forbidden";
              issue->SetRefMsg(ref_msg);
            } else {
              issue->AddDecl(decl);
            }
          }
        }
      }, true);

}

/*
 * GJB8114: 5.11.2.1
 * Init the variable at its declaration
 */
void GJB8114DeclRule::CheckInitWithDecl() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  // check static local variable
  top_scope->TraverseAll<IdentifierManager::IdentifierKind::VAR,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        auto varDecl = clang::dyn_cast<clang::VarDecl>(decl);
        if (!varDecl->hasInit()) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB8114, G5_11_2_1, decl);
            std::string ref_msg = "Init the variable at its declaration";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/*
 * GJB8114: 5.13.1.3
 * Variable names conflicting with function names is forbidden
 */
void GJB8114DeclRule::CheckVariableConflictWithFunction() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  top_scope->TraverseAll<IdentifierManager::IdentifierKind::VAR,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report, &top_scope](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        auto varDecl = clang::dyn_cast<clang::VarDecl>(decl);
        if (top_scope->HasFunctionName(varDecl->getNameAsString())) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB8114, G5_13_1_3, decl);
            std::string ref_msg = "Variable names conflicting with function names is forbidden";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}


}
}
