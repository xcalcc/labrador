/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// stmt_dump_handler.cpp
// ====================================================================
//
// stmt dump handler which dump clang Stmt for reference
//

#include "stmt_dump_handler.h"

namespace xsca {

// generate function body from StmtNodes.inc
#define STMT(CLASS, PARENT) \
    void \
    StmtDumpHandler::Visit##CLASS(const clang::CLASS *stmt) { \
      printf("==== %s ====\n", stmt->getStmtClassName()); \
      stmt->dump(); \
    }
#define ABSTRACT_STMT(STMT)
# include "clang/AST/StmtNodes.inc"
#undef ABSTRACT_STMT
#undef STMT

}  // namespace xsca

