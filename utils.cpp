#include "utils.h"

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
            if (it->second.size() > 0){
                out<<it->first <<","<<it->second<<endl;
            }
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

genes4_t U8to2bits(uint8_t input){
    genes4_t result;
    result.genes[0] = input >> 6;
    result.genes[1] = (input >> 4)%4;
    result.genes[2] = (input >> 2)%4;
    result.genes[3] = input % 4;
    return result;
}


genes80_t BitstoGenes(uint8_t input[20]){
    genes80_t result;
    genes4_t tmp;
    int i,j;

    for (i=0; i<20; i++){
        tmp = U8to2bits(input[i]);
        for(j=0; j<4; j++)
            result.genes[i*4+j] = tmp.genes[0+j];
    }
    return result;
}
 
