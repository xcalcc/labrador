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
  auto msg = diagnosticMessage.c_str();

  switch (diagnosticInfo.getID()) {
    case 1016:  // macro redefine TODO: replace the hardcode with clang::diag::blablabla
      AddIssue(G5_1_1_5, msg, location);
    case 5589:  // GJB8114 5.1.1.19
      AddIssue(G5_1_1_19, msg, location);
    case 4868:
      AddIssue(G5_3_1_2, msg, location);
    case 5949:
      AddIssue(G5_3_1_3, msg, location);
    case 5382:  // GJB8114 5.4.1.4
      AddIssue(G5_4_1_4, msg, location);
    case 5669:  // GJB8114 5.6.1.2
      AddIssue(G5_6_1_2, msg, location);
    case 5672:  // GJB8114 5.6.1.1
      AddIssue(G5_6_1_1, msg, location);
    case 5973:  // GJB8114 5.6.1.6
      AddIssue(G5_6_1_6, msg, location);
    case 5975: // GJB8114 5.6.1.7
      AddIssue(G5_6_1_7, msg, location);
    case 5945: // GJB8114 5.6.1.12
      AddIssue(G5_6_1_12, msg, location);
    case 5932: // GJB8114 5.7.1.1
      AddIssue(G5_7_1_1, msg, location);
    case 4206: // GJB8114 5.7.1.3
      AddIssue(G5_7_1_3, msg, location);
    case 2726: // GJB8114 5.7.1.5
      AddIssue(G5_7_1_5, msg, location);
    case 5591: // GJB8114 5.7.1.7
      AddIssue(G5_7_1_7, msg, location);
    default: {
      printf("%d\n", diagnosticInfo.getID());
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