/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// MISRA_pp_rule.h
// ====================================================================
//
// implement preprocess related rules in MISRA-C-2012
//

#include <clang/Lex/Preprocessor.h>
#include <vector>

namespace xsca {
namespace rule {
class MISRAPPRule : public PPNullHandler {
public:
  ~MISRAPPRule() {}

private:

public:

}; // MISRAPPRule

}
}