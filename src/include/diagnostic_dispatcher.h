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
  _FIRST                          _first;
  DiagnosticDispatcher<_REST...>  _rest;

public:
  DiagnosticDispatcher() = default;;

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) {
    _first.HandleDiagnostic(diagnosticLevel, diagnosticInfo);
    _rest.HandleDiagnostic(diagnosticLevel, diagnosticInfo);
  }
};

template<typename _FIRST>
class DiagnosticDispatcher<_FIRST> {
private:
  _FIRST                   _first;

public:
  DiagnosticDispatcher() = default;;

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) {
    _first.HandleDiagnostic(diagnosticLevel, diagnosticInfo);
  }
};

template<typename _DISPATCHER>
class DiagnosticManager : public clang::DiagnosticConsumer {
private:
  _DISPATCHER               _dispatcher;
public:
  DiagnosticManager() = default;

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) override {
    _dispatcher.HandleDiagnostic(diagnosticLevel, diagnosticInfo);
  }
};

class XscaDiagnosticConsumer : public clang::DiagnosticConsumer {
private:
  std::vector< std::unique_ptr<clang::DiagnosticConsumer> > _diagnostic_consumers;
  clang::CompilerInstance  *_CI;
  clang::DiagnosticOptions _options;

  // FIXME: Have no idea why we can't free this pointer.
  clang::TextDiagnostic   *_TextDiag;
public:

  explicit XscaDiagnosticConsumer(clang::CompilerInstance *CI)
  : _CI(CI),
    _TextDiag(new clang::TextDiagnostic(llvm::errs(), CI->getLangOpts(), &_options)) {
    DBG_ASSERT(_TextDiag, "Init TextDiagnostic failed");
  }
  void AddConsumer(std::unique_ptr<clang::DiagnosticConsumer> consumer) {
    _diagnostic_consumers.push_back(std::move(consumer));
  }

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) override {
    for (const auto &it : _diagnostic_consumers) {
      clang::DiagnosticConsumer::HandleDiagnostic(diagnosticLevel, diagnosticInfo);
      it->HandleDiagnostic(diagnosticLevel, diagnosticInfo);
    }
    HandleByTextDiagnostic(diagnosticLevel, diagnosticInfo);
  }

  void HandleByTextDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                              const clang::Diagnostic &diagnosticInfo) {
    clang::SmallString<100> diagnosticMessage;
    diagnosticInfo.FormatDiagnostic(diagnosticMessage);

    llvm::raw_svector_ostream DiagMessageStream(diagnosticMessage);
    auto full_loc = clang::FullSourceLoc(diagnosticInfo.getLocation(), diagnosticInfo.getSourceManager());
    _TextDiag->emitDiagnostic(full_loc, diagnosticLevel, DiagMessageStream.str(),
                              diagnosticInfo.getRanges(), diagnosticInfo.getFixItHints());
  }

};

}

#endif //LABRADOR_DIAGNOSTIC_DISPATCHER_H
