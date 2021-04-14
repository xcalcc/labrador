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
#include "GJB5369_diagnostic.h"

#include <map>
#include <utility>
#include <clang/Basic/Diagnostic.h>
#include <llvm/Support/raw_ostream.h>
#include <clang/Frontend/TextDiagnostic.h>
#include <clang/Frontend/CompilerInstance.h>

namespace xsca {

class DiagnosticDispatcher : public clang::DiagnosticConsumer {
private:
  clang::CompilerInstance *_CI;
  clang::DiagnosticOptions _options;

  // FIXME: Have no idea why we can't free this pointer.
  clang::TextDiagnostic *_TextDiag;

  std::map<const std::string, RuleDiagnostic *> _report_registers;

public:
  explicit DiagnosticDispatcher(clang::CompilerInstance *CI)
      : _CI(CI), _TextDiag(new clang::TextDiagnostic(llvm::errs(), CI->getLangOpts(), &_options)) {
    DBG_ASSERT(_TextDiag, "Init TextDiagnostic failed");

    RegisteDiagnostic(new GJB5369Diagnostic());
  };

  ~DiagnosticDispatcher() override {
    for (auto &it : _report_registers) {
      delete it.second;
    }
  }

  void RegisteDiagnostic(RuleDiagnostic *reporter) {
    _report_registers.insert(std::make_pair<const std::string, RuleDiagnostic *>(
        reporter->name(), reinterpret_cast<RuleDiagnostic *&&>(reporter)
    ));
  }

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) override;

  void AddIssue(const std::string &std_name, const std::string &rule,
                const std::string &ref_msg, clang::SourceLocation location) {
    _report_registers[std_name]->AddIssue(rule, ref_msg, location);
  }
};


}

#endif //LABRADOR_DIAGNOSTIC_DISPATCHER_H
