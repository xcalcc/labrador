/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB5369_diagnostic.h
// ====================================================================
//
// GJB5369 diagnostic which collect and report the clang's error report
//

#ifndef GJB5369_DIAGNOSTIC
#define GJB5369_DIAGNOSTIC

#include "xsca_report.h"
#include "base_diagnostic.h"
#include "xsca_checker_manager.h"
#include "rules/GJB5369/GJB5369_enum.inc"

namespace xsca {

class GJB5369Diagnostic : public RuleDiagnostic {
public:
  GJB5369Diagnostic() : RuleDiagnostic("GJB5369") {}

  ~GJB5369Diagnostic() override = default;

  void AddIssue(const std::string &rule, const std::string &ref_msg, clang::SourceLocation location) override {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB5369, rule.c_str(), location);
    issue->SetRefMsg(ref_msg);
  }
};

}

#endif