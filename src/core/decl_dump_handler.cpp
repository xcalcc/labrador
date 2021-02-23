/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// decl_dump_handler.cpp
// ====================================================================
//
// decl dump handler which dump clang Decl for reference
//

#include "decl_dump_handler.h"

namespace xsca {

// finalize dump handler
void
XcalDeclDumpHandler::Finalize() {
}

// generate function body from DeclNodes.inc
#define DECL(DERIVED, BASE) \
    void \
    XcalDeclDumpHandler::Visit##DERIVED(const clang::DERIVED##Decl *decl) { \
      printf("==== %s ====\n", decl->clang::Decl::getDeclKindName()); \
      decl->dump(); \
    }
#define ABSTRACT_DECL(DECL)
# include "clang/AST/DeclNodes.inc"
#undef ABSTRACT_DECL
#undef DECL

}  // namespace xsca

