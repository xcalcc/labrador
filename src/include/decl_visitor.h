/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// decl_visitor.h
// ====================================================================
//
// decl visitor which visit clang Decl and invoke decl/stmt/type handler
// respectively
//

#ifndef DECL_VISITOR_INCLUDED
#define DECL_VISITOR_INCLUDED

#include "type_visitor.h"
#include "stmt_visitor.h"

namespace xsca {

// class XcalDeclVisitor
template<typename _DeclHandler,
         typename _StmtHandler,
         typename _TypeHandler>
class XcalDeclVisitor {
  using _Self = XcalDeclVisitor<_DeclHandler, _StmtHandler, _TypeHandler>;
private:
  _DeclHandler &_decl_handler;
  XcalStmtVisitor<_StmtHandler, _Self>  _stmt_visitor;
  XcalTypeVisitor<_TypeHandler>         _type_visitor;

public:
  XcalDeclVisitor(_DeclHandler &decl_handler,
      _StmtHandler &stmt_handler,
      _TypeHandler &type_handler)
      : _decl_handler(decl_handler),
        _stmt_visitor(stmt_handler, *this),
        _type_visitor(type_handler) {}

  void Finalize() {
    _decl_handler.Finalize();
  }

  void VisitClassScopeFunctionSpecialization(
           const clang::ClassScopeFunctionSpecializationDecl *decl) {
    VisitCXXRecord(clang::cast<clang::CXXRecordDecl>(decl));
  }

  void VisitClassTemplatePartialSpecialization(
           const clang::ClassTemplatePartialSpecializationDecl *decl) {
    VisitCXXRecord(clang::cast<clang::CXXRecordDecl>(decl));
  }

  void VisitClassTemplateSpecialization(
           const clang::ClassTemplateSpecializationDecl *decl) {
    VisitCXXRecord(clang::cast<clang::CXXRecordDecl>(decl));
  }

  void VisitCXXConstructor(const clang::CXXConstructorDecl *decl) {
    VisitFunction(clang::cast<clang::FunctionDecl>(decl));
  }

  void VisitCXXConversion(const clang::CXXConversionDecl *decl) {
    VisitFunction(clang::cast<clang::FunctionDecl>(decl));
  }

  void VisitCXXDeductionGuide(const clang::CXXDeductionGuideDecl *decl) {
    VisitFunction(clang::cast<clang::FunctionDecl>(decl));
  }

  void VisitCXXDestructor(const clang::CXXDestructorDecl *decl) {
    VisitFunction(clang::cast<clang::FunctionDecl>(decl));
  }

  void VisitCXXMethod(const clang::CXXMethodDecl *decl) {
    VisitFunction(clang::cast<clang::FunctionDecl>(decl));
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    _decl_handler.VisitCXXRecord(decl);
    _type_visitor.Visit(decl->getTypeForDecl());

    // visit fields
    if (!decl->field_empty()) {
      for (const auto &it : decl->fields()) {
        _decl_handler.VisitField(it);
      }
    }

    // visit methods
    for (const auto &method : decl->methods()) {
      this->Visit(method);
    }
  }

  void VisitEnum(const clang::EnumDecl *decl) {
    _decl_handler.VisitEnum(decl);
    _type_visitor.Visit(decl->getTypeForDecl());
  }

  void VisitFunction(const clang::FunctionDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();

    // setup function scope
    ScopeHelper<clang::FunctionDecl> scope(scope_mgr, decl);

    _decl_handler.VisitFunction(decl);

    _type_visitor.Visit(decl->clang::ValueDecl::getType().getTypePtr());

    // visit parameters
    if (!decl->param_empty()) {
      for (const auto &it : decl->parameters()) {
        _decl_handler.VisitParmVar(it);
      }
    }

    // visit function body
    if (decl->doesThisDeclarationHaveABody()) {
      _stmt_visitor.SetCurrentFunctionDecl(decl);
      _stmt_visitor.VisitFunctionBody(decl->getBody());
    }
  }

  void VisitRecord(const clang::RecordDecl *decl) {
    _decl_handler.VisitRecord(decl);
    _type_visitor.Visit(decl->getTypeForDecl());
  }

  #define DECL(DERIVED, BASE) \
    void Visit##DERIVED(const clang::DERIVED##Decl *decl) { \
      _decl_handler.Visit##DERIVED(decl); \
    }
  #define ABSTRACT_DECL(TYPE)
  #define CLASSSCOPEFUNCTIONSPECIALIZATION(DERIVED, BASE)
  #define CLASSTEMPLATEPARTIALSPECIALIZATION(DERIVED, BASE)
  #define CLASSTEMPLATESPECIALIZATION(DERIVED, BASE)
  #define CXXCONSTRUCTOR(DERIVED, BASE)
  #define CXXCONVERSION(DERIVED, BASE)
  #define CXXDEDUCTIONGUIDE(DERIVED, BASE)
  #define CXXDESTRUCTOR(DERIVED, BASE)
  #define CXXMETHOD(DERIVED, BASE)
  #define CXXRECORD(DERIVED, BASE)
  #define ENUM(DERIVED, BASE)
  #define FUNCTION(DERIVED, BASE)
  #define RECORD(DERIVED, BASE)

//  #define VAR(DERIVED, BASE)
  # include "clang/AST/DeclNodes.inc"

  // general Visit method
  void Visit(const clang::Decl *decl) {
  switch (decl->getKind()) {
    #define DECL(DERIVED, BASE) \
    case clang::Decl::DERIVED: \
      Visit##DERIVED(clang::cast<clang::DERIVED##Decl>(decl)); \
      break;
    #define ABSTRACT_DECL(DECL)
    # include "clang/AST/DeclNodes.inc"

    default:
      TRACE("TODO: handle %s\n", decl->getDeclKindName());
      break;
    }

    if (decl->getKind() == clang::Decl::Var) {
      const auto *var_decl = clang::dyn_cast<clang::VarDecl>(decl);
      if (var_decl && var_decl->hasInit()) {
        _stmt_visitor.Visit(var_decl->getInit());
      }
    }
  }
};  // XcalDeclVisitor

}  // namespace xsca

#endif  // DECL_VISITOR_INCLUDED
