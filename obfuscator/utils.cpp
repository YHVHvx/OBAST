#include "utils.h"

int SaveFuncMap(){
    int result = 0;
    string filename = projPath + "/log.txt";
    ofstream out(filename, ios::app);
    if(out.is_open()){
        out<<"==FUNCTION NAME OBFUSCATION========="<<endl;
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
