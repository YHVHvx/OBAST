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
#include "utils.h"

extern clang::ast_matchers::DeclarationMatcher funcDeclMatcher;
extern clang::ast_matchers::StatementMatcher funcCallMatcher;


class FuncDeclCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
private:
  clang::tooling::Replacements* replace;
  const clang::SourceManager* srcMgr; 
public :
  FuncDeclCallback(clang::tooling::Replacements* replace):replace(replace){};
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &); 
};

class FuncCallCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
private:
  clang::tooling::Replacements* replace;
  const clang::SourceManager* srcMgr; 
public :
  FuncCallCallback(clang::tooling::Replacements* replace):replace(replace){};
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &);
};
