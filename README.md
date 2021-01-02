# Xcalibyte Standard Compliance Analyzer (XSCA)

## 1.  How to build
### 1.1 Get CLANG prebuilt libraryes
```
 $ mkdir clang-prebuilt && cd clang-prebuilt
 $ wget 'http://10.10.3.10:8888/clang-prebuilt/11.0.0/release.tar.gz'
 $ tar xf release.tar.gz
 $ export CLANG_HOME=`pwd`/release
```

### 1.2 Get XSCA source
 From gitlab:
```
 $ git clone http://git.xc5.io/git/xc5-sz/labrador.git
```
 From gerrit:
```
 $ git clone http://10.10.2.117:8282/labrador
```

### 1.3 Build xsca
```
 $ cd labrador
 $ ls
 README	doc	src	test
 $ ls src
 CMakeLists.txt	cc1_main.cpp	core	driver.cpp	include	rules	tests	xsca_gen.cpp	xsca_link.cpp
 $ ls src/rules
 CMakeLists.txt	GJB5369	GJB8114	MISRA	SJT11682	common	example
 $ mkdir build && cd build
 $ cmake ../src
 $ make
```
xsca will be built in build directory

### 1.4 Simple run
```
 $ ./xsca -cc1 -emit-llvm test.c
```

## 2. How to add new ruleset
### 2.1 Create subdirectory in rules for the new rule
Copy the whole example in src/rules directory to new ruleset name:
```
 $ cd src/rules
 $ cp -r example <NEW-RULESET-NAME>
```
rename files in <NEW-RULESET-NAME>.

### 2.2 Update CMakeLists.txt in new subdirectory
Modify <NEW-RULESET-NAME>/CMakeLists.txt with correct library and source file name to build the library for new rule

### 2.3 Update rules's CMakeLists.txt
Modify src/rules/CMakeLists.txt to include this new ruleset subdirectory

### 2.4 Update src's CMakeLists.txt
Modify src/CMakeLists.txt to add the new library to XSCA_LIBS

### 2.5 Make sure new library is linked correctly
Modify src/xsca_link.cpp to add new extern and static variable to make sure the library is correct linked and static objects are initialized correctly. Define the new variable in example_checker.cpp.
In xsca_link.cpp
```
extern int __NEW_RULESET_NAME__;   // defined in rules/example/example_checker.cpp
static ATTRIBUTE_UNUSED int New_Ruleset_Name = __NEW_RULESET_NAME__;
```
In example_checker.cpp
```
int __NEW_RULESET_NAME__;          // used in xsca_link.cpp
```

## 3. How to add new rule
### 3.1 Identify the type of the new rule. Possible type can be:
- A preprocessor(PP) rule
- An AST decl rule
- An AST stmt/expr rule
- An AST type rule

### 3.2 Implement the rule in seperated .inc file
- For PP rule, follow check_pp_example.inc to implement the new rule. New rule class must inherit from PPNullHandler which implements all clang preprocess callbacks but do nothing. New rule needs to overwrite callbacks interfaces to do corresponding check.
- For AST decl rule, follow check_decl_example.inc to inherit from DeclNullHandler. AST stmt/expr rule follows check_stmt_example.inc and inherit from StmtNullHandler. AST type rule follows check_type_example.inc and inherit from TypeNullHandler.
  
### 3.3 Add new rule to rule list file
- Modify example_pp_rule.h with new namespace. Add new PP rule class and name to ALL_PP_RULES macro and include the new PP rule inc file.
- Modify example_decl_rule.h with new namespace. Add new decl rule class and name to ALL_DECL_RULES macro and include the new decl rule inc file.
- Modify example_stmt_rule.h with new namespace. Add new stmt rule class and name to ALL_STMT_RULES macro and include the new stmt rule inc file.
- Modify example_type_rule.h with new namespace. Add new type rule class and name to ALL_TYPE_RULES macro and include the new type rule inc file.

