#include "xsca_report.h"
#include "xsca_checker_manager.h"
#include "diagnostic/diagnostic_dispatcher.h"

#include <llvm/ADT/SmallString.h>
#include <clang/Basic/DiagnosticSema.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>

namespace xsca {

void DiagnosticDispatcher::HandleDiagnostic(
    clang::DiagnosticsEngine::Level diagnosticLevel,
    const clang::Diagnostic &diagnosticInfo) {

  clang::DiagnosticConsumer::HandleDiagnostic(diagnosticLevel, diagnosticInfo);

  clang::SmallString<100> diagnosticMessage;
  diagnosticInfo.FormatDiagnostic(diagnosticMessage);
  clang::SourceLocation location = diagnosticInfo.getLocation();

  switch (diagnosticInfo.getID()) {
    case clang::diag::err_array_size_non_int: {
      _report_registers[GJB5369]->
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

} // xsca