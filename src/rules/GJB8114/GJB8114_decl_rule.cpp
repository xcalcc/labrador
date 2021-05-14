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


}
}