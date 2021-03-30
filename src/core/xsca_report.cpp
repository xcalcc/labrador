/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_report.cpp
// ====================================================================
//
// implementation for XcalReport to write issue to vtxt file/stdout
//

// vtxt format:
// {"V"}
// [
//   { "fid": 1, "path": "filename.c" }
// ]
// ["A10"],[key],[filename][fid:line],[SML],[D],[CSC],[0,0,0],[GJB5369],[4.1.1.3],[var/typename],[funcname],[fid:line:path,fid:line:path,...]
//
// stdout format:
// filename.c:line:column: GJB5369:4.1.1.1 msg
//

#include "xsca_report.h"
#include "xsca_checker_manager.h"
#include "clang/Basic/FileManager.h"

namespace xsca {

// XcalReport::PrintVtxtFileList
// Print file list to vtxt file to map file id to file name
void
XcalReport::PrintVtxtFileList()
{
  DBG_ASSERT(_vtxt_file, "vtxt file not initialized");
  DBG_ASSERT(_source_mgr, "source manager is null");

  fprintf(_vtxt_file, "{\"V\"}\n[\n");

  clang::SourceManager::fileinfo_iterator end = _source_mgr->fileinfo_end();
  bool append_comma = false;
  for (clang::SourceManager::fileinfo_iterator it = _source_mgr->fileinfo_begin();
       it != end; ++it) {
    // output comma if necessary
    if (append_comma) {
      fprintf(_vtxt_file, ",\n");
    }
    else {
      append_comma = true;
    }
    // output file entry
    fprintf(_vtxt_file, " { \"fid\":%d, \"path\":\"%s\" }",
                        it->first->getUID() + 1,
                        it->first->getName().str().c_str());
  }

  fprintf(_vtxt_file, "\n]\n");
}

// XcalReport::PrintVtxtIssue
// Print issue to vtxt file for xcalscan integration
void
XcalReport::PrintVtxtIssue(const XcalIssue *issue)
{
  DBG_ASSERT(_vtxt_file, "vtxt file not initializedd");
  DBG_ASSERT(_source_mgr, "source manager is null");

  clang::SourceLocation loc = issue->GetLocation();
  clang::PresumedLoc ploc = _source_mgr->getPresumedLoc(loc);
  const clang::FileEntry *fe = _source_mgr->getFileEntryForID(ploc.getFileID());

  char key[1024];
  snprintf(key, sizeof(key), "%s@%s@%s@%s:%d",
                             issue->DeclName(),
                             issue->RuleName(), issue->StdName(),
                             ploc.getFilename(), ploc.getLine());

  fprintf(_vtxt_file, "[\"A10\"],[%s],[%s],[%d:%d],[SML],[D],[CSC],[0,0,0],",
                      key, ploc.getFilename(),
                      fe->getUID() + 1, ploc.getLine());
  fprintf(_vtxt_file, "[%s],[%s],[%s],[],[",
                      issue->StdName(), issue->RuleName(), issue->DeclName());

  std::vector<XcalPathInfo>::const_iterator end = issue->PathInfo().end();
  bool append_comma = false;
  for (std::vector<XcalPathInfo>::const_iterator it = issue->PathInfo().begin();
       it != end; ++it) {
    // output comma if necessary
    if (append_comma) {
      fprintf(_vtxt_file, ",");
    }
    else {
      append_comma = true;
    }
    // output path
    ploc = _source_mgr->getPresumedLoc(it->Start());
    fe = _source_mgr->getFileEntryForID(ploc.getFileID());
    fprintf(_vtxt_file, "%d:%d:%d",
                         fe->getUID() + 1, ploc.getLine(), it->Kind());
  }

  fprintf(_vtxt_file, "]\n");
}

// XcalReport::PrintStdoutIssue
// Print issue to stdout for debug purpose
void
XcalReport::PrintStdoutIssue(const XcalIssue *issue)
{
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

}  // namespace xsca
