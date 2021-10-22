/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// pp_list_handler.h
// ====================================================================
//
// preprocessor list handler which call individual handlers to handle
// preprocessor callbacks
//

#ifndef PP_LIST_CALLBACK_INCLUDED
#define PP_LIST_CALLBACK_INCLUDED

#include "xsca_defs.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/FileManager.h"

namespace xsca {

// class PPListHandler with variadic template parameters as handlers
template<typename _First, typename... _Rest>
class PPListHandler {
private:
  _First                   _first;
  PPListHandler<_Rest...>  _rest;

public:
  PPListHandler() {}

  // Handle PPCallbacks
  void FileChanged(clang::SourceLocation Loc,
           clang::PPCallbacks::FileChangeReason Reason,
           clang::SrcMgr::CharacteristicKind FileType,
           clang::FileID PrevFID) {
    if (!_first.Disabled()) _first.FileChanged(Loc, Reason, FileType, PrevFID);
    _rest.FileChanged(Loc, Reason, FileType, PrevFID);
  }

  void InclusionDirective(clang::SourceLocation DirectiveLoc,
           const clang::Token &IncludeToken,
           llvm::StringRef IncludedFilename,
           bool IsAngled,
           clang::CharSourceRange FilenameRange,
           const clang::FileEntry *IncludedFile,
           llvm::StringRef SearchPath,
           llvm::StringRef RelativePath,
           const clang::Module *Imported,
           clang::SrcMgr::CharacteristicKind FileType) {
    if (!_first.Disabled())
      _first.InclusionDirective(DirectiveLoc, IncludeToken, IncludedFilename,
                              IsAngled, FilenameRange, IncludedFile,
                              SearchPath, RelativePath, Imported, FileType);
    _rest.InclusionDirective(DirectiveLoc, IncludeToken, IncludedFilename,
                              IsAngled, FilenameRange, IncludedFile,
                              SearchPath, RelativePath, Imported, FileType);
  }

  void EndOfMainFile()
           {
    if (!_first.Disabled()) _first.EndOfMainFile();
    _rest.EndOfMainFile();
  }

  void Ident(clang::SourceLocation Loc,
           llvm::StringRef Str) {
    if (!_first.Disabled()) _first.Ident(Loc, Str);
    _rest.Ident(Loc, Str);
  }

  void PragmaDirective(clang::SourceLocation Loc,
           clang::PragmaIntroducerKind Introducer) {
    if (!_first.Disabled()) _first.PragmaDirective(Loc, Introducer);
    _rest.PragmaDirective(Loc, Introducer);
  }

  void PragmaComment(clang::SourceLocation Loc,
           const clang::IdentifierInfo *Kind,
           llvm::StringRef Str) {
    if (!_first.Disabled())  _first.PragmaComment(Loc, Kind, Str);
    _rest.PragmaComment(Loc, Kind, Str);
  }

  void PragmaDetectMismatch(clang::SourceLocation Loc,
           llvm::StringRef Name,
           llvm::StringRef Value) {
    if (!_first.Disabled()) _first.PragmaDetectMismatch(Loc, Name, Value);
    _rest.PragmaDetectMismatch(Loc, Name, Value);
  }

  void PragmaDebug(clang::SourceLocation Loc,
           llvm::StringRef DebugType) {
    if (!_first.Disabled()) _first.PragmaDebug(Loc, DebugType);
    _rest.PragmaDebug(Loc, DebugType);
  }

  void PragmaMessage(clang::SourceLocation Loc,
           llvm::StringRef Namespace,
           clang::PPCallbacks::PragmaMessageKind Kind,
           llvm::StringRef Str) {
    if (!_first.Disabled()) _first.PragmaMessage(Loc, Namespace, Kind, Str);
    _rest.PragmaMessage(Loc, Namespace, Kind, Str);
  }

  void PragmaDiagnosticPush(clang::SourceLocation Loc,
           llvm::StringRef Namespace) {
    if (!_first.Disabled()) _first.PragmaDiagnosticPush(Loc, Namespace);
    _rest.PragmaDiagnosticPush(Loc, Namespace);
  }

  void PragmaDiagnosticPop(clang::SourceLocation Loc,
           llvm::StringRef Namespace) {
    if (!_first.Disabled()) _first.PragmaDiagnosticPop(Loc, Namespace);
    _rest.PragmaDiagnosticPop(Loc, Namespace);
  }

