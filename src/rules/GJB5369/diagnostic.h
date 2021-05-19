#ifndef LABRADOR_GJB5369_DIAGNOSTIC_H
#define LABRADOR_GJB5369_DIAGNOSTIC_H

#include "rules/common/base_diagnostic.h"

namespace xsca {
namespace rule {

class GJB5369Diagnostic : public RuleBaseDiagnostic {
private:
public:
  GJB5369Diagnostic() = default;

  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                        const clang::Diagnostic &diagnosticInfo) override;

  void AddIssue(const char *rule, const std::string &ref_msg, clang::SourceLocation location);
};

}
}
#endif //LABRADOR_DIAGNOSTIC_H