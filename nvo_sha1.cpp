#include "nvo_sha1.h" 
#include "obf_func.h" 

FBodys loFBodys;

DeclarationMatcher lo0FuncMatcher = functionDecl(hasName("nv_lo0")).bind("nv_lo");
DeclarationMatcher lo1FuncMatcher = functionDecl(hasName("nv_lo1")).bind("nv_lo");
DeclarationMatcher lo2FuncMatcher = functionDecl(hasName("nv_lo2")).bind("nv_lo");
DeclarationMatcher lo3FuncMatcher = functionDecl(hasName("nv_lo3")).bind("nv_lo");
DeclarationMatcher kgenesMatcher = varDecl(hasName("kgenes_raw")).bind("kgenes_raw");
DeclarationMatcher fgenesMatcher = varDecl(hasName("fgenes_raw")).bind("fgenes_raw");

uint8_t kgenes[20];
uint8_t fgenes[20];

void FBodys::Push(string fBody){
    fBodys[fCounter++] = fBody; 
}

string FBodys::RandPull(){
    uint8_t tmpId = rand()%fCounter;
    forder[4-fCounter] = tmpId;
    string fBody = fBodys[tmpId]; 
    int i = 0;
    for(i = tmpId; i< fCounter - 1; i++)
    {
         fBodys[i] = fBodys[i+1];
    }
    fCounter--;
    return fBody;
}

void FBodys::GetRealOrder(){
    uint8_t tmpIndexer[4] = {0,1,2,3};
    int i,j;
    for (i=0; i<4; i++){
        realOrder[i] = tmpIndexer[forder[i]];
        for(j = forder[i]; j< 3-i; j++)
        {
            tmpIndexer[j] = tmpIndexer[j+1];
        }
    }
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

    int i=0;
    for (i=0; i<20; i++){
	kgenes[i] = rand()%256;	
    }

    string genesDecl =  "uint8_t kgenes_raw[20] = {" + to_string(kgenes[0]);	
    for (i=1; i<20; i++){
	genesDecl = genesDecl + "," + to_string(kgenes[i]);	
    }

    genesDecl = genesDecl + "};";	

    Replacement rep(*(Result.SourceManager), var, genesDecl, LangOptions());
    replace->insert(rep);
}

void FgenesCallback::run(const MatchFinder::MatchResult &Result) {
    const VarDecl* var = Result.Nodes.getDeclAs<clang::VarDecl>("fgenes_raw");

    int i=0;
    for (i=0; i<20; i++){
	fgenes[i] = rand()%256;	
    }

    string genesDecl =  "uint8_t fgenes_raw[20] = {" + to_string(fgenes[0]);	
    for (i=1; i<20; i++){
	genesDecl = genesDecl + "," + to_string(fgenes[i]);	
    }

    genesDecl = genesDecl + "};";	

    Replacement rep(*(Result.SourceManager), var, genesDecl, LangOptions());
    replace->insert(rep);
}

genes80_t AdjustGenes(uint8_t genes[]){

    genes80_t genesBits = BitstoGenes(genes);
/*
    int fOrder[4];
    int i;    
    for(i=0;i<80;i++){
        genesBits.genes[i] = fOrder[genesBits.genes[i]];
    }
i*/
    return genesBits;
}

int SaveGenes(uint8_t genes[], int genSize){

    int result = 0;
    string filename = projPath + "/genes.txt";

    int i = 0;
    string strGenes;
    for(i=0;i<genSize;i++){
        strGenes = strGenes + to_string(genes[i]);
    }
    fstream out;
    out.open(filename,ios::app);
    if(out.is_open()){
        out<<strGenes<<endl;
        out.close();
    }
    return result;
}
