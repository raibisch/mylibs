#pragma once

#include <Arduino.h>
#include "fs_switch.h"

// Wandelt Source-Code Variablen-Nam varVALUE_i_ABL_minAmpere en in String um :-))
#define GETVARNAME(Variable) (#Variable)

// now def in platformio.ini
//#define DEBUG 1    // SET TO 0 OUT TO REMOVE TRACES

#ifdef DEBUG_PRINT
#define debug_begin(...) Serial.begin(__VA_ARGS__);
#define debug_print(...) Serial.print(__VA_ARGS__);
#define debug_write(...) Serial.write(__VA_ARGS__);
#define debug_println(...) Serial.println(__VA_ARGS__);
#define debug_printf(...) Serial.printf(__VA_ARGS__);
#else
#define debug_begin(...)
#define debug_print(...)
#define debug_printf(...)
#define debug_write(...)
#define debug_println(...)
#endif

class FileVarStore
{
  public:
    FileVarStore (const String filename);
    FileVarStore():FileVarStore("/config.txt") {}
    ~FileVarStore ();
  
    bool isLoaded();
    bool Load();
    bool Save(const String& s);
    const char* GetBuffer() {return _sBuf.c_str();}
    
    String  GetVarString(String name);
    bool    SetVarString(String sKey, String iVal, bool bPersistent=false); 

    int32_t GetVarInt(String name, int32_t defaultvalue=0);
    float   GetVarFloat(String name, float defaultvalue=0);
 
private:
    
protected:
    String _filename;
    enum {MAX_INIT_FILE_SIZE=2000};
    String _sBuf;
    virtual void GetVariables(){}; 
    //struct tm GetTime(String name); not used
    bool _isLoaded = false;
};
