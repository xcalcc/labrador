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
// example_decl_rule.h
// ====================================================================
//
// this is an example to describe how to write a clang decl checker
//

#include "xsca_defs.h"
#include "decl_null_handler.h"
#include "decl_list_handler.h"

// add all decl rules into macro below
#define ALL_DECL_RULES(R) \
  R(CheckDeclExample, "CheckDeclExample")

// force below class/types defined in xsca::example
namespace xsca {
namespace example {
// decl based rules
#include "check_decl_example.inc"

#define GetClass(CLASS, ...) CLASS
using ExampleDeclHandler = DeclListHandler<ALL_DECL_RULES(GetClass)>;

}  // namespace example
}  // namespace xsca

