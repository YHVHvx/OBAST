#include "varobf.h" 

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;
using namespace clang::ast_matchers::internal;

DeclarationMatcher varDeclMatcher = varDecl().bind("varDecl");
StatementMatcher varRefMatcher = declRefExpr().bind("varRef");

void VarDeclCallback::run(const MatchFinder::MatchResult &result) {
    srcMgr = result.SourceManager;
    const VarDecl* varDecl = result.Nodes.getDeclAs<clang::VarDecl>("varDecl");
    string fileName = srcMgr->getFilename(varDecl->getLocation());
    string varName = varDecl->getName();
    string newName;

    if (fileName.compare(0,projPath.size(),projPath) != 0
        || varName.empty()){ //system header file
        return ;
    }
    //TODO:Variable Domain
    if(IsCXXForRangeStmt(varDecl)){
	return;
    }
    if (varMap.count(varName) != 0) {
        //sprintf(newName,"%s",funcMap[funcName].c_str());
        newName = varMap[varName];
    }
    else {
        int i;
        for (i = 0; i < 15; i++){
            char tmpChar = rand()%26 + 65;
            newName = newName + tmpChar;
        }
        varMap[varName] = newName;
    }
    //varDecl->dump();
    /*Obfuscate the declarations*/ 
    SourceLocation varLoc = varDecl->getLocation();//getLocation() start from the variable name
    Replacement rep(*srcMgr, varLoc, varName.length(), newName);
    replace->insert(rep);
    outs() << "Replace VarDecl: " << varName << " to " << newName << "\n";
}

void VarRefCallback::run(const MatchFinder::MatchResult &result) {
    srcMgr = result.SourceManager;
    const DeclRefExpr* declRefExpr = result.Nodes.getDeclAs<clang::DeclRefExpr>("varRef");
    string varName, newName;

    varName = declRefExpr->getNameInfo().getName().getAsString();
    if (varName != ""){
        newName = varMap[varName];
    }    
    if (newName != ""){
        SourceLocation srcStart = declRefExpr->getLocStart();
        Replacement rep(*srcMgr, srcStart, varName.length(), newName);
        replace->insert(rep);
        outs() << "** Rewrote VarDeclRef: " << varName<<" to "<<newName << "\n";
    }
}
