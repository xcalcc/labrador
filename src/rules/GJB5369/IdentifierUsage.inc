/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

/*
 * For all rules which checks the identifier usages.
 *   * 4.1.1.1 procedure name reused as other purpose is forbidden
 *
 * 
 * 1. What kind of information must be saved or tracked before this check is triggered?
 *  All the function name must be saved
 * 2. When is this check triggered and on what kind of node(s) on CLANG AST?
 *  The check is triiggered on the variableDecl node
 * 3. How to check the issue when the check is triggered on given CLANG AST?
 *  Check if the newly defined variable name is conflict with exist function names
 */

