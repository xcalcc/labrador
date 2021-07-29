#include "diagnostic.h"
#include "xsca_report.h"
#include "MISRA_enum.inc"
#include "xsca_checker_manager.h"
#include <clang/Basic/DiagnosticSema.h>

namespace xsca {
namespace rule {


void MISRADiagnostic::HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                                       const clang::Diagnostic &diagnosticInfo) {
  clang::SmallString<100> diagnosticMessage;
  diagnosticInfo.FormatDiagnostic(diagnosticMessage);
  clang::SourceLocation location = diagnosticInfo.getLocation();
  auto msg = diagnosticMessage.c_str();

  switch (diagnosticInfo.getID()) {
    case 5705:  // MISRA 9.4
      AddIssue(M_R_9_4, msg, location);
      break;
    case 5418:  // MISRA 13.4
      AddIssue(M_R_13_4, msg, location);
      break;
    default:
      break;
  }
}

void MISRADiagnostic::AddIssue(const char *rule, const std::string &ref_msg, clang::SourceLocation location) {
  XcalIssue *issue;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, rule, location);
  issue->SetRefMsg(ref_msg);
}

}
}