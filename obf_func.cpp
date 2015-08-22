#include "obf_func.h"

map<string, string> funcMap;
Rewriter rewriter;

int LoadFuncMap(){
    int result = 0;
    string filename = projPath + "/tmp.txt";
    char buffer[128];
    ifstream in(filename);
    if(!in.is_open()){
        outs()<<"Error Loading " << filename << "\n";
        return -1;
    }
    while (!in.eof() )  
    {  
        in.getline(buffer,100); 
        char* oriName = strtok (buffer,","); 
        char* obfName = strtok (NULL,",");
        if(obfName!=NULL){
            funcMap[oriName] = obfName;
        }
    }
        
    in.close();

    return result;
}

int SaveFuncMap(){
    int result = 0;
    string filename = projPath + "/tmp.txt";
    ofstream out(filename);
    if(out.is_open()){
        
        map<string,string>::iterator it;
        for(it=funcMap.begin();it!=funcMap.end();++it){
            out<<it->first <<","<<it->second<<endl;
        }
        out.close();
    }
    return result;
}

bool HasBeenObfuscated(string funcName){
    map<string,string>::iterator it;
    for(it=funcMap.begin();it!=funcMap.end();++it){
        if (funcName.compare(0,funcName.size(),it->second) == 0){
	    return true;
	}
    }
    return false;
}

MyObfVisitor::MyObfVisitor(CompilerInstance *CI) 
      : astContext(&(CI->getASTContext()))
{
    rewriter.setSourceMgr(astContext->getSourceManager(), astContext->getLangOpts());
}

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
    else {
	if(HasBeenObfuscated(funcName)==false){
            int i;
            for (i = 0; i < 31; i++){
                newName[i] = rand()%26 + 65;
            }
            funcMap[funcName] = newName;
	}
        else
	    return false;
    }

    //Replacement rep(*sm, charSrcRange, newBody, LangOptions());
    //replace->insert(rep);

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
