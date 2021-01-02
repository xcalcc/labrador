/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// decl_null_handler.h
// ====================================================================
//
// decl null handler which does nothing on clang Decl
//

#ifndef DECL_NULL_HANDLER_INCLUDED
#define DECL_NULL_HANDLER_INCLUDED

#include "xsca_defs.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclOpenMP.h"

namespace xsca {

// class DeclNullHandler
class DeclNullHandler {

public:
  // generate function prototype from DeclNodes.inc
  #define DECL(DERIVED, BASE) \
    void Visit##DERIVED(const clang::DERIVED##Decl *decl) { \
    }
  #define ABSTRACT_DECL(DECL)
  # include "clang/AST/DeclNodes.inc"
  #undef ABSTRACT_DECL
  #undef DECL

};  // DeclNullHandler

}  // namespace xsca

#endif  // DECL_NULL_HANDLER_INCLUDED
