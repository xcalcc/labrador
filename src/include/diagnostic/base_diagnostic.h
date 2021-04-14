/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// base_diagnostic.h
// ====================================================================
//
// base diagnostic which collect and report the clang's error report
//

#ifndef LABRADOR_BASE_DIAGNOSTIC_H
#define LABRADOR_BASE_DIAGNOSTIC_H

#include <string>
#include <clang/Basic/Diagnostic.h>

#include "xsca_defs.h"

namespace xsca {

class RuleDiagnostic {
private:
  const std::string _std_name;
public:
  explicit RuleDiagnostic(std::string name) : _std_name(std::move(name)) {}

  virtual ~RuleDiagnostic() = default;;

  virtual void AddIssue(const std::string &rule, const std::string &ref_msg, clang::SourceLocation location) = 0;

  virtual bool HandleReport(const clang::Diagnostic &diagnosticInfo) = 0;

  virtual std::string name() const { return _std_name; }
};
}


#endif //LABRADOR_BASE_DIAGNOSTIC_H
