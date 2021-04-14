#ifndef LABRADOR_DIAGNOSTIC_H
#define LABRADOR_DIAGNOSTIC_H

#include "diagnostic_dispatcher.h"

namespace xsca {
namespace rule {

class GJB5369Diagnostic : public clang::IgnoringDiagConsumer {
private:
  clang::CompilerInstance *_CI;
  clang::DiagnosticOptions _options;

  // FIXME: Have no idea why we can't free this pointer.
  clang::TextDiagnostic *_TextDiag;

public:
  GJB5369Diagnostic() = default;
  explicit GJB5369Diagnostic(clang::CompilerInstance *CI)
      : _CI(CI), _TextDiag(new clang::TextDiagnostic(llvm::errs(), CI->getLangOpts(), &_options)) {
    DBG_ASSERT(_TextDiag, "Init TextDiagnostic failed");
  }


  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) override;

  void AddIssue(const std::string &rule, const std::string &ref_msg, clang::SourceLocation location);

  void SetCI(clang::CompilerInstance *CI) { _CI = CI; }
};

}
}
#endif //LABRADOR_DIAGNOSTIC_H
