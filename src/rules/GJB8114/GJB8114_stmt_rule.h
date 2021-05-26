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

  /*
   * GJB8114: 5.2.1.1
   * Loop body should be enclosed with brace
   */
  template<typename Type>
  void CheckLoopBodyWithBrace(const Type *stmt) {
    if (CheckStmtWithBrace(stmt->getBody())) return;

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_1_2_6, stmt);
    std::string ref_msg = "Loop body should be enclosed with brace";
    issue->SetRefMsg(ref_msg);
  }

  bool CheckStmtWithBrace(const clang::Stmt *stmt)  {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto location = stmt->getBeginLoc();
    auto data = src_mgr->getCharacterData(location);

    if (*data != '{') {
      return false;
    }
    return true;
  }


public:
  void VisitForStmt(const clang::ForStmt *stmt) {
    CheckLoopBodyWithBrace<clang::ForStmt>(stmt);
  }

  void VisitWhileStmt(const clang::WhileStmt *stmt) {
    CheckLoopBodyWithBrace<clang::WhileStmt>(stmt);
  }

  void VisitDoStmt(const clang::DoStmt *stmt) {
    CheckLoopBodyWithBrace<clang::DoStmt>(stmt);
  }

}; // GJB8114StmtRule
}
}
