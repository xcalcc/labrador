#ifndef LABRADOR_GJB8114_DIAGNOSTIC_H
#define LABRADOR_GJB8114_DIAGNOSTIC_H

#include "rules/common/base_diagnostic.h"

namespace xsca {
namespace rule {

class GJB8114Diagnostic : public RuleBaseDiagnostic {
private:
public:
  GJB8114Diagnostic() = default;

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) override;

  void AddIssue(const char *rule, const std::string &ref_msg, clang::SourceLocation location) final;
};

}
}
#endif //LABRADOR_DIAGNOSTIC_H
