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

StatementMatcher cfgMatcher = callExpr().bind("cfg");

void InitCodeSet(){
    opConstCodes.push_back("char *varOpConst = \"test\""
    		"if(varOpConst % 4 == 0)");    
    junkCodes.push_back("struct timeval tJkTime;"
    		"gettimeofday(&tJkTime, NULL);"
    		"srand (1000000*tJkTime.tv_sec+tJkTime.tv_usec);");
}


void CfgCallback::run(const MatchFinder::MatchResult &result) {
    srcMgr = result.SourceManager;
    const Stmt* stmt = result.Nodes.getNodeAs<clang::Stmt>("cfg");
    string fileName = srcMgr->getFilename(stmt->getLocStart());
    if (fileName.compare(0,projPath.size(),projPath) != 0
        ||IsDeclStmt(stmt)
        ||IsReturnStmt(stmt)){ 
        return ;
    }
    SourceLocation stmtStart = stmt->getLocStart();
    SourceLocation stmtEnd = stmt->getLocEnd();

    string strOpaqueConst = opConstCodes[0] + "{";
    string strJunkCodes = "} \n else {" + junkCodes[0] +"\n}";
    
    Replacement rep(*srcMgr, stmtStart, 0, strOpaqueConst);
    replace->insert(rep);
}
