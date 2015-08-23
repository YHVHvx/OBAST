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
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Lex/Lexer.h"
#include <string>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <hash_map>
#include "nvo_sha1.h"
#include "obf_func.h"
#include "nvo_bridge.h"

string projPath;

static llvm::cl::OptionCategory myToolCategory("NVO Source to Source Transformation Tool");
static cl::opt<int> nvoLevel("L", cl::desc("Specify nvo level"), cl::value_desc("0,1,2"), cl::cat(myToolCategory));
//static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::extrahelp MoreHelp("\n Example Command: nvocomp ~/gadget/sha1.c -L=1 --\n");

int NVO_Genes(RefactoringTool &tool){
    KgenesCallback kgenesCallback(&tool.getReplacements());
    FgenesCallback fgenesCallback(&tool.getReplacements());

    MatchFinder finder;

    finder.addMatcher(kgenesMatcher, &kgenesCallback);
    finder.addMatcher(fgenesMatcher, &fgenesCallback);
     
    int result = tool.runAndSave(newFrontendActionFactory(&finder).get());

    SaveGenes(kgenes);
    SaveGenes(fgenes);

    return result;
}

int NVO_Sha1SubFunc(RefactoringTool &tool){
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

int NVO_FuncName(RefactoringTool &tool){
    LoadFuncMap();
    int result = tool.run(newFrontendActionFactory<MyObfFrontendAction>().get());
    //SourceLocation srcLoc; 
    //result = rewriter.getEditBuffer(rewriter.getSourceMgr().getFileID(srcLoc)).write(errs());
    result = rewriter.overwriteChangedFiles();
    //outs()<<"NVO_L2 RESULT "<<result<<"\n";
    SaveFuncMap();

    return result;
}

int NVO_Bridge(RefactoringTool &tool){
    LoadFuncMap();
    int result = tool.run(newFrontendActionFactory<MyBridgeFrontendAction>().get());
    result = rewriter.overwriteChangedFiles();

    return result;
}

map<string, string> funcMap;
Rewriter rewriter;

int main(int argc, const char **argv) {

    int result = 0;
    CommonOptionsParser op(argc, argv, myToolCategory); 
    vector<string> projPaths = op.getSourcePathList();
    RefactoringTool tool(op.getCompilations(), projPaths);

    projPath = projPaths.at(0).substr(0,projPaths.at(0).rfind("/"));
/*
    FileID mainFid = rewriter.getSourceMgr().getMainFileID();
    SourceLocation mainLoc = rewriter.getSourceMgr().getLocForStartOfFile(mainFid);
    string mainFilePath = rewriter.getSourceMgr().getFilename(mainLoc);
    projPath = mainFilePath.substr(0,mainFilePath.rfind("/"));
*/
    outs() << "===========PROJECTPATH="<<projPath<<"=============\n";

    struct timeval t;
    gettimeofday(&t, NULL);
    srand (1000000*t.tv_sec+t.tv_usec);

    if(nvoLevel == 0){
    	outs() << "NVO Level:0:Pure seed:\n";
        NVO_Genes(tool);
    	outs() << "Replacements collected by the tool:\n";
    	for (auto &r : tool.getReplacements()) {
	    outs() << r.toString() << "\n";
    	}
    }

    if(nvoLevel == 1){
    	outs() << "NVO Level:1:Sha1 sub-function randomness:\n";
        NVO_Sha1SubFunc(tool);
    	outs() << "Replacements collected by the tool:\n";
    	for (auto &r : tool.getReplacements()) {
	    outs() << r.toString() << "\n";
    	}
    }

    if(nvoLevel == 2){
    	outs() << "NVO Level:2:Function name randomness:\n";
        NVO_FuncName(tool);
    }

    if(nvoLevel == 9){
    	outs() << "NVO Level:9:Bridge:\n";
        NVO_Bridge(tool);
    }
    return result;
}
