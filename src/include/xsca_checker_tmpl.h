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
// xsca_checker_tmpl.h
// ====================================================================
//
// interface for checker template
//

#ifndef XSCA_CHECKER_TMPL_INCLUDED
#define XSCA_CHECKER_TMPL_INCLUDED

#include "xsca_defs.h"
#include "xsca_checker_manager.h"
#include "decl_visitor.h"
#include "ast_consumer.h"
#include "pp_callback.h"

namespace xsca {

// class XcalCheckerTmpl
template<typename _DeclHandler,
         typename _StmtHandler,
         typename _TypeHandler,
         typename _PPHandler,
         typename _DiagHandler>
class XcalCheckerTmpl : public XcalChecker {
private:
  using _ASTHandler  = XcalDeclVisitor<_DeclHandler, _StmtHandler, _TypeHandler>;
  using _ASTConsumer = XcalAstConsumer<_ASTHandler>;
  using _PPCallback  = XcalPPCallback<_PPHandler>;

  clang::CompilerInstance *_CI;
  _DeclHandler             _decl_handler;
  _StmtHandler             _stmt_handler;
  _TypeHandler             _type_handler;
  _PPHandler               _pp_handler;
  _ASTHandler              _ast_handler;

public:
  XcalCheckerTmpl(XcalCheckerManager *mgr)
      : XcalChecker(mgr),
        _CI(NULL),
        _ast_handler(_decl_handler, _stmt_handler, _type_handler) {}

  ~XcalCheckerTmpl() {}

public:
  // Handle XcalChecker interfaces
  void Initialize(clang::CompilerInstance *CI,
                  llvm::StringRef InFile) override {
    DBG_ASSERT(!_CI && CI, "XcalCheckerTmpl initialized.\n");
    _CI = CI;
  }

  std::unique_ptr<clang::ASTConsumer> GetAstConsumer() override {
    return std::make_unique<_ASTConsumer>(_ast_handler, _CI);
  }

  std::unique_ptr<clang::PPCallbacks> GetPPCallbacks() override {
    return std::make_unique<_PPCallback>(_pp_handler, _CI);
  }

  std::unique_ptr<clang::DiagnosticConsumer>
  GetDiagnosticConsumer() override{
    return std::make_unique<_DiagHandler>();
  }

};  // XcalCheckerTmpl

}  // namespace xsca

#endif  // XSCA_CHECKER_TMPL_INCLUDED
