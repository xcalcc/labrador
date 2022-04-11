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
// xsca_checker_manager.h
// ====================================================================
//
// interface for XSCA checker manager which is used to manage checker
// instances and factories
//

#ifndef XSCA_CHECKER_MANAGER_INCLUDED
#define XSCA_CHECKER_MANAGER_INCLUDED

#include "xsca_defs.h"
#include "xsca_checker.h"
#include "xsca_report.h"
#include "scope_manager.h"
#include "conf_manager.h"
#include "diagnostic_dispatcher.h"
#include <memory>
#include <vector>
#include <clang/AST/ASTContext.h>

namespace xsca {

class XcalCheckerManager;
class XcalDiagnosticFactory;

// class XcalAstCheckerFactory
class XcalCheckerFactory {
public:
  virtual std::unique_ptr<XcalChecker>
  CreateChecker(XcalCheckerManager *mgr) = 0;
};  // XcalCheckerFactory

// class XcalCheckerManager
class XcalCheckerManager {
private:
  std::unique_ptr<XcalReport>                           _report;
  std::unique_ptr<XcalChecker>                          _dump_checker;
  std::unique_ptr<ScopeManager>                         _scope_mgr;
  std::unique_ptr<ConfigureManager>                     _conf_mgr;
  std::vector< std::unique_ptr<XcalChecker> >           _checkers;
  std::vector< std::unique_ptr<XcalCheckerFactory> >    _factories;
  std::unique_ptr<clang::DiagnosticConsumer>            _diagnostic_mgr;

  const clang::FunctionDecl                            *_current_func;
  clang::SourceManager                                 *_source_mgr;

  clang::ASTContext                                    *_ast_context;

  static llvm::cl::opt<std::string>                     _enable_opt;

  XcalCheckerManager()
    : _scope_mgr(std::make_unique<ScopeManager>()) ,
      _conf_mgr(std::make_unique<ConfigureManager>("")) {}
  ~XcalCheckerManager() {}

  XcalCheckerManager(const XcalCheckerManager&)
      = delete;
  XcalCheckerManager& operator=(const XcalCheckerManager&)
      = delete;

private:
  static XcalCheckerManager _instance;

  void AddFactory(std::unique_ptr<XcalCheckerFactory> manager) {
    _factories.push_back(std::move(manager));
  }

  void AddChecker(std::unique_ptr<XcalChecker> checker) {
    _checkers.push_back(std::move(checker));
  }

  std::unique_ptr<clang::ASTConsumer>
  InitCheckers(clang::CompilerInstance &CI, llvm::StringRef InFile);

  void FiniCheckers();

public:
  static XcalReport *
  GetReport() {
    DBG_ASSERT(_instance._report.get() != nullptr, "report is null");
    return _instance._report.get();
  }

  static ScopeManager *
  GetScopeManager() {
    DBG_ASSERT(_instance._scope_mgr.get() != nullptr, "scope manager is null");
    return _instance._scope_mgr.get();
  }

  static clang::SourceManager *
  GetSourceManager() {
    return _instance._source_mgr;
  }

  static ConfigureManager *
  GetConfigureManager() {
    DBG_ASSERT(_instance._conf_mgr.get() != nullptr, "configure manager is null");
    return _instance._conf_mgr.get();
  }

  static clang::ASTContext *
  GetAstContext() {
    DBG_ASSERT(_instance._ast_context != nullptr, "astcontext is null");
    return  _instance._ast_context;
  }

  static void SetCurrentFunction(const clang::FunctionDecl *decl) {
    _instance._current_func = decl;
  }

  static const clang::FunctionDecl *
  GetCurrentFunction() {
    return _instance._current_func;
  }

  static std::unique_ptr<clang::ASTConsumer>
  Initialize(clang::CompilerInstance &CI, llvm::StringRef InFile) {
    return _instance.InitCheckers(CI, InFile);
  }

  static void Finalize() {
    _instance.FiniCheckers();
    _instance._report->Finalize();
  }

  static void RegisterFactory(std::unique_ptr<XcalCheckerFactory> factory) {
    _instance.AddFactory(std::move(factory));
  }

  static llvm::cl::opt<std::string>& GetEnableOption() {
    return _instance._enable_opt;
  }

  static int RunCommand(const std::string &command, std::string &std_out);
};

template<typename _CheckerFactory>
class XcalCheckerFactoryRegister {
public:
  XcalCheckerFactoryRegister() {
    auto factory = std::make_unique<_CheckerFactory>();
    XcalCheckerManager::RegisterFactory(std::move(factory));
  }
};


}  // namespace xsca


#endif  // XSCA_CHECKER_MANAGER_INCLUDED
