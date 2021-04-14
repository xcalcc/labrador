#ifndef LABRADOR_XSCA_DIAGNOSTICS_H
#define LABRADOR_XSCA_DIAGNOSTICS_H

#include "xsca_defs.h"
#include "diagnostic_dispatcher.h"

// add all decl rules into macro below
#define ALL_DECL_RULES(R)                      \
  R(GJB5369Diagnostic,   "GJB5369Diagnostic")
//  R(GJB8114DeclRule,   "GJB8114DeclRule"),     \
//  R(MISRADeclRule,     "MISRADeclRule"),       \
//  R(SJT11682DeclRule,  "SJT11682DeclRule")

#include "GJB5369/diagnostic.h"

namespace xsca {
using namespace rule;

#define GetClass(CLASS, ...) CLASS

using RuleDiagnosticDispatcher = DiagnosticDispatcher<ALL_DECL_RULES(GetClass)>;
using RuleDiagnosticManager = DiagnosticManager<RuleDiagnosticDispatcher>;
}

#endif //LABRADOR_XSCA_DIAGNOSTICS_H
