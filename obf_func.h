#ifndef __OBF_FUNC_H__
#define __OBF_FUNC_H__

#include "utils.h"

class MyObfVisitor : public RecursiveASTVisitor<MyObfVisitor> {
private:
    ASTContext *astContext; // used for getting additional AST info

public:
    explicit MyObfVisitor(CompilerInstance *); 
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
