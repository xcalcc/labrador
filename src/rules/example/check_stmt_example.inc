/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// check_stmt_example.inc
// ====================================================================
//
// example to describe how to write checker which checks clang Stmt/Expr
//

class CheckStmtExample : public StmtNullHandler {
public:
  ~CheckStmtExample() {}

private:
  void CheckConsecutiveLabels(const clang::LabelStmt *stmt) {
    /*
     * GJB5369 4.1.1.4
     * Check multiple consecutive labels.
     */
    if (clang::dyn_cast<clang::LabelStmt>(stmt->getSubStmt())) {
      printf("GJB5396:4.1.1.4: Multiple consecutive labels: %s and %s\n",
             stmt->getDecl()->getNameAsString().c_str(),
             clang::dyn_cast<clang::LabelStmt>(stmt->getSubStmt())
                 ->getDecl()
                 ->getNameAsString()
                 .c_str());
    }
  }

public:
  void VisitGotoStmt(const clang::GotoStmt *stmt) {}

  void VisitCompoundStmt(const clang::CompoundStmt *stmt) {}

  void VisitLabelStmt(const clang::LabelStmt *stmt) {
    CheckConsecutiveLabels(stmt);
  }
}; // CheckStmtExample
