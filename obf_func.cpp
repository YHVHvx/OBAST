#include "obf_func.h" 

DeclarationMatcher funcDeclMatcher = functionDecl().bind("funcDecl");
StatementMatcher funcCallMatcher = callExpr().bind("funcCall");

void FuncDeclCallback::run(const MatchFinder::MatchResult &result) {
    srcMgr = result.SourceManager;
    const FunctionDecl* funcDecl = result.Nodes.getDeclAs<clang::FunctionDecl>("funcDecl");
    string fileName = srcMgr->getFilename(funcDecl->getLocation());
    string funcName = funcDecl->getNameInfo().getName().getAsString();
    if (fileName.compare(0,projPath.size(),projPath) != 0 //system header file
        ||HasBeenObfuscated(funcName)==true){ //obfuscated function name
        return ;
    }
    char newName[24];
    string strNewName;
    if (funcMap.count(funcName) != 0) {
        sprintf(newName,"%s",funcMap[funcName].c_str());
    }
    else {
        int i;
        for (i = 0; i < 24; i++){
            newName[i] = rand()%26 + 65;
        }
        strNewName = newName;
        funcMap[funcName] = strNewName;
    }
    
    SourceRange srcRange = funcDecl->getNameInfo().getSourceRange();
    CharSourceRange charSrcRange = CharSourceRange::getTokenRange(srcRange);
    Replacement rep(*srcMgr, charSrcRange, strNewName, LangOptions());
    replace->insert(rep);
}

void FuncCallCallback::run(const MatchFinder::MatchResult &result) {
    srcMgr = result.SourceManager;
    const CallExpr* call = result.Nodes.getDeclAs<clang::CallExpr>("funcCall");

    string fileName = srcMgr->getFilename(funcDecl->getLocation());
    if (fileName.compare(projPath.size(),fileName.size(),"obf_bridge.cpp") == 0){ 
        //the bridge file
        return ;
    }
    const FunctionDecl* funcDecl = call->getDirectCallee();
    string funcName, newName;

    if(funcDecl != NULL ) {
        //This is a C function call
        funcName = funcDecl->getNameInfo().getName().getAsString();
    }
    else{
/*
        //This is a CXX function call
        ImplicitCastExpr *imp = dyn_cast<ImplicitCastExpr>(call);
        if (imp){
            Expr *sub = imp->getSubExpr();
            if(sub){
                DeclRefExpr *def = dyn_cast<DeclRefExpr>(sub);
                if(def){  
                    funcName =  def->getFoundDecl()->getName();
                }
            }
        }
*/
    }
    if (funcName != ""){
        newName = funcMap[funcName];
    }    
    //errs() << "** Rewrote function call\n" << funcName<<" to "<<newName;
    if (newName != ""){
        SourceLocation srcBegin = call->getLocStart();
        SourceLocation srcEnd= call->getLocEnd();
        CharSourceRange charSrcRange = CharSourceRange::getTokenRange(srcBegin,srcEnd);
        Replacement rep(*srcMgr, charSrcRange, newName, LangOptions());
        replace->insert(rep);
        outs() << "** Rewrote function call\n" << funcName<<" to "<<newName;
    }
    
}
