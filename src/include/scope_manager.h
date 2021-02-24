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
#include "clang/AST/Decl.h"
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
  std::unordered_multimap<std::string, const clang::FunctionDecl*> _id_to_func;
  // map identifier to VarDecl
  std::unordered_multimap<std::string, const clang::VarDecl*>      _id_to_var;
  // map identifier to ValueDecl
  std::unordered_multimap<std::string, const clang::ValueDecl*>    _id_to_value;
  // map identifier to TypeDecl
  std::unordered_multimap<std::string, const clang::TypeDecl*>     _id_to_type;
  // map identifier to LabelDecl
  std::unordered_multimap<std::string, const clang::LabelDecl*>    _id_to_label;
  // map identifier to FieldDecl
  std::unordered_multimap<std::string, const clang::FieldDecl*>    _id_to_field;

  // current scope
  LexicalScope *_scope;

  IdentifierManager(const IdentifierManager&)
      = delete;
  IdentifierManager& operator=(const IdentifierManager&)
      = delete;

public:
  IdentifierManager(LexicalScope *scope) : _scope(scope) { }

public:
  void AddIdentifier(const clang::FunctionDecl *decl) {
    // TODO
    DBG_ASSERT(decl != nullptr, "FunctionDecl is null");
    std::string func_name = decl->getNameAsString();
    _id_to_func.emplace(std::make_pair(func_name, decl));
  }

  void AddIdentifier(const clang::VarDecl *decl) {
    DBG_ASSERT(decl != nullptr, "VarDecl is null");
    std::string var_name = decl->getNameAsString();
    _id_to_var.emplace(std::make_pair(var_name, decl));
  }

  void AddIdentifier(const clang::TypeDecl *decl) {
    DBG_ASSERT(decl != nullptr, "TypeDecl is null");
    std::string type_name = decl->getNameAsString();
    _id_to_type.emplace(std::make_pair(type_name, decl));
  }

  void AddIdentifier(const clang::LabelDecl *decl) {
    DBG_ASSERT(decl != nullptr, "LabelDecl is null");
    std::string label_name = decl->getNameAsString();
    _id_to_label.emplace(std::make_pair(label_name, decl));
  }

  void AddIdentifier(const clang::ValueDecl *decl) {
    DBG_ASSERT(decl != nullptr, "LabelDecl is null");
    std::string label_name = decl->getNameAsString();
    _id_to_value.emplace(std::make_pair(label_name, decl));
  }

  void AddIdentifier(const clang::FieldDecl *decl) {
    DBG_ASSERT(decl != nullptr, "FieldDecl is null");
    std::string field_name = decl->getNameAsString();
    _id_to_field.emplace(std::make_pair(field_name, decl));
  }

public:
  // TODO: lookup identifier
  /* Find function decl from current scope. */
  const clang::FunctionDecl *FindFunctionDecl(std::string &func_name) {
    auto num = _id_to_func.count(func_name);
    if (num == 0) {
      return nullptr;
    } else if (num == 1) {
      return _id_to_func.find(func_name)->second;
    } else {
      // TODO: _id_to_func may contain multi functions with the same name
    }
  }

  /* Check if the identifier is in the function map. */
  bool HasFunctionName(const std::string &func_name) {
    return _id_to_func.count(func_name) > 0;
  }

  /* Check if the identifier is in the variable map. */
  bool HasVariableName(const std::string &var_name, bool recursive=false) const;

private:
  template<typename _MAP>
  static void Dump(int depth, const _MAP &map, const char* name) {
    for (const auto &it : map) {
      printf("%*s%s: %s\n",
             depth, " ", name, it.first.c_str());
    }
  }

  template<typename _MAP, typename _RULE>
  void TraverseMap(const _MAP &map, const _RULE &rule) {
    for (const auto &it : map) {
      rule(it.first, this);
    }
  }

public:
  enum IdentifierKind {
    FUNCTION = 0x01,
    VAR      = 0x02,
    VALUE    = 0x04,
    TYPE     = 0x08,
    LABEL    = 0x10,
    FIELD    = 0x20,
    // combinations for readability
    NONE     = 0x00,
    ALL      = 0xff,
    NON_FUNC = (ALL & (~FUNCTION)),
  };

  void Dump(int depth) const {
    Dump(depth, _id_to_func, "Function");
    Dump(depth, _id_to_var, "Variable");
    Dump(depth, _id_to_value, "Value");
    Dump(depth, _id_to_type, "Type");
    Dump(depth, _id_to_label, "Label");
    Dump(depth, _id_to_field, "Field");
  }

  template<uint32_t _IDKIND, typename _RULE>
  void TraverseAll(const _RULE& rule) {
    if ((_IDKIND & IdentifierKind::FUNCTION) != 0)
      TraverseMap(_id_to_func,  rule);
    if ((_IDKIND & IdentifierKind::VAR) != 0)
      TraverseMap(_id_to_var,   rule);
    if ((_IDKIND & IdentifierKind::VALUE) != 0)
      TraverseMap(_id_to_value, rule);
    if ((_IDKIND & IdentifierKind::TYPE) != 0)
      TraverseMap(_id_to_type,  rule);
    if ((_IDKIND & IdentifierKind::LABEL) != 0)
      TraverseMap(_id_to_label, rule);
    if ((_IDKIND & IdentifierKind::FIELD) != 0)
      TraverseMap(_id_to_field, rule);
  }

};  // IdentifierManager

