/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// pp_callback.h
// ====================================================================
//
// interface for clang preprocessor callback bridge
//

#ifndef PP_CALLBACK_INCLUDED
#define PP_CALLBACK_INCLUDED

#include "xsca_defs.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/FileManager.h"

namespace xsca {

// class XcalPPCallback
template<typename _Handler>
class XcalPPCallback : public clang::PPCallbacks {
private:
  _Handler                &_handler;
  clang::CompilerInstance *_ci;

public:
  XcalPPCallback(_Handler handler, clang::CompilerInstance *CI)
      : _handler(handler), _ci(CI) {}

  ~XcalPPCallback() {}

  clang::Preprocessor *getPreprocessor() const;

private:
  // Handle PPCallbacks
  void FileChanged(clang::SourceLocation Loc,
           clang::PPCallbacks::FileChangeReason Reason,
           clang::SrcMgr::CharacteristicKind FileType,
           clang::FileID PrevFID) override {
    _handler.FileChanged(Loc, Reason, FileType, PrevFID);
  }

  void InclusionDirective(clang::SourceLocation DirectiveLoc,
           const clang::Token &IncludeToken, llvm::StringRef IncludedFilename,
           bool IsAngled, clang::CharSourceRange FilenameRange,
           const clang::FileEntry *IncludedFile, llvm::StringRef SearchPath,
           llvm::StringRef RelativePath, const clang::Module *Imported,
           clang::SrcMgr::CharacteristicKind FileType) override {
    _handler.InclusionDirective(DirectiveLoc, IncludeToken, IncludedFilename,
        IsAngled, FilenameRange, IncludedFile, SearchPath, RelativePath,
        Imported, FileType);
  }

  void EndOfMainFile() override {
    _handler.EndOfMainFile();
  }

  void Ident(clang::SourceLocation Loc, llvm::StringRef Str) override {
    _handler.Ident(Loc, Str);
  }

  void PragmaDirective(clang::SourceLocation Loc,
           clang::PragmaIntroducerKind Introducer) override {
    _handler.PragmaDirective(Loc, Introducer);
  }

  void PragmaComment(clang::SourceLocation Loc,
           const clang::IdentifierInfo *Kind, llvm::StringRef Str) override {
    _handler.PragmaComment(Loc, Kind, Str);
  }

  void PragmaDetectMismatch(clang::SourceLocation Loc, llvm::StringRef Name,
           llvm::StringRef Value) override {
    _handler.PragmaDetectMismatch(Loc, Name, Value);
  }

  void PragmaDebug(clang::SourceLocation Loc,
           llvm::StringRef DebugType) override {
    _handler.PragmaDebug(Loc, DebugType);
  }

  void PragmaMessage(clang::SourceLocation Loc, llvm::StringRef Namespace,
           clang::PPCallbacks::PragmaMessageKind Kind,
           llvm::StringRef Str) override {
    _handler.PragmaMessage(Loc, Namespace, Kind, Str);
  }

  void PragmaDiagnosticPush(clang::SourceLocation Loc,
           llvm::StringRef Namespace) override {
    _handler.PragmaDiagnosticPush(Loc, Namespace);
  }

  void PragmaDiagnosticPop(clang::SourceLocation Loc,
           llvm::StringRef Namespace) override {
    _handler.PragmaDiagnosticPop(Loc, Namespace);
  }

  void PragmaDiagnostic(clang::SourceLocation Loc, llvm::StringRef Namespace,
           clang::diag::Severity Mapping, llvm::StringRef Str) override {
    _handler.PragmaDiagnostic(Loc, Namespace, Mapping, Str);
  }

  void HasInclude(clang::SourceLocation Loc, llvm::StringRef FileName,
           bool IsAngled, llvm::Optional<clang::FileEntryRef> File,
           clang::SrcMgr::CharacteristicKind FileType) override {
    _handler.HasInclude(Loc, FileName, IsAngled, File, FileType);
  }

  void PragmaOpenCLExtension(clang::SourceLocation NameLoc,
           const clang::IdentifierInfo *Name, clang::SourceLocation StateLoc,
           unsigned State) override {
    _handler.PragmaOpenCLExtension(NameLoc, Name, StateLoc, State);
  }

  void PragmaWarning(clang::SourceLocation Loc, llvm::StringRef WarningSpec,
           llvm::ArrayRef<int> Ids) override {
    _handler.PragmaWarning(Loc, WarningSpec, Ids);
  }

  void PragmaWarningPush(clang::SourceLocation Loc, int Level) override {
    _handler.PragmaWarningPush(Loc, Level);
  }

  void PragmaWarningPop(clang::SourceLocation Loc) override {
    _handler.PragmaWarningPop(Loc);
  }

  void PragmaAssumeNonNullBegin(clang::SourceLocation Loc) override {
    _handler.PragmaAssumeNonNullBegin(Loc);
  }

  void PragmaAssumeNonNullEnd(clang::SourceLocation Loc) override {
    _handler.PragmaAssumeNonNullEnd(Loc);
  }

  void MacroExpands(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD, clang::SourceRange Range,
           const clang::MacroArgs *Args) override {
    _handler.MacroExpands(MacroNameTok, MD, Range, Args);
  }

  void MacroDefined(const clang::Token &MacroNameTok,
           const clang::MacroDirective *MD) override {
    _handler.MacroDefined(MacroNameTok, MD);
  }

  void MacroUndefined(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD,
           const clang::MacroDirective *Undef) override {
    _handler.MacroUndefined(MacroNameTok, MD, Undef);
  }

  void Defined(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD,
           clang::SourceRange Range) override {
    _handler.Defined(MacroNameTok, MD, Range);
  }

  void SourceRangeSkipped(clang::SourceRange Range,
           clang::SourceLocation EndifLoc) override {
    _handler.SourceRangeSkipped(Range, EndifLoc);
  }

  void If(clang::SourceLocation Loc, clang::SourceRange ConditionalRange,
           clang::PPCallbacks::ConditionValueKind ConditionalValue) override {
    _handler.If(Loc, ConditionalRange, ConditionalValue);
  }

  void Elif(clang::SourceLocation Loc, clang::SourceRange ConditionalRange,
           clang::PPCallbacks::ConditionValueKind ConditionalValue,
           clang::SourceLocation IfLoc) override {
    _handler.Elif(Loc, ConditionalRange, ConditionalValue, IfLoc);
  }

  void Ifdef(clang::SourceLocation Loc, const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD) override {
    _handler.Ifdef(Loc, MacroNameTok, MD);
  }

  void Ifndef(clang::SourceLocation Loc, const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD) override {
    _handler.Ifndef(Loc, MacroNameTok, MD);
  }

  void Else(clang::SourceLocation Loc, clang::SourceLocation IfLoc) override {
    _handler.Else(Loc, IfLoc);
  }

  void Endif(clang::SourceLocation Loc, clang::SourceLocation IfLoc) override {
    _handler.Endif(Loc, IfLoc);
  }

};  // XcalPPCallback

}  // namespace xsca

#endif  // PP_CALLBACK_INCLUDED
