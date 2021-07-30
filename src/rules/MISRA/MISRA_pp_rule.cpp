/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// MISRA_pp_rule.cpp
// ====================================================================
//
// implement all pp related rules in MISRA
//

#include "MISRA_enum.inc"
#include "MISRA_pp_rule.h"

namespace xsca {
namespace rule {

/* MISRA
 * Rule: 17.1
 * The features of <stdarg.h> shall not be used
 */
void MISRAPPRule::CheckStdArgHeaderFile(clang::SourceLocation Loc, llvm::StringRef IncludedFilename) {
  if (IncludedFilename.str() == "stdarg.h") {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_17_1, Loc);
    std::string ref_msg = "The features of <stdarg.h> shall not be used";
    issue->SetRefMsg(ref_msg);
  }
}

}
}
