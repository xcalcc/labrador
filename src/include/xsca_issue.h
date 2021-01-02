/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_issue.h
// ====================================================================
//
// interface for XSCA issue
//

#ifndef XSCA_ISSUE_INCLUDED
#define XSCA_ISSUE_INCLUDED

#include "xsca_rule_meta.h"
#include "clang/Basic/SourceLocation.h"
#include <string>

namespace xsca {

// class XcalSCAIssue
class XcalSCAIssue {
private:
  const XcalSCARule    *_rule;           // which rule reports this issue
  clang::SourceLocation _beginLoc;       // begin location
  clang::SourceLocation _endLoc;         // end location
  std::string           _name;           // type/symbol/etc name
  std::string           _message;        // issue message
  std::string           _remediation;    // issue remediation

public:
  XcalSCAIssue(const XcalSCARule *rule) : _rule(rule) { }

  XcalSCAIssue(const XcalSCARule *rule,
      clang::SourceLocation begin,
      clang::SourceLocation end) : _rule(rule), _beginLoc(begin), _endLoc(end) { }

  ~XcalSCAIssue() { }

public:
  void SetName(const std::string name) { _name = name; }
  void SetMessage(const std::string msg) { _message = msg; }
  void SetRemediation(const std::string rem) { _remediation = rem; }

  std::string Name() const { return _name; }
  std::string Message() const { return _message; }
  std::string Remediation() const { return _remediation; }

};  // XcalSCAIssue

}  // name xsca


#endif  // XSCA_ISSUE_INCLUDED
