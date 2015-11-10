#include "funcobf.h" 

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;
using namespace clang::ast_matchers::internal;

DeclarationMatcher funcDeclMatcher = functionDecl().bind("funcDecl");
StatementMatcher funcCallMatcher = callExpr().bind("funcCall");

void FuncDeclCallback::run(const MatchFinder::MatchResult &result) {
    srcMgr = result.SourceManager;
    const FunctionDecl* funcDecl = result.Nodes.getDeclAs<clang::FunctionDecl>("funcDecl");
    string fileName = srcMgr->getFilename(funcDecl->getLocation());
    string funcName = funcDecl->getNameInfo().getName().getAsString();
    if (fileName.compare(0,projPath.size(),projPath) != 0 //system header file
        ||fileName.find("obf_bridge") != string::npos
        ||HasBeenObfuscated(funcName)==true){ //obfuscated function name
        return ;
    }
    string newName;
    if (funcMap.count(funcName) != 0) {
        //sprintf(newName,"%s",funcMap[funcName].c_str());
        newName = funcMap[funcName];
    }
    else {
        int i;
        //Shorter than this may trigger LVMM bugs
        for (i = 0; i < 24; i++){
            char tmpChar = rand()%26 + 65;
            newName = newName + tmpChar;
        }
        funcMap[funcName] = newName;
    }
    SourceRange srcRange = funcDecl->getNameInfo().getSourceRange();
    CharSourceRange charSrcRange = CharSourceRange::getTokenRange(srcRange);
    Replacement rep(*srcMgr, charSrcRange, newName, LangOptions());
    replace->insert(rep);
    outs() << "Replace FunctionDecl: " << funcName << " to " << newName << "\n";
}

void FuncCallCallback::run(const MatchFinder::MatchResult &result) {
    srcMgr = result.SourceManager;
    const CallExpr* call = result.Nodes.getDeclAs<clang::CallExpr>("funcCall");

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
        SourceLocation srcStart = call->getLocStart();
        Replacement rep(*srcMgr, srcStart, funcName.length(), newName);
        replace->insert(rep);
        outs() << "** Rewrote function call\n" << funcName<<" to "<<newName;
    }
}
