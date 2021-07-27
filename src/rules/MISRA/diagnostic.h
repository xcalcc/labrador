#ifndef LABRADOR_MISRA_DIAGNOSTIC_H
#define LABRADOR_MISRA_DIAGNOSTIC_H

#include "rules/common/base_diagnostic.h"

namespace xsca {
namespace rule {
class MISRADiagnostic : public RuleBaseDiagnostic {
private:

public:
  MISRADiagnostic() = default;

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) override;

  void AddIssue(const char *rule, const std::string &ref_msg, clang::SourceLocation location);
};
}
}

#endif //LABRADOR_DIAGNOSTIC_H
