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


extern clang::ast_matchers::DeclarationMatcher lo0FuncMatcher;
extern clang::ast_matchers::DeclarationMatcher lo1FuncMatcher;
extern clang::ast_matchers::DeclarationMatcher lo2FuncMatcher;
extern clang::ast_matchers::DeclarationMatcher lo3FuncMatcher;
extern clang::ast_matchers::DeclarationMatcher kgenesMatcher;
extern clang::ast_matchers::DeclarationMatcher fgenesMatcher;


class FBodys{

private: 
    std::string fBodys[4];
    uint8_t fCounter = 0;
    uint8_t forder[4];

public:
    uint8_t realOrder[4];
    FBodys(){}
    void Push(std::string);
    std::string RandPull();
    void GetRealOrder();
};


class FuncReadCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
private:
  clang::tooling::Replacements* replace;
  const clang::SourceManager* sm; 
public :
  FuncReadCallback(clang::tooling::Replacements* replace):replace(replace){};
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &); 
};

class FuncRewriteCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
private:
  clang::tooling::Replacements* replace;
  const clang::SourceManager* sm; 
public :
  FuncRewriteCallback(clang::tooling::Replacements* replace):replace(replace){};
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &);
};

class KgenesCallback : public  clang::ast_matchers::MatchFinder::MatchCallback {
private:
  clang::tooling::Replacements* replace;
public :
  KgenesCallback(clang::tooling::Replacements* replace):replace(replace){};
  virtual void run(const  clang::ast_matchers::MatchFinder::MatchResult &);
};

class FgenesCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
private:
  clang::tooling::Replacements* replace;
public :
  FgenesCallback(clang::tooling::Replacements* replace):replace(replace){};
  virtual void run(const  clang::ast_matchers::MatchFinder::MatchResult &);
};
