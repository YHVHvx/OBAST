#include "cfgobf.h" 

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;
using namespace clang::ast_matchers::internal;

vector<string> opConstCodes; 
vector<string> junkCodes; 

//TODO: MORE MATCHER
StatementMatcher binOpMatcher = binaryOperator(
			unless(hasParent(implicitCastExpr())),//Handle cast inside a conditionalOperator
			unless(hasParent(conditionalOperator())),//Handle if, for, while
			hasRHS(callExpr()),
			hasOperatorName("=")).bind("binOp");

void InitCodeSet(){
    opConstCodes.push_back("char *varOpConst = \"test\";\n"
    		"if((int)varOpConst % 4 == 0)");    
    junkCodes.push_back("int jospsss = 0;\n");
    junkCodes.push_back("struct timeval tJkTime;\n"
    		"gettimeofday(&tJkTime, NULL);\n"
    		"srand (1000000*tJkTime.tv_sec+tJkTime.tv_usec);\n");
}


void BinOpCallback::run(const MatchFinder::MatchResult &result) {
    srcMgr = result.SourceManager;
    const BinaryOperator* binOp = result.Nodes.getNodeAs<clang::BinaryOperator>("binOp");
    //binOp->dump();
    //outs() << binOp->getOpcodeStr();
    string fileName = srcMgr->getFilename(binOp->getOperatorLoc());
    if (fileName.compare(0,projPath.size(),projPath) != 0){
        return ;
    }

    //TODO CHECK DECL
    SourceLocation opStartLoc = binOp->getLHS()->getLocStart();
    SourceLocation opEndLoc = binOp->getRHS()->getLocEnd().getLocWithOffset(2);

    string strOpaqueConst = opConstCodes[0] + "{\n";
    string strJunkCodes = "\n} else { \n" + junkCodes[0] +"}";
    
    Replacement repOpConst(*srcMgr, opStartLoc, 0, strOpaqueConst);
    Replacement repJunk(*srcMgr, opEndLoc, 0, strJunkCodes);
    replace->insert(repOpConst);
    replace->insert(repJunk);
}
