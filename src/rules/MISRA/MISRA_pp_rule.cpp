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
 * Rule: 21.5
 * The standard header file <signal.h> shall not be used
 */
void MISRAPPRule::CheckFidHeaderFile(clang::SourceLocation Loc, llvm::StringRef IncludedFilename) {
  auto conf_mgr = XcalCheckerManager::GetConfigureManager();
  if (conf_mgr->IsForbidHeaderFile(IncludedFilename.str())) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    if (IncludedFilename.str() == "stdarg.h")
      issue = report->ReportIssue(MISRA, M_R_17_1, Loc);
    else if (IncludedFilename.str() == "signal.h")
      issue = report->ReportIssue(MISRA, M_R_21_5, Loc);
    std::string ref_msg = "The features of " + IncludedFilename.str() + " shall not be used";
    issue->SetRefMsg(ref_msg);
  }
}

}
}
