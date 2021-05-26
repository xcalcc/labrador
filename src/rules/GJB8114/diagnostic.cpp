#include "diagnostic.h"
#include "xsca_report.h"
#include "GJB8114_enum.inc"
#include "xsca_checker_manager.h"
#include <clang/Basic/DiagnosticSema.h>

namespace xsca {
namespace rule {

void GJB8114Diagnostic::HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                                         const clang::Diagnostic &diagnosticInfo) {
  clang::SmallString<100> diagnosticMessage;
  diagnosticInfo.FormatDiagnostic(diagnosticMessage);
  clang::SourceLocation location = diagnosticInfo.getLocation();

  switch (diagnosticInfo.getID()) {
    case 1016:  // macro redefine TODO: replace the hardcode with clang::diag::blablabla
      AddIssue(G5_1_1_5, diagnosticMessage.c_str(), location);
    case 5589:  // GJB8114 5.1.1.19
      AddIssue(G5_1_1_19, diagnosticMessage.c_str(), location);
    case 4868:
      AddIssue(G5_3_1_2, diagnosticMessage.c_str(), location);
    case 5949:
      AddIssue(G5_3_1_3, diagnosticMessage.c_str(), location);
    default: {
      printf("%d\n",diagnosticInfo.getID());
      break;
    }
  }
}

void GJB8114Diagnostic::AddIssue(const char *rule,
                                 const std::string &ref_msg,
                                 clang::SourceLocation location) {
  XcalIssue *issue;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(GJB8114, rule, location);
  issue->SetRefMsg(ref_msg);
}

}
}