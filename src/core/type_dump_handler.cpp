/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// type_dump_handler.cpp
// ====================================================================
//
// type dump handler which dump clang Type for reference
//

#include "type_dump_handler.h"

namespace xsca {

// generate function body from TypeNodes.inc
#define TYPE(CLASS, BASE) \
    void \
    TypeDumpHandler::Visit##CLASS(const clang::CLASS##Type *type) { \
      printf("==== %s ====\n", type->getTypeClassName()); \
      type->dump(); \
    }
#define ABSTRACT_TYPE(CLASS, BASE)
# include "clang/AST/TypeNodes.inc"
#undef ABSTRACT_TYPE
#undef TYPE

}  // namespace xsca
