#include "diagnostic.h"
#include "xsca_report.h"
#include "GJB5369_enum.inc"
#include "xsca_checker_manager.h"
#include <clang/Basic/DiagnosticSema.h>

namespace xsca {
namespace rule {

void GJB5369Diagnostic::HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                                         const clang::Diagnostic &diagnosticInfo) {
  clang::SmallString<100> diagnosticMessage;
  diagnosticInfo.FormatDiagnostic(diagnosticMessage);
  clang::SourceLocation location = diagnosticInfo.getLocation();

  switch (diagnosticInfo.getID()) {
    case clang::diag::err_array_size_non_int: {
      AddIssue(G4_15_1_6, "size of array has non-integer type 'float'", location);
      break;
    }
    default: {
      llvm::raw_svector_ostream DiagMessageStream(diagnosticMessage);
      auto full_loc = clang::FullSourceLoc(diagnosticInfo.getLocation(), diagnosticInfo.getSourceManager());
      _TextDiag->emitDiagnostic(full_loc, diagnosticLevel, DiagMessageStream.str(),
                                diagnosticInfo.getRanges(), diagnosticInfo.getFixItHints());
      break;
    }
  }
}

void GJB5369Diagnostic::AddIssue(const std::string &rule,
                                 const std::string &ref_msg,
                                 clang::SourceLocation location) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(GJB5369, rule.c_str(), location);
  issue->SetRefMsg(ref_msg);
}

}
}