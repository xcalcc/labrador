/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// scope_manager.h
// ====================================================================
//
// interface for scope manager
//

#ifndef SCOPE_MANAGER_INCLUDED
#define SCOPE_MANAGER_INCLUDED

#include "xsca_defs.h"
#include "llvm/ADT/PointerIntPair.h"
#include <vector>
#include <unordered_map>

namespace clang {

// forward declaration of clang classes
class TranslationUnitDecl;
class FunctionDecl;
class VarDecl;
class TypeDecl;
class LabelDecl;
class CompoundStmt;
class LambdaExpr;

};  // clang

namespace xsca {

// kind of scope
enum ScopeKind {
  SK_GLOBAL = 0,            // global scope
  SK_FUNCTION = 1,          // function scope
  SK_BLOCK = 2,             // lexical scope
  SK_LAMBDA = 3,            // lambda function scope
};  // ScopeKind

class LexicalScope;

// class IdentifierManager
// mapping from string to function/variable/type/label in the scope
class IdentifierManager {
private:
  // map identifier to FunctionDecl
  std::unordered_multimap<std::string, clang::FunctionDecl*> _id_to_func;
  // map identifier to VarDecl
  std::unordered_multimap<std::string, clang::VarDecl*>      _id_to_var;
  // map identifier to TypeDecl
  std::unordered_multimap<std::string, clang::TypeDecl*>     _id_to_type;
  // map identifier to LabelDecl
  std::unordered_multimap<std::string, clang::LabelDecl*>    _id_to_label;
  // current scope, seems useless. TODO: remove it later
  LexicalScope *_scope;

  IdentifierManager(const IdentifierManager&)
      = delete;
  IdentifierManager& operator=(const IdentifierManager&)
      = delete;

public:
  IdentifierManager(LexicalScope *scope) : _scope(_scope) { }

public:
  void AddIdentifier(clang::FunctionDecl *decl) {
    // TODO
  }

  void AddIdentifier(clang::VarDecl *decl);

  void AddIdentifier(clang::TypeDecl *decl);

  void AddIdentifier(clang::LabelDecl);

  // TODO: lookup identifier

};  // IdentifierManager

// class LexicalScope
// manages identifiers declared in this lexical scope
class LexicalScope {
private:
  // ScopeId is formed by higher 30/62 bit for scope pointer, which may be
  // FunctionDecl, CompoundStmt, or LambdaExpr, and lower 2 bit for kind
  using ScopeId = llvm::PointerIntPair<void *, 2>;
  // unique pointer for LexicalScope
  using LexicalScopePtr = std::unique_ptr<LexicalScope>;
  // vector of LexicalScope unique pointer
  using LexicalScopeVec = std::vector<LexicalScopePtr>;
  // unique pointer for IdentifierManager
  using IdentifierManagerPtr = std::unique_ptr<IdentifierManager>;

  ScopeId              _scope;        // current scope
  LexicalScope        *_parent;       // parent scope
  LexicalScopeVec      _children;     // children scopes
  IdentifierManagerPtr _identifiers;  // identifiers in this scope

  LexicalScope(const LexicalScope&)
      = delete;
  LexicalScope& operator=(const LexicalScope&)
      = delete;

public:
  // Global scope
  LexicalScope(LexicalScope *parent, clang::TranslationUnitDecl *decl)
    : _scope(decl, SK_GLOBAL), _parent(parent),
      _identifiers(std::make_unique<IdentifierManager>(this)) {
    DBG_ASSERT(parent == nullptr, "global scope parent is not null");
  }

  // Function scope
  LexicalScope(LexicalScope *parent, clang::FunctionDecl *decl)
    : _scope(decl, SK_FUNCTION), _parent(parent),
      _identifiers(std::make_unique<IdentifierManager>(this)) { }

  // Block scope
  LexicalScope(LexicalScope *parent, clang::CompoundStmt *stmt)
    : _scope(stmt, SK_BLOCK), _parent(parent),
      _identifiers(std::make_unique<IdentifierManager>(this)) { }

  // Lambda scope
  LexicalScope(LexicalScope *parent, clang::LambdaExpr *expr)
    : _scope(expr, SK_LAMBDA), _parent(parent),
      _identifiers(std::make_unique<IdentifierManager>(this)) { }

public:
  // Add identifier IdentifierManager
  template<typename _DECL>
  void AddIdentifier(_DECL *decl) {
    _identifiers->AddIdentifier(decl);
  }

