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
// SJT11682_decl_rule.h
// ====================================================================
//
// implement Decl related rules for SJT11682
//

#include "scope_manager.h"
#include "decl_null_handler.h"
//#include <clang/AST/Decl.h>
//#include <vector>

namespace xsca {
namespace rule {
class SJT11682DeclRule : public DeclNullHandler {
public:
  ~SJT11682DeclRule() {}

private:

public:

  void VisitFunction(const clang::FunctionDecl *decl) {
  }
  void Finalize() {
  }


}; // SJT11682DeclRule
}  // rule
}  // xsca
