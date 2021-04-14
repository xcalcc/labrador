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

#include "clang/Basic/SourceLocation.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Stmt.h"
#include <vector>
#include <string>

namespace xsca {

enum PathKind {
  PK_SPOT = 3,     // indicate the issue is checked at this spot
  PK_DECL = 4,     // indicate the related declaration for Func/Var/Type
};

// class XcalPathInfo
class XcalPathInfo {
public:
  clang::SourceLocation _start;        // start location of the path
  clang::SourceLocation _end;          // end location of the path
  PathKind              _kind;         // path kind

  XcalPathInfo(const clang::Decl *decl)
    : _start(decl->getBeginLoc()), _end(decl->getEndLoc()), _kind(PK_DECL) {
  }

  XcalPathInfo(const clang::Stmt *stmt)
    : _start(stmt->getBeginLoc()), _end(stmt->getEndLoc()), _kind(PK_SPOT) {
  }

  XcalPathInfo(const clang::SourceLocation start)
    : _start(start), _end(start), _kind(PK_SPOT) {

  }

  clang::SourceLocation Start() const {
    return _start;
  }

  PathKind Kind() const {
    return _kind;
  }
};

// class XcalIssue
class XcalIssue {
  friend class XcalReport;

private:
  const char               *_std_name;   // std name, say "GJB5369", "MISRA"
  const char               *_rule_name;  // rule name, say "4.1.1.1", "Rule 1.1"
  std::string               _decl_name;  // decl name for function, variable, type
  std::string               _ref_msg;    // message for reference
  std::vector<XcalPathInfo> _path_info;  // path info for this path

public:
  XcalIssue(const char *std, const char *rule)
    : _std_name(std), _rule_name(rule) {
  }

  XcalIssue(const char *std, const char *rule, const clang::Decl *decl)
    : _std_name(std), _rule_name(rule) {
    _path_info.push_back(XcalPathInfo(decl));
    if (clang::isa<clang::NamedDecl>(decl)) {
      _decl_name = clang::cast<clang::NamedDecl>(decl)->getNameAsString();
    }
  }

  XcalIssue(const char *std, const char *rule, const clang::Stmt *stmt)
    : _std_name(std), _rule_name(rule) {
    _path_info.push_back(XcalPathInfo(stmt));
  }

  XcalIssue(const char *std, const char *rule, clang::SourceLocation location)
    : _std_name(std), _rule_name(rule) {
    _path_info.push_back(XcalPathInfo(location));
  }

public:
  void SetName(const std::string &name) {
    _decl_name = name;
  }

  void SetRefMsg(const std::string &msg) {
    _ref_msg = msg;
  }

  void AddDecl(const clang::Decl *decl) {
    _path_info.push_back(XcalPathInfo(decl));
  }

  void AddStmt(const clang::Stmt *stmt) {
    _path_info.push_back(XcalPathInfo(stmt));
  }

public:
  const char *StdName() const {
    return _std_name;
  }

  const char *RuleName() const {
    return _rule_name;
  }

  const char *DeclName() const {
    return _decl_name.c_str();
  }

  const char *RefMessage() const {
    return _ref_msg.c_str();
  }

  const std::vector<XcalPathInfo>& PathInfo() const {
    return _path_info;
  }

  clang::SourceLocation GetLocation() const {
    DBG_ASSERT(_path_info.size() > 0, "no path info");
    return _path_info[0].Start();
  }

};  // XcalIssue

}  // name xsca


#endif  // XSCA_ISSUE_INCLUDED
