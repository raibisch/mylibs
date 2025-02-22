#include "FileVarStore.h"

#define FIND_KEY_LOWERCASE

//----------------------------------------------------public Functions----------------------------------------------------

//default constructor 
/* 7.12.24 now overrides the other constructor in FileVarStore.h 
FileVarStore::FileVarStore()
{
  _filename = "/config.txt";
}
*/

// constructor with custom filename
FileVarStore::FileVarStore(const String fn)
{
  _filename = fn;
  _sBuf.reserve(MAX_INIT_FILE_SIZE);
}

/*-----------------------------
destructor for class, not needed by Arduino but for complete class. Calls Arduino end function
*/
FileVarStore::~FileVarStore()
{
  
}

bool FileVarStore::Load()
{
  File configFile = myFS.open(_filename);
  if (!configFile) 
  {
    debug_println("Failed to open config file");
    _isLoaded = false;
    return false;
  }

  size_t size = configFile.size();
  debug_printf("size of config.txt:%d \n", size);
  if (size > MAX_INIT_FILE_SIZE) {
    debug_println("*ERROR: Config file size is too large");
    _isLoaded = false;
    return false;
  }

  _sBuf = configFile.readString();
  _sBuf.replace(" ","");

  // ACHTUNG führt zu Fehler bei Passwort mit '#' ..geht so leider nicht !!
  //_sBuf.replace('#',';');    // allow '#' as comment

  _sBuf.replace(";var",";"); // dirty hack for comment before var-definition
  _sBuf.replace("; var", ";");

  GetVariables(); // virtual !!

  configFile.close();
  debug_println("config.txt load OK!");
  _isLoaded = true;
  return true;
}

bool FileVarStore::isLoaded()
{
  return _isLoaded;
}

bool FileVarStore::Save(const String & s)
{
  File configFile = myFS.open(_filename, "w");
  if (!configFile) 
  {
    debug_println("ERROR: Failed to open config file for writing");
    return false;
  }
  configFile.print(s);
 
  configFile.close();
  debug_println("config.txt save OK!");
   // daten neu laden
  Load();
  return true;
}

// private member
String FileVarStore::GetVarString(String sK)
{
  int posStart = _sBuf.indexOf(sK);
  if (posStart < 0)
  {
#ifdef FIND_KEY_LOWERCASE
    sK.toLowerCase();
    String s2 = _sBuf;
    s2.toLowerCase();
    posStart = s2.indexOf(sK);
    if(posStart < 0)
    {
      debug_printf("key: %s not found\r\n", sK.c_str());
      return "";
    }
#else
    debug_printf("key: %s not found\r\n", sK.cstr());
    return "";
#endif
  }

  //Serial.printf("PosStart1:%d", posStart);
  posStart = _sBuf.indexOf('=', posStart);
  //Serial.printf("PosStart2:%d", posStart);
  int posEnd   = _sBuf.indexOf(';', posStart);
  int posCR    = _sBuf.indexOf(0x0a, posStart); // test for LF
  if (posEnd > posCR) // no ';' at End
  {
    debug_println("Warning: end marker ';' missing");
    posEnd = posCR;
  }
  String sVal = _sBuf.substring(posStart+1,posEnd); 
  
  debug_print(sK+":");
  debug_print(sVal); 
  debug_printf(" length:%d\r\n", sVal.length());

  // by JG 26.04.2024 trick for sprintf '%' modifier
  sVal.replace('&','%');
  return sVal;
}


/*
int32_t FileVarStore::GetVarInt(String sKey)
{   
  int32_t val = 0;
  ---------- comment
  int posStart = _sBuf.indexOf(sKey);
  if (posStart < 0)
  {
    debug_println("key:'"+sKey+ "' not found!!");
    return -1;
  }
  //Serial.printf("PosStart1:%d", posStart);
  posStart = _sBuf.indexOf('=', posStart);
  //Serial.printf("PosStart2:%d", posStart);
  int posEnd   = _sBuf.indexOf('\n', posStart);
  String sVal = _sBuf.substring(posStart+1,posEnd); 
  debug_print(sKey+":");
  val = sVal.toInt();
  ----------------- comment 
  val = GetVarString(sKey).toInt();
  debug_println(val); 
  return val;
 
}
*/

int32_t FileVarStore::GetVarInt(String sKey, int32_t defaultvalue)
{   
  int32_t val = defaultvalue;
  
  val = GetVarString(sKey).toInt();
  debug_printf("Int-Value=%d\r\n", val); 
  return val;
}


float FileVarStore::GetVarFloat(String sKey, float defaultvalue)
{
  float val = 0;
  val = GetVarString(sKey).toFloat();
  debug_printf("Float-Value=%f\r\n", val); 
  return val;
}

/*
 /// @brief convert myValue=22:35 ---> tm_hour  and tm_min 
 ///         valid: tm_year =1  unvalid: tm_year=0
 /// @param sKey 
 /// @return 
 struct tm FileVarStore::GetTime(String sKey)
 {
   struct tm xtime;
   xtime.tm_year = 1;

   int32_t val = 0;
   int posStart = _sBuf.indexOf(sKey);
   if (posStart < 0)
   {
     debug_println("key:'"+sKey+ "' not found!!");
     xtime.tm_year = 0;
     return xtime;
   }
  //Serial.printf("PosStart1:%d", posStart);
  posStart = _sBuf.indexOf('=', posStart);
  //Serial.printf("PosStart2:%d", posStart);
  int posEnd   = _sBuf.indexOf(':', posStart);
  String sVal = _sBuf.substring(posStart+1,posEnd); 
  val =sVal.toInt();
  xtime.tm_hour = val;
  posStart = posEnd;
  posEnd   = _sBuf.indexOf('\n', posStart);
  sVal = _sBuf.substring(posStart+1, posEnd);
  val = sVal.toInt();
  xtime.tm_min = val;
  return xtime;
 }  
 */

/// @brief 
/// @param sKey 
/// @param sNewVal 
/// @param bPersistent -->true: store permanent in file
/// @return 
bool FileVarStore::SetVarString(String sKey, String sNewVal, bool bPersistent)
{
   int posStart = _sBuf.indexOf(sKey);
  if (posStart < 0)
  {
    debug_printf("SetVar key: %s not found\r\n", sKey.c_str());
    return false;
  }

  //Serial.printf("PosStart1:%d", posStart);
  posStart = _sBuf.indexOf('=', posStart);
  //Serial.printf("PosStart2:%d", posStart);
  int posEnd   = _sBuf.indexOf(';', posStart);
  String sVal = _sBuf.substring(posStart+1,posEnd);
  String sReplaceOld = sKey;
  sReplaceOld += "=";
  String sReplaceNew = sReplaceOld;
  sReplaceOld += sVal;
  sReplaceNew += sNewVal;
  _sBuf.replace(sReplaceOld, sReplaceNew);
   debug_printf("SetVar key: %s=%s\r\n", sKey.c_str(), sNewVal.c_str());
  if (bPersistent)
  {
   return Save(_sBuf);
  }
  else
  {return true;}
}





