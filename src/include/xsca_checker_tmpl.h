/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
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
         typename _PPHandler>
class XcalCheckerTmpl : public XcalChecker {
private:
  using _ASTHandler  = XcalDeclVisitor<_DeclHandler, _StmtHandler, _TypeHandler>;
  using _ASTConsumer = XcalAstConsumer<_ASTHandler>;
  using _PPCallback  = XcalPPCallback<_PPHandler>;

  clang::CompilerInstance *_CI;
  _DeclHandler             _decl_handler;
  _StmtHandler             _stmt_handler;
  _TypeHandler             _type_handler;
  _ASTHandler              _ast_handler;
  _PPHandler               _pp_handler;

  std::function<void()>      _checkCallBack;
public:
  XcalCheckerTmpl(XcalCheckerManager *mgr)
      : XcalChecker(mgr),
        _CI(NULL),
        _checkCallBack([](){}),
        _ast_handler(_decl_handler, _stmt_handler, _type_handler) {}

  ~XcalCheckerTmpl() {
    Finalize();
  }

public:
  // Handle XcalChecker interfaces
  void Initialize(clang::CompilerInstance *CI,
                  llvm::StringRef InFile) override {
    DBG_ASSERT(!_CI && CI, "XcalCheckerTmpl initialized.\n");
    _CI = CI;
  }

  void Finalize() override {
    TRACE0();
    _checkCallBack();
  }

  std::unique_ptr<clang::ASTConsumer> GetAstConsumer() override {
    return std::make_unique<_ASTConsumer>(_ast_handler, _CI);
  }

  std::unique_ptr<clang::PPCallbacks> GetPPCallbacks() override {
    return std::make_unique<_PPCallback>(_pp_handler, _CI);
  }

  void SetCheckCallBack(std::function<void()> checkCallBack) {
    _checkCallBack = checkCallBack;
  }

};  // XcalCheckerTmpl

}  // namespace xsca

#endif  // XSCA_CHECKER_TMPL_INCLUDED
