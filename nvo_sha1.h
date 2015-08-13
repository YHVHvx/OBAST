#pragma once

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

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;
using namespace clang::ast_matchers::internal;

extern DeclarationMatcher lo0FuncMatcher;
extern DeclarationMatcher lo1FuncMatcher;
extern DeclarationMatcher lo2FuncMatcher;
extern DeclarationMatcher lo3FuncMatcher;
extern DeclarationMatcher kgenesMatcher;
extern DeclarationMatcher fgenesMatcher;

class FBodys{
private: 
string fBodys[4];
int fCounter = 0;
public:
    FBodys(){}
    void Push(string);
    string RandPull();
};

class FuncReadCallback : public MatchFinder::MatchCallback {
private:
  Replacements* replace;
  const SourceManager* sm; 
public :
  FuncReadCallback(Replacements* replace):replace(replace){};
  virtual void run(const MatchFinder::MatchResult &); 
};

class FuncRewriteCallback : public MatchFinder::MatchCallback {
private:
  Replacements* replace;
  const SourceManager* sm; 
public :
  FuncRewriteCallback(Replacements* replace):replace(replace){};
  virtual void run(const MatchFinder::MatchResult &);
};

class KgenesCallback : public MatchFinder::MatchCallback {
private:
  Replacements* replace;
public :
  KgenesCallback(Replacements* replace):replace(replace){};
  virtual void run(const MatchFinder::MatchResult &);
};

class FgenesCallback : public MatchFinder::MatchCallback {
private:
  Replacements* replace;
public :
  FgenesCallback(Replacements* replace):replace(replace){};
  virtual void run(const MatchFinder::MatchResult &);
};
