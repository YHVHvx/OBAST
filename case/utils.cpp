#include "utils.h"

int SaveGenes(){

    int result = 0;
    string filename = projPath + "/log.txt";

    int i = 0;
    string strFgenes = "FGENES";
    string strKgenes = "KGENES";
    
    for(i=0; i<20; i++){
        strFgenes = strFgenes + ":" + to_string(fgenes[i]);
        strKgenes = strKgenes + ":" + to_string(kgenes[i]);
    }
    fstream out;
    out.open(filename,ios::app);
    if(out.is_open()){
        out << "==GENES==========" << endl;
        out << strFgenes << endl;
        out << strKgenes << endl;
        out.close();
    }
    return result;
}
int SaveFuncOrder(){
    int result = 0;
    string filename = projPath + "/log.txt";
    string strForder = "FORDER";
    loFBodys.GetRealOrder();
    int i = 0;
    for(i=0; i<4; i++){
        strForder = strForder + ":" + to_string(loFBodys.realOrder[i]);
    }
    fstream out;
    out.open(filename,ios::app);
    if(out.is_open()){
        out << "==FORDER==========" << endl;
        out << strForder << endl;
        out.close();
    }
    return result;
} 