  void PragmaDiagnostic(clang::SourceLocation Loc,
           llvm::StringRef Namespace,
           clang::diag::Severity Mapping,
           llvm::StringRef Str) {
    if (!_first.Disabled()) _first.PragmaDiagnostic(Loc, Namespace, Mapping, Str);
    _rest.PragmaDiagnostic(Loc, Namespace, Mapping, Str);
  }

  void HasInclude(clang::SourceLocation Loc,
           llvm::StringRef FileName,
           bool IsAngled,
           llvm::Optional<clang::FileEntryRef> File,
           clang::SrcMgr::CharacteristicKind FileType) {
    if (!_first.Disabled()) _first.HasInclude(Loc, FileName, IsAngled, File, FileType);
    _rest.HasInclude(Loc, FileName, IsAngled, File, FileType);
  }

  void PragmaOpenCLExtension(clang::SourceLocation NameLoc,
           const clang::IdentifierInfo *Name,
           clang::SourceLocation StateLoc,
           unsigned State) {
    if (!_first.Disabled()) _first.PragmaOpenCLExtension(NameLoc, Name, StateLoc, State);
    _rest.PragmaOpenCLExtension(NameLoc, Name, StateLoc, State);
  }

  void PragmaWarning(clang::SourceLocation Loc,
           llvm::StringRef WarningSpec,
           llvm::ArrayRef<int> Ids) {
    if (!_first.Disabled())  _first.PragmaWarning(Loc, WarningSpec, Ids);
    _rest.PragmaWarning(Loc, WarningSpec, Ids);
  }

  void PragmaWarningPush(clang::SourceLocation Loc,
           int Level) {
    if (!_first.Disabled()) _first.PragmaWarningPush(Loc, Level);
    _rest.PragmaWarningPush(Loc, Level);
  }

  void PragmaWarningPop(clang::SourceLocation Loc) {
    if (!_first.Disabled()) _first.PragmaWarningPop(Loc);
    _rest.PragmaWarningPop(Loc);
  }

  void PragmaAssumeNonNullBegin(clang::SourceLocation Loc) {
    if (!_first.Disabled()) _first.PragmaAssumeNonNullBegin(Loc);
    _rest.PragmaAssumeNonNullBegin(Loc);
  }

  void PragmaAssumeNonNullEnd(clang::SourceLocation Loc) {
    if (!_first.Disabled()) _first.PragmaAssumeNonNullEnd(Loc);
    _rest.PragmaAssumeNonNullEnd(Loc);
  }

  void MacroExpands(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD,
           clang::SourceRange Range,
           const clang::MacroArgs *Args) {
    if (!_first.Disabled()) _first.MacroExpands(MacroNameTok, MD, Range, Args);
    _rest.MacroExpands(MacroNameTok, MD, Range, Args);
  }

  void MacroDefined(const clang::Token &MacroNameTok,
           const clang::MacroDirective *MD) {
    if (!_first.Disabled()) _first.MacroDefined(MacroNameTok, MD);
    _rest.MacroDefined(MacroNameTok, MD);
  }

  void MacroUndefined(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD,
           const clang::MacroDirective *Undef) {
    if (!_first.Disabled()) _first.MacroUndefined(MacroNameTok, MD, Undef);
    _rest.MacroUndefined(MacroNameTok, MD, Undef);
  }

  void Defined(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD,
           clang::SourceRange Range) {
    if (!_first.Disabled()) _first.Defined(MacroNameTok, MD, Range);
    _rest.Defined(MacroNameTok, MD, Range);
  }

  void SourceRangeSkipped(clang::SourceRange Range,
           clang::SourceLocation EndifLoc) {
    if (!_first.Disabled())  _first.SourceRangeSkipped(Range, EndifLoc);
    _rest.SourceRangeSkipped(Range, EndifLoc);
  }

  void If(clang::SourceLocation Loc,
           clang::SourceRange ConditionalRange,
           clang::PPCallbacks::ConditionValueKind ConditionalValue) {
    if (!_first.Disabled()) _first.If(Loc, ConditionalRange, ConditionalValue);
    _rest.If(Loc, ConditionalRange, ConditionalValue);
  }

