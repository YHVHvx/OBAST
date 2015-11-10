/*
*Author:china.xuhui@gmail.com
*Description:this is the entry file of the OBOT project
*/
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Refactoring.h"
#include <string>
#include <sys/time.h>
#include <hash_map>
#include "case/nvo_sha1.h"
#include "case/utils.h"
#include "obfuscator/utils.h"
#include "obfuscator/funcobf.h"
#include "obfuscator/cfgobf.h"
#include "obfuscator/varobf.h"

using namespace std;
using namespace llvm;;
using namespace clang::tooling;
using namespace clang::ast_matchers;

string projPath;
map<string, string> funcMap;
map<string, string> varMap;

//Command Defination
static llvm::cl::OptionCategory myToolCategory("NVO Source to Source Transformation Tool");
const char* optDesc = "Obfuscation Type:" 
		 " --0:Researved for new feature testing"
		 " --1:General Obfuscation"
		 " --2:General N-version Obfuscation"
        	 " --9:Specific N-version Ofbuscation for SHA1 algorithm";
static cl::opt<int> obfType("O",
           cl::desc(optDesc),
	   cl::value_desc("0,1,2,9"), 
	   cl::cat(myToolCategory));
static cl::extrahelp MoreHelp("\n Example Command: nvocomp ~/gadget/sha1.c -O=1 --\n");

//The Implementation of NVO for SHA1 Algorithm
int NVO_Sha1Alg(RefactoringTool &tool){
    //To rewrite the value of k and function of f in SHA1 algorithm
    MatchFinder genFinder, frFinder, fwFinder;
    KgenesCallback kgenesCallback(&tool.getReplacements());
    FgenesCallback fgenesCallback(&tool.getReplacements());
    genFinder.addMatcher(kgenesMatcher, &kgenesCallback);
    genFinder.addMatcher(fgenesMatcher, &fgenesCallback);
    int result = tool.runAndSave(newFrontendActionFactory(&genFinder).get());
    SaveGenes();

    //Obtain the codes of SHA1 sub function bodies
    FuncReadCallback lo0ReadCallback(&tool.getReplacements());
    FuncReadCallback lo1ReadCallback(&tool.getReplacements());
    FuncReadCallback lo2ReadCallback(&tool.getReplacements());
    FuncReadCallback lo3ReadCallback(&tool.getReplacements());
    frFinder.addMatcher(lo0FuncMatcher, &lo0ReadCallback);
    frFinder.addMatcher(lo1FuncMatcher, &lo1ReadCallback);
    frFinder.addMatcher(lo2FuncMatcher, &lo2ReadCallback);
    frFinder.addMatcher(lo3FuncMatcher, &lo3ReadCallback);
    result = tool.runAndSave(newFrontendActionFactory(&frFinder).get());

    //Rewrite the codes
    FuncRewriteCallback lo0RewriteCallback(&tool.getReplacements());
    FuncRewriteCallback lo1RewriteCallback(&tool.getReplacements());
    FuncRewriteCallback lo2RewriteCallback(&tool.getReplacements());
    FuncRewriteCallback lo3RewriteCallback(&tool.getReplacements());
    fwFinder.addMatcher(lo0FuncMatcher, &lo0RewriteCallback);
    fwFinder.addMatcher(lo1FuncMatcher, &lo1RewriteCallback);
    fwFinder.addMatcher(lo2FuncMatcher, &lo2RewriteCallback);
    fwFinder.addMatcher(lo3FuncMatcher, &lo3RewriteCallback);
    result = tool.runAndSave(newFrontendActionFactory(&fwFinder).get());
    SaveFuncOrder();

    return result;
}

int Obf_FuncName(RefactoringTool &tool){
    int result = 0;
    
    FuncDeclCallback funcDeclCbk(&tool.getReplacements());
    FuncCallCallback funcCallCbk(&tool.getReplacements());
    MatchFinder finder;
    finder.addMatcher(funcDeclMatcher, &funcDeclCbk);
    finder.addMatcher(funcCallMatcher, &funcCallCbk);

    tool.runAndSave(newFrontendActionFactory(&finder).get());
    SaveFuncMap();

    return result;
}

int Obf_VarName(RefactoringTool &tool){
    int result = 0;
    
    VarDeclCallback varDeclCbk(&tool.getReplacements());
    VarRefCallback varRefCbk(&tool.getReplacements());
    MatchFinder finder;
    finder.addMatcher(varDeclMatcher, &varDeclCbk);
    finder.addMatcher(varRefMatcher, &varRefCbk);

    tool.runAndSave(newFrontendActionFactory(&finder).get());
//    SaveVarMap();
    return result;
}

int Obf_CFG(RefactoringTool &tool){
    int result = 0;
    
    CfgCallback cfgCbk(&tool.getReplacements());
    MatchFinder finder;
    finder.addMatcher(cfgMatcher, &cfgCbk);

    result = tool.runAndSave(newFrontendActionFactory(&finder).get());
    return result;
}

int main(int argc, const char **argv) {

    int result = 0;

    CommonOptionsParser op(argc, argv, myToolCategory); 
    vector<string> projPaths = op.getSourcePathList();
    RefactoringTool tool(op.getCompilations(), projPaths);

    projPath = projPaths.at(0).substr(0,projPaths.at(0).rfind("/"));

    struct timeval t;
    gettimeofday(&t, NULL);
    srand (1000000*t.tv_sec+t.tv_usec);

    if(obfType == 0){
    	outs() << "Reserved for Developers\n";
        InitCodeSet();
        Obf_CFG(tool);
    }

    if(obfType == 1){
    	outs() << "Obfuscation Type: General Obfuscation\n";
        Obf_FuncName(tool);
        Obf_VarName(tool);
    }

    if(obfType == 2){
    	outs() << "Obfuscation Type: General N-version Obfuscation\n";
    }

    if(obfType == 9){
    	outs() << "Obfuscation Type: Specific N-version Obfuscation for the SHA1 Algorithm\n";
        NVO_Sha1Alg(tool);
    	outs() << "Replacements collected by the tool:\n";
    	//for (auto &r : tool.getReplacements()) {
	    //outs() << r.toString() << "\n";
    	//}
    }

    return result;
}
