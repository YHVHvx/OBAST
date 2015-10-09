#include "obf_func.h"


MyObfVisitor::MyObfVisitor(CompilerInstance *CI) 
      : astContext(&(CI->getASTContext()))
{
    rewriter.setSourceMgr(astContext->getSourceManager(), astContext->getLangOpts());
}

bool MyObfVisitor::VisitFunctionDecl(FunctionDecl *func) {
    string fileName = rewriter.getSourceMgr().getFilename(func->getLocation());
    string funcName = func->getNameInfo().getName().getAsString();
    if (fileName.compare(0,projPath.size(),projPath) != 0 //system header file
        ||HasBeenObfuscated(funcName)==true){ //obfuscated function name
        return false;
    }
    char newName[32];
    if (funcMap.count(funcName) != 0) {
        sprintf(newName,"%s",funcMap[funcName].c_str());
    }
    else {
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

    FunctionDecl* funcDecl = call->getDirectCallee();
    string funcName, newName;
    if(funcDecl != NULL ) {
        //This is a C function call
        funcName = funcDecl->getNameInfo().getName().getAsString();
    }
    else{
        //This is a CXX function call
        ImplicitCastExpr  *imp = dyn_cast<ImplicitCastExpr>(call);
        if (imp){
            Expr *sub = imp->getSubExpr();
            if(sub){
                DeclRefExpr *def = dyn_cast<DeclRefExpr>(sub);
                if(def){  
                    funcName =  def->getFoundDecl()->getName();
                }
            }
        }
    }
    if (funcName != ""){
        newName = funcMap[funcName];
    }    
    errs() << "** Rewrote function call\n" << funcName<<" to "<<newName;
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
