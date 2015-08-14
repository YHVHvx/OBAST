#include "obf_func.h"

map<string, string> funcMap;
Rewriter rewriter;

bool MyObfVisitor::VisitFunctionDecl(FunctionDecl *func) {
    string fileName = rewriter.getSourceMgr().getFilename(func->getLocation());
    if (fileName.compare(0,projPath.size(),projPath) != 0){
        return false;
    }
    string funcName = func->getNameInfo().getName().getAsString();
    char newName[32];
    if (funcMap[funcName] != "") {
        sprintf(newName,"%s",funcMap[funcName].c_str());
    }
    else{
        int i;
        for (i = 0; i < 31; i++){
            newName[i] = rand()%26 + 65;
        }
        funcMap[funcName] = newName;
    }
    rewriter.ReplaceText(func->getNameInfo().getSourceRange(), newName);
    errs() << "** Rewrote function def: " << funcName << " to " << newName << "in File" << fileName <<"\n";
    return true;
}

bool MyObfVisitor::VisitCallExpr(CallExpr *call) {
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



void MyObfASTConsumer::HandleTranslationUnit(ASTContext &Context) {
    visitor->TraverseDecl(Context.getTranslationUnitDecl());
}


bool MyObfASTConsumer::HandleTopLevelDecl(DeclGroupRef DG) {
    for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; i++) {
        Decl *D = *i;    
        visitor->TraverseDecl(D); // recursively visit each AST node in Decl "D"
    }
    return true;
}

std::unique_ptr<clang::ASTConsumer> MyObfFrontendAction::CreateASTConsumer(CompilerInstance &CI, StringRef file) {
    return std::unique_ptr<clang::ASTConsumer>(new MyObfASTConsumer(&CI)); // pass CI pointer to ASTConsumer
}
