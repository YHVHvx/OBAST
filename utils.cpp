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
