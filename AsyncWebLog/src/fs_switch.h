#pragma once

#ifdef FS_LITTLEFS
#pragma message("Info : USE 'LITTLEFS' Filesystem")
#include <LittleFS.h>
#define myFS LittleFS
#else
#pragma message("Info : USE 'SPIFFS' Filesystem")
#include <SPIFFS.h>
#define myFS SPIFFS
#endif 


