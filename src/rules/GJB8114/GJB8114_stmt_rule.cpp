/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB8114_stmt_rule.cpp
// ====================================================================
//
// implement Decl related rules for GJB8114
//

#include "GJB8114_enum.inc"
#include "GJB8114_stmt_rule.h"


namespace xsca {
namespace rule {

/*
 * GJB8114: 5.1.2.6
 * Loop body should be enclosed with brace
 */
void GJB8114StmtRule::CheckLoopBodyWithBrace(const clang::ForStmt *stmt) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto body = stmt->getBody();
  auto location = body->getBeginLoc();
  auto data = src_mgr->getCharacterData(location);

  if (*data != '{') {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_1_2_6, stmt);
    std::string ref_msg = "Loop body should be enclosed with brace";
    issue->SetRefMsg(ref_msg);
    issue->AddStmt(body);
  }
}

}
}
