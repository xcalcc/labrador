/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_link.cpp
// ====================================================================
//
// call external functions defined in rules to make sure rule files are
// correct linked
//

#define ATTRIBUTE_UNUSED __attribute__((__unused__))

// make sure example rule linked to executable
extern int __ExampleRuleLinked__;
//static int ATTRIBUTE_UNUSED ExampleRuleLinked = __ExampleRuleLinked__;
int ExampleRuleLinked = __ExampleRuleLinked__;
