#ifndef __NVO_SHA1_UTILS_H__
#define __NVO_SHA1_UTILS_H__

#include <string>
#include <fstream>
#include <sstream>
#include "nvo_sha1.h"

extern std::string projPath;
extern uint8_t kgenes[20];
extern uint8_t fgenes[20];
extern FBodys loFBodys;

int SaveGenes();
int SaveFuncOrder();

#endif
