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

class GJB5369StmtRule : public StmtNullHandler {
public:
  ~GJB5369StmtRule() {}

private:
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
      REPORT("GJB5396:4.2.1.2: The loop must be enclosed in braces: %s\n",
             body_loc.printToString(*src_mgr).c_str());
    }
  }

  void CheckLoopBrace(const clang::ForStmt *stmt) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto body_loc = stmt->getBody()->getBeginLoc();
    const char *start = src_mgr->getCharacterData(body_loc);
    if (*start != '{') {
      REPORT("GJB5396:4.2.1.2: The loop must be enclosed in braces: %s\n",
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
}; // GJB5369StmtRule

