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
  ~CheckStmtExample() { TRACE0(); }

private:
  void CheckConsecutiveLabels(const clang::LabelStmt *stmt) {
    /*
     * GJB5369 4.1.1.4
     * Check multiple consecutive labels.
     */
    if (clang::dyn_cast<clang::LabelStmt>(stmt->getSubStmt())) {
      printf("Multiple consecutive labels: %s and %s\n",
             stmt->getDecl()->getNameAsString().c_str(),
             clang::dyn_cast<clang::LabelStmt>(stmt->getSubStmt())
                 ->getDecl()
                 ->getNameAsString()
                 .c_str());
    }
  }

public:
  void VisitGotoStmt(const clang::GotoStmt *stmt) {
    printf("Hit a goto stmt.\n");
  }

  void VisitCompoundStmt(const clang::CompoundStmt *stmt) {
    printf("Hit a block stmt.\n");
  }

  void VisitLabelStmt(const clang::LabelStmt *stmt) {
    CheckConsecutiveLabels(stmt);
  }
}; // CheckStmtExample
