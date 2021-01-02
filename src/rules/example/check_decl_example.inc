/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// check_decl_example.inc
// ====================================================================
//
// example to describe how to write a checker to check clang Decl.
//

class CheckDeclExample : public DeclNullHandler {
public:
  ~CheckDeclExample() {
    TRACE0();
  }

public:
  void VisitLabel(const clang::LabelDecl *decl) {
    printf("Hit a LabelDecl.\n");
  }

};  // CheckDeclExample
