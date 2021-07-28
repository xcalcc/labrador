#ifndef LABRADOR_CERT_DIAGNOSTIC_H
#define LABRADOR_CERT_DIAGNOSTIC_H

#include "rules/common/base_diagnostic.h"

namespace xsca {
namespace rule {
class CERTDiagnostic : public RuleBaseDiagnostic {
private:

public:
  CERTDiagnostic() = default;

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) override;

  void AddIssue(const char *rule, const std::string &ref_msg, clang::SourceLocation location) override;
};
}
}

#endif //LABRADOR_DIAGNOSTIC_H