  void Elif(clang::SourceLocation Loc,
            clang::SourceRange ConditionalRange,
            clang::PPCallbacks::ConditionValueKind ConditionalValue,
            clang::SourceLocation IfLoc) {
    if (!_first.Disabled()) _first.Elif(Loc, ConditionalRange, ConditionalValue, IfLoc);
    _rest.Elif(Loc, ConditionalRange, ConditionalValue, IfLoc);
  }

  void Ifdef(clang::SourceLocation Loc,
           const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD) {
    if (!_first.Disabled()) _first.Ifdef(Loc, MacroNameTok, MD);
    _rest.Ifdef(Loc, MacroNameTok, MD);
  }

  void Ifndef(clang::SourceLocation Loc,
           const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD) {
    if (!_first.Disabled()) _first.Ifndef(Loc, MacroNameTok, MD);
    _rest.Ifndef(Loc, MacroNameTok, MD);
  }

  void Else(clang::SourceLocation Loc,
           clang::SourceLocation IfLoc) {
    if (!_first.Disabled()) _first.Else(Loc, IfLoc);
    _rest.Else(Loc, IfLoc);
  }

  void Endif(clang::SourceLocation Loc,
           clang::SourceLocation IfLoc) {
    if (!_first.Disabled()) _first.Endif(Loc, IfLoc);
    _rest.Endif(Loc, IfLoc);
  }

};  // PPListHandler with variadic template parameters as handlers

// class PPListHandler with single template parameters as handlers
template<typename _First>
class PPListHandler<_First> {
private:
  _First _first;

public:
  PPListHandler() {}

  // Handle PPCallbacks
  void FileChanged(clang::SourceLocation Loc,
           clang::PPCallbacks::FileChangeReason Reason,
           clang::SrcMgr::CharacteristicKind FileType,
           clang::FileID PrevFID) {
    if (!_first.Disabled()) _first.FileChanged(Loc, Reason, FileType, PrevFID);
  }

  void InclusionDirective(clang::SourceLocation DirectiveLoc,
           const clang::Token &IncludeToken,
           llvm::StringRef IncludedFilename,
           bool IsAngled,
           clang::CharSourceRange FilenameRange,
           const clang::FileEntry *IncludedFile,
           llvm::StringRef SearchPath,
           llvm::StringRef RelativePath,
           const clang::Module *Imported,
           clang::SrcMgr::CharacteristicKind FileType) {
    if (!_first.Disabled())
      _first.InclusionDirective(DirectiveLoc, IncludeToken, IncludedFilename,
                              IsAngled, FilenameRange, IncludedFile,
                              SearchPath, RelativePath, Imported, FileType);
  }

  void EndOfMainFile() {
    if (!_first.Disabled()) _first.EndOfMainFile();
  }

  void Ident(clang::SourceLocation Loc,
           llvm::StringRef Str) {
    if (!_first.Disabled()) _first.Ident(Loc, Str);
  }

  void PragmaDirective(clang::SourceLocation Loc,
           clang::PragmaIntroducerKind Introducer) {
    if (!_first.Disabled()) _first.PragmaDirective(Loc, Introducer);
  }

  void PragmaComment(clang::SourceLocation Loc,
           const clang::IdentifierInfo *Kind,
           llvm::StringRef Str) {
    if (!_first.Disabled()) _first.PragmaComment(Loc, Kind, Str);
  }

  void PragmaDetectMismatch(clang::SourceLocation Loc,
           llvm::StringRef Name,
           llvm::StringRef Value) {
    if (!_first.Disabled()) _first.PragmaDetectMismatch(Loc, Name, Value);
  }

  void PragmaDebug(clang::SourceLocation Loc,
           llvm::StringRef DebugType) {
    if (!_first.Disabled()) _first.PragmaDebug(Loc, DebugType);
  }

  void PragmaMessage(clang::SourceLocation Loc,
           llvm::StringRef Namespace,
           clang::PPCallbacks::PragmaMessageKind Kind,
           llvm::StringRef Str) {
    if (!_first.Disabled()) _first.PragmaMessage(Loc, Namespace, Kind, Str);
  }

  void PragmaDiagnosticPush(clang::SourceLocation Loc,
           llvm::StringRef Namespace) {
    if (!_first.Disabled()) _first.PragmaDiagnosticPush(Loc, Namespace);
  }

  void PragmaDiagnosticPop(clang::SourceLocation Loc,
           llvm::StringRef Namespace) {
    if (!_first.Disabled()) _first.PragmaDiagnosticPop(Loc, Namespace);
  }

