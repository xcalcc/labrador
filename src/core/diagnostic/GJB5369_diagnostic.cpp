#include <clang/Basic/DiagnosticSema.h>

#include "diagnostic/GJB5369_diagnostic.h"

namespace xsca {


bool GJB5369Diagnostic::HandleReport(const clang::Diagnostic &diagnosticInfo) {

  bool handled = false;

  clang::SmallString<100> diagnosticMessage;
  diagnosticInfo.FormatDiagnostic(diagnosticMessage);
  clang::SourceLocation location = diagnosticInfo.getLocation();
  switch (diagnosticInfo.getID()) {
    case clang::diag::err_array_size_non_int: {
      AddIssue(G4_15_1_6, "size of array has non-integer type 'float'", location);
      handled = true;
      break;
    }
    default: {
      break;
    }
  }
  return handled;
}

}
