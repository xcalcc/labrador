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
// xsca_null_checker.h
// ====================================================================
//
// interface for XSCA null checker which does nothing
//

#ifndef XSCA_NULL_CHECKER_INCLUDED
#define XSCA_NULL_CHECKER_INCLUDED

#include "xsca_defs.h"
#include "decl_null_handler.h"
#include "pp_null_handler.h"
#include "ast_consumer.h"
#include "pp_callback.h"
#include "xsca_checker.h"

namespace xsca {

class XcalCheckerManager;

// class XcalDeclNullVisitor
class XcalDeclNullHandler : public DeclNullHandler {
public:
  void Visit(const clang::Decl *decl) {}
};  // XcalDeclNullVisitor

// class XcalNullChecker
class XcalNullChecker : public XcalChecker {
private:
  clang::CompilerInstance *_CI;

  XcalDeclNullHandler      _decl_handler;
  using XcalNullConsumer = XcalAstConsumer<XcalDeclNullHandler>;

  PPNullHandler            _pp_handler;
  using XcalNullCallback = XcalPPCallback<PPNullHandler>;

public:
  XcalNullChecker(XcalCheckerManager *mgr)
      : XcalChecker(mgr) , _CI(NULL) {}

  ~XcalNullChecker() {}

public:
  // Handle XcalChecker interfaces
  void Initialize(clang::CompilerInstance *CI,
                  llvm::StringRef InFile) override {
    DBG_ASSERT(!_CI && CI, "XcalNullChecker initialized.\n");
    _CI = CI;
  }

  std::unique_ptr<clang::ASTConsumer> GetAstConsumer() override {
    return std::make_unique<XcalNullConsumer>(_decl_handler, _CI);
  }

  std::unique_ptr<clang::PPCallbacks> GetPPCallbacks() override {
    return std::make_unique<XcalNullCallback>(_pp_handler, _CI);
  }

  std::unique_ptr<clang::DiagnosticConsumer>
  GetDiagnosticConsumer() override {
    return nullptr;
  }

};  // XcalNullChecker

}  // name xsca


#endif  // XSCA_NULL_CHECKER_INCLUDED
