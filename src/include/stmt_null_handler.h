/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// stmt_null_handler.h
// ====================================================================
//
// stmt null handler which does nothing on clang Stmt
//

#ifndef STMT_NULL_HANDLER_INCLUDED
#define STMT_NULL_HANDLER_INCLUDED

#include "xsca_defs.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprConcepts.h"
#include "clang/AST/StmtObjC.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/StmtOpenMP.h"
#include "clang/AST/ExprOpenMP.h"

namespace xsca {

// class StmtNullHandler
class StmtNullHandler {

public:
  // generate function prototype from StmtNodes.inc
  #define STMT(CLASS, PARENT) \
      void Visit##CLASS(const clang::CLASS *stmt) { \
      }
  #define ABSTRACT_STMT(STMT)
  # include "clang/AST/StmtNodes.inc"
  #undef ABSTRACT_STMT
  #undef STMT

};  // StmtNullHandler

}  // namespace xsca

#endif  // STMT_NULL_HANDLER_INCLUDED