  void PragmaDiagnostic(clang::SourceLocation Loc,
           llvm::StringRef Namespace,
           clang::diag::Severity Mapping,
           llvm::StringRef Str) {
    if (!_first.Disabled()) _first.PragmaDiagnostic(Loc, Namespace, Mapping, Str);
  }

  void HasInclude(clang::SourceLocation Loc,
           llvm::StringRef FileName,
           bool IsAngled,
           llvm::Optional<clang::FileEntryRef> File,
           clang::SrcMgr::CharacteristicKind FileType) {
    if (!_first.Disabled()) _first.HasInclude(Loc, FileName, IsAngled, File, FileType);
  }

  void PragmaOpenCLExtension(clang::SourceLocation NameLoc,
           const clang::IdentifierInfo *Name,
           clang::SourceLocation StateLoc,
           unsigned State) {
    if (!_first.Disabled()) _first.PragmaOpenCLExtension(NameLoc, Name, StateLoc, State);
  }

  void PragmaWarning(clang::SourceLocation Loc,
           llvm::StringRef WarningSpec,
           llvm::ArrayRef<int> Ids) {
    if (!_first.Disabled()) _first.PragmaWarning(Loc, WarningSpec, Ids);
  }

  void PragmaWarningPush(clang::SourceLocation Loc,
           int Level) {
    if (!_first.Disabled()) _first.PragmaWarningPush(Loc, Level);
  }

  void PragmaWarningPop(clang::SourceLocation Loc) {
    if (!_first.Disabled()) _first.PragmaWarningPop(Loc);
  }

  void PragmaAssumeNonNullBegin(clang::SourceLocation Loc) {
    if (!_first.Disabled()) _first.PragmaAssumeNonNullBegin(Loc);
  }

  void PragmaAssumeNonNullEnd(clang::SourceLocation Loc) {
    if (!_first.Disabled()) _first.PragmaAssumeNonNullEnd(Loc);
  }

  void MacroExpands(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD,
           clang::SourceRange Range,
           const clang::MacroArgs *Args) {
    if (!_first.Disabled()) _first.MacroExpands(MacroNameTok, MD, Range, Args);
  }

  void MacroDefined(const clang::Token &MacroNameTok,
           const clang::MacroDirective *MD) {
    if (!_first.Disabled()) _first.MacroDefined(MacroNameTok, MD);
  }

  void MacroUndefined(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD,
           const clang::MacroDirective *Undef) {
    if (!_first.Disabled()) _first.MacroUndefined(MacroNameTok, MD, Undef);
  }

  void Defined(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD,
           clang::SourceRange Range) {
    if (!_first.Disabled()) _first.Defined(MacroNameTok, MD, Range);
  }

  void SourceRangeSkipped(clang::SourceRange Range,
           clang::SourceLocation EndifLoc) {
    if (!_first.Disabled()) _first.SourceRangeSkipped(Range, EndifLoc);
  }

  void If(clang::SourceLocation Loc,
           clang::SourceRange ConditionalRange,
           clang::PPCallbacks::ConditionValueKind ConditionalValue) {
    if (!_first.Disabled()) _first.If(Loc, ConditionalRange, ConditionalValue);
  }

  void Elif(clang::SourceLocation Loc,
            clang::SourceRange ConditionalRange,
            clang::PPCallbacks::ConditionValueKind ConditionalValue,
            clang::SourceLocation IfLoc) {
    if (!_first.Disabled()) _first.Elif(Loc, ConditionalRange, ConditionalValue, IfLoc);
  }

  void Ifdef(clang::SourceLocation Loc,
           const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD) {
    if (!_first.Disabled()) _first.Ifdef(Loc, MacroNameTok, MD);
  }

  void Ifndef(clang::SourceLocation Loc,
           const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD) {
    if (!_first.Disabled()) _first.Ifndef(Loc, MacroNameTok, MD);
  }

  void Else(clang::SourceLocation Loc,
           clang::SourceLocation IfLoc) {
    if (!_first.Disabled()) _first.Else(Loc, IfLoc);
  }

  void Endif(clang::SourceLocation Loc,
           clang::SourceLocation IfLoc) {
    if (!_first.Disabled()) _first.Endif(Loc, IfLoc);
  }

};  // PPListHandler with single template parameter as handler

}  // namespace xsca

#endif  // PP_LIST_CALLBACK_INCLUDED
