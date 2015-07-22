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
static cl::opt<int> nvoLevel("L", cl::desc("Specify nvo level"), cl::value_desc("1,2,3"), cl::cat(myToolCategory));
//static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::extrahelp MoreHelp("\n Example Command: nvocomp ~/gadget/sha1.c -L=1 --\n");

DeclarationMatcher nvoFuncMatcher = functionDecl(hasName("NVO_MODULE")).bind("nvo");
DeclarationMatcher kgenesMatcher = varDecl(hasName("kgenes")).bind("kgenes");
DeclarationMatcher fgenesMatcher = varDecl(hasName("fgenes")).bind("fgenes");

const LangOptions langOpt = LangOptions();

class NVOCallback : public MatchFinder::MatchCallback {
private:
  Replacements* replace;
public :
  NVOCallback(Replacements* replace):replace(replace){};
  virtual void run(const MatchFinder::MatchResult &Result) {
    const FunctionDecl* f = Result.Nodes.getDeclAs<clang::FunctionDecl>("nvo");
    //f->dump();
    Replacement rep(*(Result.SourceManager), f->getLocStart(), 0, "// the 'NVO' part\n");
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
    if(nvoLevel == 1){
    	//NVOCallback nvoCallback(&Tool.getReplacements());
    	KgenesCallback kgenesCallback(&Tool.getReplacements());
    	FgenesCallback fgenesCallback(&Tool.getReplacements());

    	MatchFinder finder;

    	//finder.addMatcher(nvoFuncMatcher, &nvoCallback);
    	finder.addMatcher(kgenesMatcher, &kgenesCallback);
    	finder.addMatcher(fgenesMatcher, &fgenesCallback);
     
    	result = Tool.runAndSave(newFrontendActionFactory(&finder).get());
    	outs() << "Replacements collected by the tool:\n";
    	for (auto &r : Tool.getReplacements()) {
	    outs() << r.toString() << "\n";
    	}
    }
    return result;
}
