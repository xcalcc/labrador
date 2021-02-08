/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
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
#include <memory>
#include <vector>

namespace xsca {

class XcalCheckerManager;

// class XcalAstCheckerFactory
class XcalCheckerFactory {
public:
  virtual std::unique_ptr<XcalChecker>
  CreateChecker(XcalCheckerManager *mgr) = 0;
};  // XcalCheckerFactory

// class XcalCheckerManager
class XcalCheckerManager {
private:
  std::unique_ptr<XcalReport>                        _report;
  std::unique_ptr<XcalChecker>                       _dump_checker;
  std::unique_ptr<ScopeManager>                      _scope_mgr;
  std::vector< std::unique_ptr<XcalChecker> >        _checkers;
  std::vector< std::unique_ptr<XcalCheckerFactory> > _factories;

  XcalCheckerManager()
    : _scope_mgr(std::make_unique<ScopeManager>()) {}
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

  ScopeManager *etScopeManager() const {
  }

public:
  static ScopeManager *
  GetScopeManager() {
    DBG_ASSERT(_instance._scope_mgr.get() != nullptr, "scope manager is null");
    return _instance._scope_mgr.get();
  }

  static std::unique_ptr<clang::ASTConsumer>
  Initialize(clang::CompilerInstance &CI, llvm::StringRef InFile) {
    return _instance.InitCheckers(CI, InFile);
  }

  static void Finalize() {
    _instance.FiniCheckers();
  }

  static void RegisterFactory(std::unique_ptr<XcalCheckerFactory> factory) {
    _instance.AddFactory(std::move(factory));
  }
};

template<typename _CheckerFactory>
class XcalCheckerFactoryRegister {
public:
  XcalCheckerFactoryRegister() {
    TRACE0();
    auto factory = std::make_unique<_CheckerFactory>();
    XcalCheckerManager::RegisterFactory(std::move(factory));
  }
};

}  // namespace xsca


#endif  // XSCA_CHECKER_MANAGER_INCLUDED
