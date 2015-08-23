#include "nvo_bridge.h"
#include "utils.h"

MyBridgeVisitor::MyBridgeVisitor(CompilerInstance *CI) 
      : astContext(&(CI->getASTContext()))
{
    rewriter.setSourceMgr(astContext->getSourceManager(), astContext->getLangOpts());
}

bool MyBridgeVisitor::VisitCallExpr(CallExpr *call) {
    string funcName = call->getDirectCallee()->getNameInfo().getName().getAsString();
    string newName;

    if (funcName != ""){
        newName = funcMap[funcName];
    }    
    if (newName != ""){
        rewriter.ReplaceText(call->getLocStart(), funcName.length(), newName);
        errs() << "** Rewrote function call\n" << funcName<<" to "<<newName;
    }
    return true;
}


void MyBridgeASTConsumer::HandleTranslationUnit(ASTContext &Context) {
    visitor->TraverseDecl(Context.getTranslationUnitDecl());
}


/*
bool MyBridgeASTConsumer::HandleTopLevelDecl(DeclGroupRef DG) {
    for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; i++) {
        Decl *D = *i;    
        visitor->TraverseDecl(D); // recursively visit each AST node in Decl "D"
    }
    return true;
}
*/
std::unique_ptr<clang::ASTConsumer> MyBridgeFrontendAction::CreateASTConsumer(CompilerInstance &CI, StringRef file) {
    return std::unique_ptr<clang::ASTConsumer>(new MyBridgeASTConsumer(&CI)); // pass CI pointer to ASTConsumer
}
