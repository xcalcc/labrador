/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// MISRA_stmt_rule.h
// ====================================================================
//
// implement all stmt related rules in MISRA-C-2012
//

#include "MISRA_enum.inc"
#include "stmt_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class MISRAStmtRule : public StmtNullHandler {
public:
  ~MISRAStmtRule() = default;

private:
public:

}; // MISRAStmtRule

}
}
