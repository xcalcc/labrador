#include "diagnostic.h"
#include "xsca_report.h"
#include "cert_enum.inc"
#include "xsca_checker_manager.h"
#include <clang/Basic/DiagnosticSema.h>

namespace xsca {
namespace rule {

void CERTDiagnostic::HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                                      const clang::Diagnostic &diagnosticInfo) {

}

void CERTDiagnostic::AddIssue(const char *rule, const std::string &ref_msg, clang::SourceLocation location) {

}
}
}