  // TODO: lookup identifier

public:
  // Create new child scope started by _NODE, add to children vector
  // and return the new scope
  template<typename _NODE>
  LexicalScope *NewScope(_NODE *node) {
    LexicalScopePtr scope = std::make_unique<LexicalScope>(this, node);
    _children.push_back(std::move(scope));
    return _children.back().get();
  }

  // Get parent scope
  LexicalScope *Parent() const {
    DBG_ASSERT(_parent != nullptr, "parent scope is null");
    return _parent;
  }

  // Get node that starts this scope
  template<typename _NODE>
  _NODE *GetNode() const {
    return static_cast<_NODE*>(_scope.getPointer());
  }

};  // LexicalScope

// class ScopeManager
// manages all scope in tree structure
class ScopeManager {
private:
  std::unique_ptr<LexicalScope> _root;       // root scope
  LexicalScope                 *_current;    // current scope

  ScopeManager(const ScopeManager&)
      = delete;
  ScopeManager& operator=(const ScopeManager&)
      = delete;

public:
  // Constructor, set both root and current to empty/null
  ScopeManager() : _current(nullptr) { }

public:
  // Initialize global scope with clang TranslationUnitDecl
  void InitializeScope(clang::TranslationUnitDecl *decl) {
    DBG_ASSERT(_root.get() == nullptr, ("root is not null"));
    _root = std::make_unique<LexicalScope>(nullptr, decl);
    _current = _root.get();
  }

  // Finalize global scope
  void FinalizeScope(clang::TranslationUnitDecl *decl) {
    DBG_ASSERT(_current == _root.get(), "current scope mismatch");
    DBG_ASSERT(_current->GetNode<clang::TranslationUnitDecl>() == decl,
               "current scope mismatch");
  }

  // Start new scope and set as current scope
  template<typename _NODE>
  void PushScope(_NODE *node) {
    DBG_ASSERT(_current != nullptr, "current scope not set");
    _current = _current->NewScope<_NODE>(node);
  }

  // Finish current scope and switch to parent scope
  template<typename _NODE>
  void PopScope(_NODE *node) {
    DBG_ASSERT(_current != nullptr, "current scope not set");
    DBG_ASSERT(_current->GetNode<_NODE>() == node, "current scope mismatch");
    _current = _current->Parent();
  }

  // Get global scope
  LexicalScope *GlobalScope() const {
    DBG_ASSERT(_root.get() != nullptr, ("global scope not set"));
    return _root.get();
  }

  // Get current scope
  LexicalScope *CurrentScope() const {
    DBG_ASSERT(_current != nullptr, ("current scope not set"));
    return _current;
  }

};  // ScopeManager

// class ScopeHelper
// helper class to push and pop scope
template<typename _NODE>
class ScopeHelper {
private:
  ScopeManager *_mgr;     // Scope manager
  _NODE        *_node;    // node starts the scope

public:
  // Constructor, push the scope
  ScopeHelper(ScopeManager* mgr, _NODE *node) : _node(node) {
    _mgr->PushScope(_node);
  }

  // Destructor, pop the scope
  ~ScopeHelper() {
    _mgr->PopScope(_node);
  }

  // Return current scope
  LexicalScope *CurrentScope() const {
    return _mgr->CurrentScope();
  }
};  // ScopeHelper

// class ScopeHelper<clang::TranslationUnitDecl>
// helper class to initialize and finalize scope
template<>
class ScopeHelper<clang::TranslationUnitDecl> {
private:
  ScopeManager               *_mgr;    // Scope manager
  clang::TranslationUnitDecl *_node;   // Tanslation unit decl

public:
  // Constructor, initialize the scope
  ScopeHelper(ScopeManager* mgr, clang::TranslationUnitDecl *node) {
    _mgr->InitializeScope(_node);
  }

  // Destructor, finalize the scope
  ~ScopeHelper() {
    _mgr->FinalizeScope(_node);
  }

  // Return current scope
  LexicalScope *CurrentScope() const {
    return _mgr->CurrentScope();
  }
};  // ScopeHelper<clang::TranslationUnitDecl>

}  // namespace xsca

#endif  // SCOPE_MANAGER_INCLUDED
