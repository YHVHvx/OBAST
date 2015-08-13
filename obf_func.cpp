#include "obf_func.h"

map<string, string> funcMap;
Rewriter rewriter;

bool MyObfVisitor::VisitFunctionDecl(FunctionDecl *func) {
    string funcName = func->getNameInfo().getName().getAsString();
    char newName[8];
    int i;
    for (i = 0; i < 8; i++){
        newName[i] = (char) rand()%26 + 65;
    }
    funcMap[funcName] = newName;
    rewriter.ReplaceText(func->getLocation(), funcName.length(), newName);
    errs() << "** Rewrote function def: " << funcName << "\n";
    return true;
}

bool MyObfVisitor::VisitCallExpr(CallExpr *call) {
    string funcName = call->getCallee()->getType().getAsString();
    errs() <<funcName<<"\n";
    string newName = funcMap[funcName];
    if (newName != NULL){
        rewriter.ReplaceText(call->getLocStart(), funcName.length(), newName);
        errs() << "** Rewrote function call\n";
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
