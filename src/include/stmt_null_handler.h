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
// stmt_null_handler.h
// ====================================================================
//
// stmt null handler which does nothing on clang Stmt
//

#ifndef STMT_NULL_HANDLER_INCLUDED
#define STMT_NULL_HANDLER_INCLUDED

#include "xsca_defs.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprConcepts.h"
#include "clang/AST/StmtObjC.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/StmtOpenMP.h"
#include "clang/AST/ExprOpenMP.h"

namespace xsca {

// class StmtNullHandler
class StmtNullHandler {
protected:
  bool _disabled;

public:
  inline bool Disabled() const { return _disabled; }

  // generate function prototype from StmtNodes.inc
  #define STMT(CLASS, PARENT) \
      void Visit##CLASS(const clang::CLASS *stmt) { \
      }
  #define ABSTRACT_STMT(STMT)
  # include "clang/AST/StmtNodes.inc"
  #undef ABSTRACT_STMT
  #undef STMT

  void VisitFunctionBody(const clang::Stmt *stmt) {}

  void VisitAtFunctionExit(const clang::Stmt *stmt) {}

public:
  void SetCurrentFunctionDecl(const clang::FunctionDecl *stmt) {}

};  // StmtNullHandler

}  // namespace xsca

#endif  // STMT_NULL_HANDLER_INCLUDED
