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

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;
using namespace clang::ast_matchers::internal;

extern Rewriter rewriter;
extern string projPath;
extern map<string, string> funcMap;

int SaveFuncMap();
int LoadFuncMap();
bool HasBeenObfuscated(string);

struct genes4_st{
    uint8_t genes[4];
};

struct genes80_st{
    uint8_t genes[80];
};

#define genes4_t struct genes4_st
#define genes80_t struct genes80_st

genes80_t BitstoGenes(uint8_t input[20]);

#endif
