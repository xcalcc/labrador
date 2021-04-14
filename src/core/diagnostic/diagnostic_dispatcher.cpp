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


  bool handled;
  handled = _report_registers[GJB5369]->HandleReport(diagnosticInfo);

  if (!handled) {
    clang::SmallString<100> diagnosticMessage;
    diagnosticInfo.FormatDiagnostic(diagnosticMessage);
    clang::SourceLocation location = diagnosticInfo.getLocation();
    llvm::raw_svector_ostream DiagMessageStream(diagnosticMessage);
    auto full_loc = clang::FullSourceLoc(diagnosticInfo.getLocation(), diagnosticInfo.getSourceManager());
    _TextDiag->emitDiagnostic(full_loc, diagnosticLevel, DiagMessageStream.str(),
                              diagnosticInfo.getRanges(), diagnosticInfo.getFixItHints());
  }

}

} // xsca