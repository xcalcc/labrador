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
// stmt_list_handler.h
// ====================================================================
//
// stmt list handler which call individual handlers on clang Stmt
//

#ifndef STMT_LIST_HANDLER_INCLUDED
#define STMT_LIST_HANDLER_INCLUDED

#include "xsca_defs.h"
#include "clang/AST/Stmt.h"

namespace xsca {

// class StmtListHandler with variadic template parameters as handlers
template<typename _First, typename... _Rest>
class StmtListHandler {
private:
  _First                    _first;
  StmtListHandler<_Rest...> _rest;

public:
  // generate Visit* function from StmtNodes.inc
  #define STMT(CLASS, PARENT)                              \
      void Visit##CLASS(const clang::CLASS *stmt) {        \
        if (!_first.Disabled()) _first.Visit##CLASS(stmt); \
        _rest.Visit##CLASS(stmt);                          \
      }
  #define ABSTRACT_STMT(STMT)
  # include "clang/AST/StmtNodes.inc"
  #undef ABSTRACT_STMT
  #undef STMT

  void VisitFunctionBody(const clang::Stmt *stmt) {
    if (!_first.Disabled()) _first.VisitFunctionBody(stmt);
    _rest.VisitFunctionBody(stmt);
  }

  void VisitAtFunctionExit(const clang::Stmt *stmt) {
    if (!_first.Disabled()) _first.VisitAtFunctionExit(stmt);
    _rest.VisitAtFunctionExit(stmt);
  }

public:
  void SetCurrentFunctionDecl(const clang::FunctionDecl *decl) {
    _first.SetCurrentFunctionDecl(decl);
    _rest.SetCurrentFunctionDecl(decl);
  }

};  // StmtListHandler

// class StmtListHandler with single template parameters as handlers
template<typename _First>
class StmtListHandler<_First> {
private:
  _First _first;

public:
  // generate Visit* function from StmtNodes.inc
  #define STMT(CLASS, PARENT) \
      void Visit##CLASS(const clang::CLASS *stmt) { \
        if (!_first.Disabled()) _first.Visit##CLASS(stmt); \
      }
  #define ABSTRACT_STMT(STMT)
  # include "clang/AST/StmtNodes.inc"
  #undef ABSTRACT_STMT
  #undef STMT

  void VisitFunctionBody(const clang::Stmt *stmt) {
    if (!_first.Disabled()) _first.VisitFunctionBody(stmt);
  }

  void VisitAtFunctionExit(const clang::Stmt *stmt) {
    if (!_first.Disabled()) _first.VisitAtFunctionExit(stmt);
  }

public:
  void SetCurrentFunctionDecl(const clang::FunctionDecl *decl) {
    _first.SetCurrentFunctionDecl(decl);
  }

};  // StmtListHandler

}  // namespace xsca

#endif  // STMT_LIST_HANDLER_INCLUDED