// class LexicalScope
// manages identifiers declared in this lexical scope
class LexicalScope {
private:
  // ScopeId is formed by higher 30/62 bit for scope pointer, which may be
  // FunctionDecl, CompoundStmt, or LambdaExpr, and lower 2 bit for kind
  using ScopeId = llvm::PointerIntPair<const void *, 2>;
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
  LexicalScope(LexicalScope *parent, const clang::TranslationUnitDecl *decl)
    : _scope(decl, SK_GLOBAL), _parent(parent),
      _identifiers(std::make_unique<IdentifierManager>(this)) {
    DBG_ASSERT(parent == nullptr, "global scope parent is not null");
  }

  // Function scope
  LexicalScope(LexicalScope *parent, const clang::FunctionDecl *decl)
    : _scope(decl, SK_FUNCTION), _parent(parent),
      _identifiers(std::make_unique<IdentifierManager>(this)) { }

  // Block scope
  LexicalScope(LexicalScope *parent, const clang::CompoundStmt *stmt)
    : _scope(stmt, SK_BLOCK), _parent(parent),
      _identifiers(std::make_unique<IdentifierManager>(this)) { }

  // Lambda scope
  LexicalScope(LexicalScope *parent, const clang::LambdaExpr *expr)
      : _scope(expr, SK_LAMBDA), _parent(parent),
        _identifiers(std::make_unique<IdentifierManager>(this)) { }

public:
  // Add identifier IdentifierManager
  template<typename _DECL>
  void AddIdentifier(const _DECL *decl) {
    _identifiers->AddIdentifier(decl);
  }

  // TODO: lookup identifier
  const clang::FunctionDecl *FindFunctionDecl(std::string &func_name) {
    return _identifiers->FindFunctionDecl(func_name);
  }

  /* Check if the identifier is in the function map. */
  bool HasFunctionName(const std::string &func_name) {
    return _identifiers->HasFunctionName(func_name);
  }

  bool HasVariableName(const std::string &var_name, bool recursive=false) const;

public:
  // Create new child scope started by _NODE, add to children vector
  // and return the new scope
  template<typename _NODE>
  LexicalScope *NewScope(const _NODE *node) {
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

  const LexicalScopeVec &Children() const {
    return _children;
  }

  ScopeKind GetScopeKind() const {
    return static_cast<ScopeKind>(_scope.getInt());
  }

public:
  // Dump scope info
  void Dump(bool recursive, int depth = 1) {
    _identifiers->Dump(depth);
    if (recursive){
      depth++;
      for (const auto &it : _children) {
          it->Dump(recursive, depth);
      }
    }
  }

  template<uint32_t _IDKIND>
  void TraverseAll(const std::function<void(const std::string&, IdentifierManager *)>& rule) {
    _identifiers->TraverseAll<_IDKIND>(rule);
    for (const auto &it : _children) {
      it->TraverseAll<_IDKIND>(rule);
    }
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
  void InitializeScope(const clang::TranslationUnitDecl *decl) {
    DBG_ASSERT(_root.get() == nullptr, ("root is not null"));
    _root = std::make_unique<LexicalScope>(nullptr, decl);
    _current = _root.get();
  }

  // Finalize global scope
  void FinalizeScope(const clang::TranslationUnitDecl *decl) {
    DBG_ASSERT(_current == _root.get(), "current scope mismatch");
    DBG_ASSERT(_current->GetNode<const clang::TranslationUnitDecl>() == decl,
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

  void DumpAll() const {
    _root->Dump(true);
  }

};  // ScopeManager

// class ScopeHelper
// helper class to push and pop scope
template<typename _NODE>
class ScopeHelper {
private:
  ScopeManager       *_mgr;     // Scope manager
  const _NODE        *_node;    // node starts the scope

public:
  // Constructor, push the scope
  ScopeHelper(ScopeManager* mgr, const _NODE *node)
      :_mgr(mgr), _node(node) {
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
  ScopeHelper(ScopeManager* mgr, clang::TranslationUnitDecl *node)
  :_mgr(mgr), _node(node) {
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

  void DumpAll() {
    _mgr->DumpAll();
  }
};  // ScopeHelper<clang::TranslationUnitDecl>

}  // namespace xsca

#endif  // SCOPE_MANAGER_INCLUDED
