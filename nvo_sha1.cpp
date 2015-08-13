#include "nvo_sha1.h" 

FBodys loFBodys;

DeclarationMatcher lo0FuncMatcher = functionDecl(hasName("nv_lo0")).bind("nv_lo");
DeclarationMatcher lo1FuncMatcher = functionDecl(hasName("nv_lo1")).bind("nv_lo");
DeclarationMatcher lo2FuncMatcher = functionDecl(hasName("nv_lo2")).bind("nv_lo");
DeclarationMatcher lo3FuncMatcher = functionDecl(hasName("nv_lo3")).bind("nv_lo");
DeclarationMatcher kgenesMatcher = varDecl(hasName("kgenes_raw")).bind("kgenes_raw");
DeclarationMatcher fgenesMatcher = varDecl(hasName("fgenes_raw")).bind("fgenes_raw");

void FBodys::Push(string fBody){
    fBodys[fCounter++] = fBody; 
}

string FBodys::RandPull(){
    int tmpId = rand()%fCounter;
    string fBody = fBodys[tmpId]; 
    int i = 0;
    for(i = tmpId; i< fCounter - 1; i++)
    {
         fBodys[i] = fBodys[i+1];
    }
    fCounter--;
    return fBody;
}

void FuncReadCallback::run(const MatchFinder::MatchResult &result) {
    sm = result.SourceManager;
    const FunctionDecl* fDecl = result.Nodes.getDeclAs<clang::FunctionDecl>("nv_lo");
    CharSourceRange charSrcRange = CharSourceRange::getTokenRange(fDecl->getBody()->getSourceRange());
    string fBody = Lexer::getSourceText(charSrcRange, *sm, LangOptions(), 0);
    loFBodys.Push(fBody);
}


void FuncRewriteCallback::run(const MatchFinder::MatchResult &result) {
    sm = result.SourceManager;
    const FunctionDecl* fDecl = result.Nodes.getDeclAs<clang::FunctionDecl>("nv_lo");
    CharSourceRange charSrcRange = CharSourceRange::getTokenRange(fDecl->getBody()->getSourceRange());
    string newBody = loFBodys.RandPull();
    Replacement rep(*sm, charSrcRange, newBody, LangOptions());
    replace->insert(rep);
}

void KgenesCallback::run(const MatchFinder::MatchResult &Result) {
    const VarDecl* var = Result.Nodes.getDeclAs<clang::VarDecl>("kgenes_raw");
    string genes =  "uint8_t kgenes_raw[20] = {";
    int i=0;
    for (i=0; i<19; i++){
	genes = genes + to_string(rand()%256) + ",";	
    }
    genes = genes + to_string(rand()%256) + "};\n";	

    Replacement rep(*(Result.SourceManager), var, genes, LangOptions());
    replace->insert(rep);
}

void FgenesCallback::run(const MatchFinder::MatchResult &Result) {
    const VarDecl* var = Result.Nodes.getDeclAs<clang::VarDecl>("fgenes_raw");
    string genes =  "uint8_t fgenes_raw[20] = {";
    int i=0;
    for (i=0; i<19; i++){
	genes = genes + to_string(rand()%256) + ",";	
    }
    genes = genes + to_string(rand()%256) + "};\n";	

    Replacement rep(*(Result.SourceManager), var, genes, LangOptions());
    replace->insert(rep);
}
