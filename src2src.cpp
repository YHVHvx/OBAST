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
#include "nvo_sha1.h"

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;
using namespace clang::ast_matchers::internal;

Rewriter rewriter;
map<string, string> funcMap;

class MyObfVisitor : public RecursiveASTVisitor<MyObfVisitor> {
private:
    ASTContext *astContext; // used for getting additional AST info

public:
    explicit MyObfVisitor(CompilerInstance *CI) 
      : astContext(&(CI->getASTContext())) // initialize private members
    {
        rewriter.setSourceMgr(astContext->getSourceManager(), astContext->getLangOpts());
    }

/*
    virtual bool VisitVarDecl(VarDecl *var) {
        string varName = var->getNameAsString();
	QualType QT = var->getType();
	return true;
    }
*/
   virtual bool VisitFunctionDecl(FunctionDecl *func) {
        string funcName = func->getNameInfo().getName().getAsString();
        char newName[8];
        int i;
        for (i = 0; i < 8; i++){
            newName[i] = (char) rand()%26 + 65;
        }
        funcMap[funcName] = newName;
        rewriter.ReplaceText(func->getLocation(), funcName.length(), newName);
        errs() << "** Rewrote function def: " << funcName << "\n";
        return true;
    }

    virtual bool VisitCallExpr(CallExpr *call) {
        string funcName = call->getCallee()->getType().getAsString();
        errs() <<funcName<<"\n";
        string newName = funcMap[funcName];
        if (newName != NULL){
            rewriter.ReplaceText(call->getLocStart(), funcName.length(), newName);
            errs() << "** Rewrote function call\n";
        }
        return true;
    }

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
    virtual void HandleTranslationUnit(ASTContext &Context) {
        visitor->TraverseDecl(Context.getTranslationUnitDecl());
    }


    // override this to call our ExampleVisitor on each top-level Decl
    virtual bool HandleTopLevelDecl(DeclGroupRef DG) {
        // a DeclGroupRef may have multiple Decls, so we iterate through each one
        for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; i++) {
            Decl *D = *i;    
            visitor->TraverseDecl(D); // recursively visit each AST node in Decl "D"
        }
        return true;
    }

};



class MyObfFrontendAction : public ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) {
        return std::unique_ptr<clang::ASTConsumer>(new MyObfASTConsumer(&CI)); // pass CI pointer to ASTConsumer
    }
};

int NVO_L0(RefactoringTool &tool){
    KgenesCallback kgenesCallback(&tool.getReplacements());
    FgenesCallback fgenesCallback(&tool.getReplacements());

    MatchFinder finder;

    finder.addMatcher(kgenesMatcher, &kgenesCallback);
    finder.addMatcher(fgenesMatcher, &fgenesCallback);
     
    int result = tool.runAndSave(newFrontendActionFactory(&finder).get());
    return result;
}

int NVO_L1(RefactoringTool &tool){
    FuncReadCallback lo0ReadCallback(&tool.getReplacements());
    FuncReadCallback lo1ReadCallback(&tool.getReplacements());
    FuncReadCallback lo2ReadCallback(&tool.getReplacements());
    FuncReadCallback lo3ReadCallback(&tool.getReplacements());
    FuncRewriteCallback lo0RewriteCallback(&tool.getReplacements());
    FuncRewriteCallback lo1RewriteCallback(&tool.getReplacements());
    FuncRewriteCallback lo2RewriteCallback(&tool.getReplacements());
    FuncRewriteCallback lo3RewriteCallback(&tool.getReplacements());

    MatchFinder finder;

    finder.addMatcher(lo0FuncMatcher, &lo0ReadCallback);
    finder.addMatcher(lo1FuncMatcher, &lo1ReadCallback);
    finder.addMatcher(lo2FuncMatcher, &lo2ReadCallback);
    finder.addMatcher(lo3FuncMatcher, &lo3ReadCallback);
 
    int result = tool.runAndSave(newFrontendActionFactory(&finder).get());

    MatchFinder finder2;

    finder2.addMatcher(lo0FuncMatcher, &lo0RewriteCallback);
    finder2.addMatcher(lo1FuncMatcher, &lo1RewriteCallback);
    finder2.addMatcher(lo2FuncMatcher, &lo2RewriteCallback);
    finder2.addMatcher(lo3FuncMatcher, &lo3RewriteCallback);

    result = tool.runAndSave(newFrontendActionFactory(&finder2).get());
    return result;
}
int NVO_L2(RefactoringTool &tool){
    int result = tool.run(newFrontendActionFactory<MyObfFrontendAction>().get());
    rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(errs());
    return result;
}

int main(int argc, const char **argv) {

    int result = 0;
    CommonOptionsParser op(argc, argv, myToolCategory); 
    RefactoringTool tool(op.getCompilations(), op.getSourcePathList());

    srand (time(NULL));

    //"L1 NVO"
    if(nvoLevel == 0){
    	outs() << "NVO Level:0:Pure seed:\n";
        NVO_L0(tool);
    	outs() << "Replacements collected by the tool:\n";
    	for (auto &r : tool.getReplacements()) {
	    outs() << r.toString() << "\n";
    	}
    }

    if(nvoLevel == 1){
    	outs() << "NVO Level:1:Function randomness:\n";
        NVO_L0(tool);
        NVO_L1(tool);
    	outs() << "Replacements collected by the tool:\n";
    	for (auto &r : tool.getReplacements()) {
	    outs() << r.toString() << "\n";
    	}
    }

    if(nvoLevel == 2){
    	outs() << "NVO Level:1:Function randomness:\n";
        NVO_L0(tool);
        NVO_L1(tool);
        NVO_L2(tool);
    	outs() << "Replacements collected by the tool:\n";
    	for (auto &r : tool.getReplacements()) {
	    outs() << r.toString() << "\n";
    	}
    }
    return result;
}
