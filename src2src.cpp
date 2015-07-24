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

static llvm::cl::OptionCategory myToolCategory("NVO Source to Source Transformation Tool");
static cl::opt<int> nvoLevel("L", cl::desc("Specify nvo level"), cl::value_desc("0,1,2"), cl::cat(myToolCategory));
//static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::extrahelp MoreHelp("\n Example Command: nvocomp ~/gadget/sha1.c -L=1 --\n");

DeclarationMatcher lo0FuncMatcher = functionDecl(hasName("nv_lo0")).bind("nv_lo");
DeclarationMatcher lo1FuncMatcher = functionDecl(hasName("nv_lo1")).bind("nv_lo");
DeclarationMatcher lo2FuncMatcher = functionDecl(hasName("nv_lo2")).bind("nv_lo");
DeclarationMatcher lo3FuncMatcher = functionDecl(hasName("nv_lo3")).bind("nv_lo");
DeclarationMatcher kgenesMatcher = varDecl(hasName("kgenes")).bind("kgenes");
DeclarationMatcher fgenesMatcher = varDecl(hasName("fgenes")).bind("fgenes");

const LangOptions langOpt = LangOptions();


class FBodys{
private: 
string fBodys[4];
int fCounter = 0;
public:
    FBodys(){}
    void Push(string fBody){
        fBodys[fCounter++] = fBody; 
    }
    string RandPull(){
        int tmpId = rand()%fCounter;
        string fBody = fBodys[tmpId]; 
        int i = 0;
        for(i = tmpId; i< fCounter - 1; i++)
        {
             fBodys[i] = fBodys[i+1];
        }
        fCounter--;
        return fBody;
    }
}loFBodys;

class FuncReadCallback : public MatchFinder::MatchCallback {
private:
  Replacements* replace;
  const SourceManager* sm; 
public :
  FuncReadCallback(Replacements* replace):replace(replace){};
  virtual void run(const MatchFinder::MatchResult &result) {
    sm = result.SourceManager;
    const FunctionDecl* fDecl = result.Nodes.getDeclAs<clang::FunctionDecl>("nv_lo");
    CharSourceRange charSrcRange = CharSourceRange::getTokenRange(fDecl->getBody()->getSourceRange());
    string fBody = Lexer::getSourceText(charSrcRange, *sm, LangOptions(), 0);
    loFBodys.Push(fBody);
  }
};

class FuncRewriteCallback : public MatchFinder::MatchCallback {
private:
  Replacements* replace;
  const SourceManager* sm; 
public :
  FuncRewriteCallback(Replacements* replace):replace(replace){};
  virtual void run(const MatchFinder::MatchResult &result) {
    sm = result.SourceManager;
    const FunctionDecl* fDecl = result.Nodes.getDeclAs<clang::FunctionDecl>("nv_lo");
    CharSourceRange charSrcRange = CharSourceRange::getTokenRange(fDecl->getBody()->getSourceRange());
    string newBody = loFBodys.RandPull();
    Replacement rep(*sm, charSrcRange, newBody, langOpt);
    replace->insert(rep);
  }
};

class KgenesCallback : public MatchFinder::MatchCallback {
private:
  Replacements* replace;
public :
  KgenesCallback(Replacements* replace):replace(replace){};
  virtual void run(const MatchFinder::MatchResult &Result) {
    const VarDecl* var = Result.Nodes.getDeclAs<clang::VarDecl>("kgenes");
    string genes =  "int kgenes[80] = {";
    int i=0;
    for (i=0; i<79; i++){
	genes = genes + to_string(rand()%4) + ",";	
    }
    genes = genes + to_string(rand()%4) + "}";	

    Replacement rep(*(Result.SourceManager), var, genes, langOpt);
    replace->insert(rep);
  }
};

class FgenesCallback : public MatchFinder::MatchCallback {
private:
  Replacements* replace;
public :
  FgenesCallback(Replacements* replace):replace(replace){};
  virtual void run(const MatchFinder::MatchResult &Result) {
    const VarDecl* var = Result.Nodes.getDeclAs<clang::VarDecl>("fgenes");
    string genes =  "int fgenes[80] = {";
    int i=0;
    for (i=0; i<79; i++){
	genes = genes + to_string(rand()%4) + ",";	
    }
    genes = genes + to_string(rand()%4) + "}";	

    Replacement rep(*(Result.SourceManager), var, genes, langOpt);
    replace->insert(rep);
  }
};

int main(int argc, const char **argv) {

    int result;
    CommonOptionsParser op(argc, argv, myToolCategory); 
    RefactoringTool Tool(op.getCompilations(), op.getSourcePathList());

    srand (time(NULL));

    //"L1 NVO"
    if(nvoLevel == 0){
    	outs() << "NVO Level:0:Pure seed:\n";
    	KgenesCallback kgenesCallback(&Tool.getReplacements());
    	FgenesCallback fgenesCallback(&Tool.getReplacements());

    	MatchFinder finder;

    	finder.addMatcher(kgenesMatcher, &kgenesCallback);
    	finder.addMatcher(fgenesMatcher, &fgenesCallback);
     
    	result = Tool.runAndSave(newFrontendActionFactory(&finder).get());
    	outs() << "Replacements collected by the tool:\n";
    	for (auto &r : Tool.getReplacements()) {
	    outs() << r.toString() << "\n";
    	}
    }

    if(nvoLevel == 1){
    	outs() << "NVO Level:1:Function randomness:\n";
    	FuncReadCallback lo0ReadCallback(&Tool.getReplacements());
    	FuncReadCallback lo1ReadCallback(&Tool.getReplacements());
    	FuncReadCallback lo2ReadCallback(&Tool.getReplacements());
    	FuncReadCallback lo3ReadCallback(&Tool.getReplacements());
    	FuncRewriteCallback lo0RewriteCallback(&Tool.getReplacements());
    	FuncRewriteCallback lo1RewriteCallback(&Tool.getReplacements());
    	FuncRewriteCallback lo2RewriteCallback(&Tool.getReplacements());
    	FuncRewriteCallback lo3RewriteCallback(&Tool.getReplacements());
    	KgenesCallback kgenesCallback(&Tool.getReplacements());
    	FgenesCallback fgenesCallback(&Tool.getReplacements());

    	MatchFinder finder;

    	finder.addMatcher(lo0FuncMatcher, &lo0ReadCallback);
    	finder.addMatcher(lo1FuncMatcher, &lo1ReadCallback);
    	finder.addMatcher(lo2FuncMatcher, &lo2ReadCallback);
    	finder.addMatcher(lo3FuncMatcher, &lo3ReadCallback);
 
    	result = Tool.runAndSave(newFrontendActionFactory(&finder).get());

    	MatchFinder finder2;

    	finder2.addMatcher(lo0FuncMatcher, &lo0RewriteCallback);
    	finder2.addMatcher(lo1FuncMatcher, &lo1RewriteCallback);
    	finder2.addMatcher(lo2FuncMatcher, &lo2RewriteCallback);
    	finder2.addMatcher(lo3FuncMatcher, &lo3RewriteCallback);
    	
        finder2.addMatcher(kgenesMatcher, &kgenesCallback);
    	finder2.addMatcher(fgenesMatcher, &fgenesCallback);
 
    	result = Tool.runAndSave(newFrontendActionFactory(&finder2).get());
    	outs() << "Replacements collected by the tool:\n";
    	for (auto &r : Tool.getReplacements()) {
	    outs() << r.toString() << "\n";
    	}
    }
    return result;
}
