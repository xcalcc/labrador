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
  ~CheckStmtExample() {
    TRACE0();
  }

public:
  void VisitGotoStmt(const clang::GotoStmt *stmt) {
    printf("Hit a goto stmt.\n");
  }

};  // CheckStmtExample
