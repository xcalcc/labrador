/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// base_diagnostic.h
// ====================================================================
//
// the base diagnostic collector for checkers
//

#ifndef LABRADOR_BASE_DIAGNOSTIC_H
#define LABRADOR_BASE_DIAGNOSTIC_H

#include "diagnostic_dispatcher.h"

namespace xsca {
namespace rule {

class RuleBaseDiagnostic : public clang::IgnoringDiagConsumer {
private:
public:
  RuleBaseDiagnostic() = default;

  virtual void AddIssue(const char *rule, const std::string &ref_msg,
                        clang::SourceLocation location) = 0;
};

}
}
#endif //LABRADOR_BASE_DIAGNOSTIC_H
