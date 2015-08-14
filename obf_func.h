#ifndef __OBF_FUNC_H__
#define __OBF_FUNC_H__
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

class MyObfVisitor : public RecursiveASTVisitor<MyObfVisitor> {
private:
    ASTContext *astContext; // used for getting additional AST info

public:
    explicit MyObfVisitor(CompilerInstance *CI) 
      : astContext(&(CI->getASTContext())) // initialize private members
    {
        rewriter.setSourceMgr(astContext->getSourceManager(), astContext->getLangOpts());
    }
    virtual bool VisitFunctionDecl(FunctionDecl *);
    virtual bool VisitCallExpr(CallExpr *);
};

class MyObfASTConsumer : public ASTConsumer {
private:
    MyObfVisitor *visitor; // doesn't have to be private

public:
    // override the constructor in order to pass CI
    explicit MyObfASTConsumer(CompilerInstance *CI)
        : visitor(new MyObfVisitor(CI)) // initialize the visitor
    { }
    // override this to call our ExampleVisitor on the entire source file
    virtual void HandleTranslationUnit(ASTContext &);

    // override this to call our ExampleVisitor on each top-level Decl
    virtual bool HandleTopLevelDecl(DeclGroupRef);
};



class MyObfFrontendAction : public ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(CompilerInstance &, StringRef);
};

#endif