### 3.4 Modify example_checker.cpp
- Modify example_checker.cpp with new namespace, using name, factory name.

## 4. XSCA internal
### 4.1 virtual function v.s. template
XSCA oonly uses virtual functions in the interface with clang, include ASTConsumer and PPCallbacks. Inside XSCA, all checkers are composed by template without virtual function for performance consideration.

### 4.2 Checker, CheckerManager, CheckerFactory and CheckerFactooryRegister
Each ruleset is implemented as a checker. The checker base class provides 2 virtual functions to return callbacks (ASTConsumer and PPCallbacks) to be invoked by clang Lexer and Sema.
CheckerManager is a singleton to manage all Checker instances and CheckerFactory instances. In XcalCheckerManager::Initialize(), all checker are created by CheckerFactory and added to clang's preprocess callbacks or AST consumers.
CheckerFactory provides interface to create the Checker instance. It's created and added to CheckerManager by CheckerFactooryRegister.
CheckerFactooryRegister is a static object to create CheckerFactory instance and add to CheckerManager.

XcalChecker is defined in include/xsca_checker.h. XcalCheckerManager, XcalCheckerFactory and XcalCheckerFactoryRegister is defined in include/xsca_checker_manager.h.

### 4.2 PPCallback, PPNullHandler and PPListHandler
PPCallback derives from clang::PPCallbacks which will be invoked by clang Lexer. When interfaces in PPCallbacks is called, the corresponding handler API will be invoked.
PPNullHandler implements all clang::PPCallbacks API but do nothing. This can be the default base class for all PP handlers.
PPListHandler composes multiple PP handles as a list so that they can be invoked by PPCallbacks one by one.

XcalPPCallback is defined in include/pp_callback.h. PPNullHandler is defined in pp_null_handler.h. PPListHandler is defined in pp_list_handler.h

### 4.3 TypeVisitor, TypeNullHandler and TypeListHandler
TypeVisitor implements a general Visit(clang::Type *type) method, which checks the type class and call individual Visit* method on given Type class. In iindividual Visit* method, corresponding handler method is called.
TypeNullHandler implements all Visit* method for types but do nothing. This can be the default base class for all Type checkers.
TypeListHandler composes multiple Type handlers as a list so that they can be invoked one by one.

XcalTypeVisitor is defined in include/type_visiotor.h. TypeNullHandler is defined in type_null_handler.h. TypeListHandler is defined in type_list_handler.h

### 4.4 StmtVisitor, StmtNullHandler and StmtListHandler.
Similar to TypeVisitor, TypeNullHandler, TypeListHandler. StmtVisitor will visit the Stmt node ans all it's children.

XcalStmtVisitor is defined in include/stmt_visiotor.h. StmtNullHandler is defined in stmt_null_handler.h. StmtListHandler is defined in stmt_list_handler.h

### 4.5 DeclVisitor, DeclNullHandler and DeclListHandler.
Similar to TypeVisitor, TypeNullHandler, TypeListHandler. DeclVisitor will visit the Decl node ans call TypeVisitor for TypeDecl and StmtVisitor for FunctionDecl.

XcalDeclVisitor is defined in include/decl_visiotor.h. DeclNullHandler is defined in decl_null_handler.h. DeclListHandler is defined in decl_list_handler.h

### 4.6 ASTConsumer
Interfaces in ASTConsumer is called by clang Sema and corresponding method in DeclVisitor is called so that all Handlers can be called.

### 4.7 Calling sequence:
  For preprocess:
```
  Clang Lex --> PPCallbacks --> PPHandler --> individual PP check
```
  For AST:
```
  Clang Sema --> ASTConsumer --> DeclVisitor --> DeclHandler --> individual Decl check
                                             --> (FunctionDecl) StmtVisitor --> StmtHandler --> individual Stmt check
                                             --> (TypeDecl) TypeVisitor --> TypeHandler --> individual Type check
```
