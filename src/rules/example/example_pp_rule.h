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
// example_pp_rule.h
// ====================================================================
//
// this is an example to describe how to write a preprocessor callbacks
// based checker for lexer related rules.
//

#include "xsca_defs.h"
#include "pp_null_handler.h"
#include "pp_list_handler.h"

// CHANGE HERE: add all preprocess rules into macro below
#define ALL_PP_RULES(R) \
  R(CheckPPExample, "CheckPPExample")

// force class/types defined in xcsa::example
namespace xsca {
namespace example {

// CHANGE HERE: add include file below
#include "check_pp_example.inc"

#define GetClass(CLASS, ...) CLASS
using ExamplePPHandler = PPListHandler<ALL_PP_RULES(GetClass)>;

}  // namespace example
}  // namespace xsca
