#include "varobf.h" 

DeclarationMatcher varDeclMatcher = varDecl().bind("varDecl");
StatementMatcher varRefMatcher = declRefExpr().bind("varRef");

void VarDeclCallback::run(const MatchFinder::MatchResult &result) {
    srcMgr = result.SourceManager;
    const VarDecl* varDecl = result.Nodes.getDeclAs<clang::VarDecl>("varDecl");
    string fileName = srcMgr->getFilename(varDecl->getLocation());
    string varName = varDecl->getName();
    if (fileName.compare(0,projPath.size(),projPath) != 0
        || varName.empty()){ //system header file
        return ;
    }
    //TODO:Variable Domain

    string newName;
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
    
    SourceLocation varLoc = varDecl->getInnerLocStart();
    Replacement rep(*srcMgr, varLoc, varName.length(), newName);
    replace->insert(rep);
    outs() << "Replace VarDecl: " << varName << " to " << newName << "\n";
}

void VarRefCallback::run(const MatchFinder::MatchResult &result) {
    srcMgr = result.SourceManager;
    const DeclRefExpr* declRefExpr = result.Nodes.getDeclAs<clang::DeclRefExpr>("varRef");

    string varName, newName;

    varName = declRefExpr->getNameInfo().getName().getAsString();

    //TODO:
    if (varName != ""){
        newName = funcMap[varName];
    }    

    if (newName != ""){
        SourceLocation srcStart = declRefExpr->getLocStart();
        Replacement rep(*srcMgr, srcStart, varName.length(), newName);
        replace->insert(rep);
        outs() << "** Rewrote function call\n" << varName<<" to "<<newName;
    }
}
