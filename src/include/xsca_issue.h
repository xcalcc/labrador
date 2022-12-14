/*
   Copyright (C) 2021 Xcalibyte (Shenzhen) Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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

#include "xsca_defs.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceLocation.h"
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
  const char               *_std_name;    // std name, say "GJB5369", "MISRA"
  const char               *_rule_name;   // rule name, say "4.1.1.1", "Rule 1.1"
  std::string               _decl_name;   // decl name for function, variable, type
  std::string               _ref_msg;     // message for reference
  std::string               _func_name;   // function name for issue
  std::vector<XcalPathInfo> _path_info;   // path info for this path

  bool                      _need_ignore; // ignore if it is std library
  bool                      _is_maybe;    // if this result is possible FP

private:
  void setFunctionName(const clang::Stmt *stmt);
  void setFunctionName(const clang::Decl *stmt);

public:
  XcalIssue(const char *std, const char *rule)
    : _std_name(std), _rule_name(rule), _need_ignore(false), _is_maybe(false) {
  }

  XcalIssue(const char *std, const char *rule, const clang::Decl *decl)
    : _std_name(std), _rule_name(rule), _need_ignore(false), _is_maybe(false) {
    _path_info.push_back(XcalPathInfo(decl));
    if (clang::isa<clang::NamedDecl>(decl)) {
      _decl_name = clang::cast<clang::NamedDecl>(decl)->getNameAsString();
      if (clang::isa<clang::FunctionDecl>(decl)) {
        _func_name = clang::cast<clang::FunctionDecl>(decl)->getNameAsString();
      }
    }
  }

  XcalIssue(const char *std, const char *rule, const clang::Stmt *stmt)
    : _std_name(std), _rule_name(rule), _need_ignore(false), _is_maybe(false) {
    _path_info.push_back(XcalPathInfo(stmt));
  }

  XcalIssue(const char *std, const char *rule, clang::SourceLocation location)
    : _std_name(std), _rule_name(rule), _need_ignore(false), _is_maybe(false) {
    _path_info.push_back(XcalPathInfo(location));
  }

public:
  void SetName(const std::string &name) {
    _decl_name = name;
  }

  void SetRefMsg(const std::string &msg) {
    _ref_msg = msg;
  }

  void SetIgnore(bool ignore) {
    _need_ignore = ignore;
  }

  void SetIsMaybe(bool maybe = true) {
    _is_maybe = maybe;
  }

  void SetFuncName(const std::string &name) {
    _func_name = name;
  }

  void AddDecl(const clang::Decl *decl) {
    _path_info.push_back(XcalPathInfo(decl));
  }

  void AddStmt(const clang::Stmt *stmt) {
    _path_info.push_back(XcalPathInfo(stmt));
  }

  void AddLocation(const clang::SourceLocation loc) {
    _path_info.push_back(XcalPathInfo(loc));
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

  const char *FuncName() const {
    return _func_name.c_str();
  }

  bool IsIgnore() const {
    return _need_ignore;
  }

  bool IsMaybe() const {
    return _is_maybe;
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
