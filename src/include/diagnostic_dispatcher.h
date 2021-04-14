/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// diagnostic_dispatcher.h
// ====================================================================
//
// diagnostic dispatcher which collect and report the clang's error report
//

#ifndef LABRADOR_DIAGNOSTIC_DISPATCHER_H
#define LABRADOR_DIAGNOSTIC_DISPATCHER_H

#include "xsca_defs.h"

#include <utility>
#include <clang/Basic/Diagnostic.h>
#include <llvm/Support/raw_ostream.h>
#include <clang/Frontend/TextDiagnostic.h>
#include <clang/Frontend/CompilerInstance.h>

namespace xsca {

template<typename _FIRST, typename... _REST>
class DiagnosticDispatcher  {
private:
  clang::CompilerInstance        *_CI;

  _FIRST                          _first;
  DiagnosticDispatcher<_REST...>  _rest;

public:
  DiagnosticDispatcher() = default;;
  explicit DiagnosticDispatcher(clang::CompilerInstance *CI) : _CI(CI) {};

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) {
    _first.HandleDiagnostic(diagnosticLevel, diagnosticInfo);
    _rest.HandleDiagnostic(diagnosticLevel, diagnosticInfo);
  }

  void SetCI(clang::CompilerInstance *CI) {
    _CI = CI;
    _first.CI;
    _rest.SetCI(CI);
  }
};

template<typename _FIRST>
class DiagnosticDispatcher<_FIRST> {
private:
  _FIRST                   _first;
  clang::CompilerInstance *_CI;

  clang::DiagnosticOptions _options;

  // FIXME: Have no idea why we can't free this pointer.
  clang::TextDiagnostic   *_TextDiag{};

public:
  DiagnosticDispatcher() = default;;
  explicit DiagnosticDispatcher(clang::CompilerInstance *CI)
      : _CI(CI), _TextDiag(new clang::TextDiagnostic(llvm::errs(), CI->getLangOpts(), &_options)) {
    DBG_ASSERT(_TextDiag, "Init TextDiagnostic failed");
  }

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) {
    _first.HandleDiagnostic(diagnosticLevel, diagnosticInfo);
  }

  void SetCI(clang::CompilerInstance *CI) {
    _CI = CI;
    _first.SetCI(CI);
  }
};

template<typename _DISPATCHER>
class DiagnosticManager : public clang::DiagnosticConsumer {
private:
  _DISPATCHER               _dispatcher;
  clang::CompilerInstance  *_CI;
public:
  DiagnosticManager() = default;
  explicit DiagnosticManager(clang::CompilerInstance *CI) : _CI(CI) {
    _dispatcher.SetCI(_CI);
  }

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) override {
    _dispatcher.HandleDiagnostic(diagnosticLevel, diagnosticInfo);
  }

  void setCI(clang::CompilerInstance *CI) { _CI = CI; }
};

}

#endif //LABRADOR_DIAGNOSTIC_DISPATCHER_H
