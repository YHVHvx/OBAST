#ifndef __OBF_UTILS_H__
#define __OBF_UTILS_H__

#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Lex/Lexer.h"
#include <string>
#include <fstream>
#include <sstream>
#include <time.h>
#include <hash_map>

extern std::string projPath;
extern std::map<std::string, std::string> funcMap;

int SaveFuncMap();
bool HasBeenObfuscated(std::string);
bool IsDeclStmt(const clang::Stmt*);
bool IsReturnStmt(const clang::Stmt*);
bool IsCXXForRangeStmt(const clang::VarDecl*);

#endif
