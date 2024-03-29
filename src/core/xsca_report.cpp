/*
   Copyright (C) 2021 Xcalibyte (Shenzhen) Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

//
// ====================================================================
// xsca_report.cpp
// ====================================================================
//
// implementation for XcalReport to write issue to vtxt file/stdout
//

// vtxt format:
// {"V", A, 0.7.1,0000000000000000000000000000000000000000000000000000000000000000}
// [
//   {
//     "fid": 1,
//     "path": "filename.c"
//   }
// ]
// [A10],[key],[filename][fid:line],[RBC],[D],[G4_2_1_7],[1,0,0],[var/typename],#funcname#,[fid:line:path,fid:line:path,...]
//
// stdout format:
// filename.c:line:column: GJB5369:4.1.1.1 msg
//


#include "xsca_report.h"
#include "xsca_checker_manager.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManagerInternals.h"

namespace xsca {
llvm::cl::opt<std::string> XcalReport::_magic_opt(
    "magic", llvm::cl::init("A"), llvm::cl::Hidden,
    llvm::cl::desc("magic number of vtxt"));

// XcalReport::GetFileIdFromLineTable
// Get file index used in vtxt file table from file name in LineTable
unsigned
XcalReport::GetFileIdFromLineTable(const char *fname) {
  llvm::StringRef strr(fname);
  clang::LineTableInfo &line_table = const_cast<clang::SourceManager *>(_source_mgr)->getLineTable();
  return _line_table_offset + line_table.getLineTableFilenameID(fname);
}

// XcalReport::WinPathToLinux
// convert path from windows form to linux form
std::string XcalReport::WinPathToLinux(const std::string &win_path) {
  std::string path;
  std::size_t idx = 0;

  // remove the disk letter
//  if (win_path.length() >= 2) {
//    if (std::isalpha(win_path[0]) && (win_path[1] == ':')) {
//      idx = 2;
//    }
//  }

  while (idx < win_path.length()) {
    if (win_path[idx] == '\\') {
      if (path.back() != '/')
      path += '/';
    } else {
      path += win_path[idx];
    }
    idx++;
  }
  return path;
}

// XcalReport::PrintVtxtFileList
// Print file list to vtxt file to map file id to file name
void
XcalReport::PrintVtxtFileList() {
  DBG_ASSERT(_vtxt_file, "vtxt file not initialized");
  DBG_ASSERT(_source_mgr, "source manager is null");

  fprintf(_vtxt_file, "{\"V\", %s, 0.7.2, 0000000000000000000000000000000000000000000000000000000000000000}\n[\n",
          _magic_opt.getValue().c_str());

  clang::SourceManager::fileinfo_iterator end = _source_mgr->fileinfo_end();
  bool append_comma = false;
  for (clang::SourceManager::fileinfo_iterator it = _source_mgr->fileinfo_begin();
       it != end; ++it) {

    // ignore its real filename
    auto real_name = it->first->tryGetRealPathName();
    if (!real_name.empty() && (real_name.endswith(".i") || real_name.endswith(".ii"))) continue;

    // output comma if necessary
    if (append_comma) {
      fprintf(_vtxt_file, ",\n");
    } else {
      append_comma = true;
    }

    std::string path = WinPathToLinux(it->first->getName().rtrim(".i").rtrim(".ii").str());

    // output file entry
    fprintf(_vtxt_file, "  {\n    \"fid\" : %d,\n    \"path\" : \"%s\"\n  }",
            it->first->getUID() + 1,
            path.c_str());
    ++_line_table_offset;
  }

  clang::LineTableInfo &line_table = const_cast<clang::SourceManager *>(_source_mgr)->getLineTable();
  for (unsigned int i = 0; i < line_table.getNumFilenames(); ++i) {
    llvm::StringRef fname = line_table.getFilename(i);
    // ignore special names
    if (fname[0] == '<')
      continue;
    // output comma if necessary
    if (append_comma) {
      fprintf(_vtxt_file, ",\n");
    } else {
      append_comma = true;
    }

    std::string path = WinPathToLinux(std::string(fname.data()));

    // output file entry
    fprintf(_vtxt_file, "  {\n    \"fid\" : %d,\n    \"path\" : \"%s\"\n  }",
            i + _line_table_offset, path.c_str());
  }

  fprintf(_vtxt_file, "\n]\n");
}

// XcalReport::PrintVtxtIssue
// Print issue to vtxt file for xcalscan integration
void
XcalReport::PrintVtxtIssue(const XcalIssue *issue) {
  // omit this issue if need ignore
  if (issue->IsIgnore()) return;

  DBG_ASSERT(_vtxt_file, "vtxt file not initialized");
  DBG_ASSERT(_source_mgr, "source manager is null");

  clang::SourceLocation loc = issue->GetLocation();
  clang::PresumedLoc ploc = _source_mgr->getPresumedLoc(loc);
  const clang::FileEntry *fe = _source_mgr->getFileEntryForID(ploc.getFileID());
  unsigned fid = fe ? fe->getUID() + 1 : GetFileIdFromLineTable(ploc.getFilename());

  auto getLastToken = [](std::string s, const std::string &delimiter) -> std::string {
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
      token = s.substr(0, pos);
      s.erase(0, pos + delimiter.length());
    }
    return s;
  };

  char key[1024];
  std::string ploc_filename = WinPathToLinux(ploc.getFilename());
  std::string short_filename = getLastToken(ploc_filename, "/");

  snprintf(key, sizeof(key), "%s@%s@%s:%d",
           issue->DeclName(), issue->RuleName(),
           short_filename.c_str(), ploc.getLine());

  std::string output_std;
  std::string std_name = std::string(issue->StdName());
  if (std_name.find("MISRA") != std::string::npos) {
    output_std = "MSR";
  } else if (std_name.find("GJB") != std::string::npos) {
    output_std = "GJB";
  } else if (std_name.find("AUTOSAR") != std::string::npos) {
    output_std = "ATS";
  }

  //                     1    2    3    4  5          6                  7    8
  fprintf(_vtxt_file, "[%s],[%s],[%s],[%d:%d],[SML],[%s],[RBC],[1,0,0],[%s],[%s],",
          _magic_opt.getValue().c_str(),  // 1
          key,                            // 2
          short_filename.c_str(),         // 3
          fid, ploc.getLine(),            // 4, 5
          (issue->IsMaybe() ? "M" : "D"), // 6
          output_std.c_str(),             // 7
          issue->RuleName());             // 8
  fprintf(_vtxt_file, "[%s],[%s],[", issue->DeclName(), issue->FuncName());

  std::vector<XcalPathInfo>::const_iterator end = issue->PathInfo().end();
  bool append_comma = false;
  for (std::vector<XcalPathInfo>::const_iterator it = issue->PathInfo().begin();
       it != end; ++it) {
    // output comma if necessary
    if (append_comma) {
      fprintf(_vtxt_file, ",");
    } else {
      append_comma = true;
    }
    // output path
    ploc = _source_mgr->getPresumedLoc(it->Start());
    if (ploc.isValid()) {
      fe = _source_mgr->getFileEntryForID(ploc.getFileID());
      fid = fe ? fe->getUID() + 1 : GetFileIdFromLineTable(ploc.getFilename());
      fprintf(_vtxt_file, "%d:%d:%d:%d",
              fid, ploc.getLine(), ploc.getColumn(), it->Kind());
    } else {
      fprintf(_vtxt_file, "%d:%d:%d:%d", 0, 0, 0, 0);
    }
  }

  fprintf(_vtxt_file, "]\n");
}

// XcalReport::PrintStdoutIssue
// Print issue to stdout for debug purpose
void
XcalReport::PrintStdoutIssue(const XcalIssue *issue) {
  // omit this issue if need ignore
  if (issue->IsIgnore()) return;

  DBG_ASSERT(_source_mgr, "source manager is null");

  clang::SourceLocation loc = issue->GetLocation();
  clang::PresumedLoc ploc = _source_mgr->getPresumedLoc(loc);
  printf("%s:%d:%d %s:%s",
         ploc.getFilename(), ploc.getLine(), ploc.getColumn(),
         issue->StdName(), issue->RuleName());
  const char *refmsg = issue->RefMessage();
  if (refmsg) {
    printf(": %s", refmsg);
  }
  printf("\n");
}

// XcalReport::IsStdLibrary
// Check if source is std library
bool XcalReport::IsStdLibrary(clang::SourceLocation location) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  if (location.isInvalid()) return false;
  if (src_mgr->isInSystemHeader(location) ||
      src_mgr->isInSystemMacro(location) ||
      src_mgr->isWrittenInBuiltinFile(location)) {
    return true;
  }
  return false;
}

bool XcalReport::IsXvsaFIle(clang::SourceLocation location) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  if (location.isInvalid()) return false;
  clang::PresumedLoc ploc = src_mgr->getPresumedLoc(location);
  llvm::StringRef ploc_filename = ploc.getFilename();
  return ploc_filename.contains("__xvsa");
}

XcalIssue *XcalReport::ReportIssue(const char *std, const char *rule, const clang::Stmt *stmt) {
  auto issue = std::make_unique<XcalIssue>(std, rule, stmt);

  if (auto func = XcalCheckerManager::GetCurrentFunction()) {
    issue->SetFuncName(func->getNameAsString());
  }

  // ignore this issue if it is std source
  if (IsStdLibrary(stmt->getBeginLoc()) || IsXvsaFIle(stmt->getBeginLoc()))
    issue->SetIgnore(true);

  XcalIssue *issue_ptr = issue.get();
  if (issue_ptr->GetLocation().isInvalid()) {
    issue->SetIgnore(true);
  }
  _issue_vec.push_back(std::move(issue));
  return issue_ptr;
}

XcalIssue *XcalReport::ReportIssue(const char *std, const char *rule, const clang::Decl *decl) {
  auto issue = std::make_unique<XcalIssue>(std, rule, decl);

  if (auto func = XcalCheckerManager::GetCurrentFunction()) {
    issue->SetFuncName(func->getNameAsString());
  }

  // ignore this issue if it is std source
  if (IsStdLibrary(decl->getLocation()) || IsXvsaFIle(decl->getLocation()))
    issue->SetIgnore(true);

  XcalIssue *issue_ptr = issue.get();
  _issue_vec.push_back(std::move(issue));
  return issue_ptr;
}

void XcalPDB::WriteSymbolInfo(const char* name, const clang::Decl::Kind kind, bool defined,
                              const clang::SourceLocation SL, const char* type,
                              const clang::StorageClass SC, const clang::Linkage L) {
  if (_pdb_file == NULL)
    return;

  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto SpellingLoc = src_mgr->getSpellingLoc(SL);
  clang::PresumedLoc PLoc = src_mgr->getPresumedLoc(SpellingLoc);
  uint16_t line = 0;
  uint16_t col = 0;
  const char* filename = nullptr;
  if (PLoc.isInvalid())
    return;

  line = PLoc.getLine();
  col = PLoc.getColumn();
  filename = PLoc.getFilename();
  auto func = XcalCheckerManager::GetCurrentFunction();
  const char* funcname = "";
  if (func != NULL && func->getIdentifier() != NULL)
    funcname = func->getName().data();
  fprintf(_pdb_file, "%s;;%d;;%d;;%s;;%d:%d;;%s;;%s;;%d;;%d\n",
          name, kind, defined, filename ? filename : "",
          line, col, funcname, type ? type : "", SC, L);
}

}  // namespace xsca
