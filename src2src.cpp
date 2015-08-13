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
#include "obf_func.h"

static llvm::cl::OptionCategory myToolCategory("NVO Source to Source Transformation Tool");
static cl::opt<int> nvoLevel("L", cl::desc("Specify nvo level"), cl::value_desc("0,1,2"), cl::cat(myToolCategory));
//static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::extrahelp MoreHelp("\n Example Command: nvocomp ~/gadget/sha1.c -L=1 --\n");

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
