#include "SMLdecode.h"

//----------------------------------------------------public Functions----------------------------------------------------


bool SMLdecode::init(const char* url, const char* user, const char* pass)
{
  _url.reserve(20);
  _passw.reserve(10);
  _usern.reserve(10);
  _url = url; _usern = user; _passw = pass;

  debug_println("SMLdecode-init OK!");
  return true;
}


#ifdef SML_TASMOTA
// special version to get the values from the main energy-meter 
// from tasmota device to read the optical SML-Signal fom the main energy-meter
String SMLdecode::httpGETRequest(const char* http_url) 
{
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(wificlient, http_url);
  
  // If you need Node-RED/server authentication, insert user and password below
  //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = ""; 
  
  if (httpResponseCode>0) {
    //debug_print("HTTP Response code: ");
    //debug_println(httpResponseCode);
    payload = http.getString();
    //debug_printf("httpGetRequest: %s\r\n",payload);
  }
  else {
    debug_print("Error code: ");
    debug_println(httpResponseCode);
  }
  // Free resources
  http.end();

  payload.trim();
  return payload;
}

/// @brief my implementation for reading a TASMOTA device...adapt to your needs
void getSmlMeter()
{
  // by JG : todo: replace hardcoded url with config-value
  String s = httpGETRequest("http://192.168.2.88/cm?cmnd=status%2010");
  /* Format of Tasmota SML
  {
   "StatusSNS": {
    "Time": "2024-04-29T18:28:19",
    "SML": {
      "Total_in": 5953.3436,
      "Total_out": 8620.3801,
      "Power_curr": -27
    }
   }
  }
 */
 JsonDocument doc;
 deserializeJson(doc, s);
 valSML_kwh_in  = doc["StatusSNS"]["SML"]["Total_in"];
 valSML_kwh_out = doc["StatusSNS"]["SML"]["Total_out"];
 valSML_watt   = doc["StatusSNS"]["SML"]["Power_curr"];
 String sml = "SML-Meter-Watt: ";
 sml += String(valSML_watt);
 //AsyncWebLog.println(sml);
}
#endif


#ifdef SML_TIBBER
/* private functions */

// my special version to get the values from the tibber-host 
// ...quick and dirty ;-) only testet for Meter: ISKRA MT631 !!!
// write your own routine if you need this values !!!
byte * SMLdecode::httpGETRequest(const char* http_url) 
{
  HTTPClient http;

  // for TEST !!!
  // _usern = "admin"; 
  // _passw = "xxxx-xxxx";

  int getlength = 0;
  http.setConnectTimeout(500); // 16.02.2025 fix: if url not found or down
  http.begin(_wificlient, http_url);
  http.setAuthorization(_usern.c_str(), _passw.c_str());
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) 
  {
    getlength = http.getSize();
    if ((getlength > SMLPAYLOADMAXSIZE) || (getlength < 200))
    {
      http.end();
      return NULL;
    }

    WiFiClient * w = http.getStreamPtr();
    w->readBytes(smlpayload, getlength);

    if (getlength != 272)
    {
     debug_printf("smlpayload length ?:%d Data: ", getlength);
     for (size_t i = 0; i < 10; i++)
     {
       debug_printf("%02x ",smlpayload[i]);
     }
     debug_println("...");
     getlength = 0;
    }
    
  }
  else 
  {
    debug_print("TIBBER httpGETRequest Error code: ");
    debug_println(httpResponseCode);
  }
  // Free resources
  http.end();

  if (getlength <= 0)
  {
    return NULL;
  }

  return smlpayload;
}

uint32_t SMLdecode::decodeSMLval(byte * payload, byte* smlcode, uint smlsize,  uint offset)
{
  byte *loc = (byte*)memmem(payload, SMLPAYLOADMAXSIZE, smlcode, smlsize);
  if (loc == NULL)
  {
    return 0;
  }

  /* Test fake "-1"
  if (nlen == 2)
  {
    loc[offset]  = 0xff;
    loc[offset+1] = 0xff;
  }
  */
  // 10/2024 neu: 'nlen' aus SML lesen (aendert sich bei Leistung je nach Größe des Wertes !!!)


  uint8_t nlen = (loc[offset-1] & 0x0F) -1;

  /*
  // for extra debugging
  debug_printf("nlen=%d\r\n", nlen);
  debug_print("HEX: ");

  for (size_t i = 0; i < offset+nlen+1; i++)
  {
    debug_printf("%02x ", loc[i]);
  }
  debug_println();
  
  if ((nlen < 1) ||(nlen > 8))
  {
    debug_println("decodeSMLval unvalid length");
    return 0;
  }
  */
  
  byte* pval = loc + offset;
  uint32_t value=0;

 if (nlen == 1)
 {
   value = (int8_t) *(pval);
 }
 else
 {
  nlen=nlen+1; 
  while (--nlen) 
  {
      //value<<=8;
      value = value << 8;
      value|=*(pval)++;
  }
 }

  return value;
}

/* public */

/// @brief my implementation for reading a Tibber device...adapt to your needs
bool SMLdecode::read()
{
  // http://192.168.2.87/data.json?node_id=1  ( for test with webbrowser)
  // varStore.varSML_s_url="192.168.2.87";
  
  String sURL = "http://";
  sURL += _url;
  sURL += "/data.json?node_id=1";
  byte * payload = httpGETRequest(sURL.c_str());
  
  if (payload == NULL)
  {
    return false;
  }

  // Energy IN (1.8.0)
  byte sml_1_8_0[] {0x77, 0x07, 0x01, 0x00, 0x01, 0x08, 0x00, 0xff};
  _inputkWh = double(decodeSMLval(payload, sml_1_8_0, sizeof(sml_1_8_0), 19) / 10000.0);
 
  // Energy OUT (2.8.0)
  byte sml_2_8_0[] {0x77, 0x07, 0x01, 0x00, 0x02, 0x08, 0x00, 0xff};
  _outputkWh = double(decodeSMLval(payload, sml_2_8_0, sizeof(sml_2_8_0), 15) / 10000.0);
 

// for Power:
// length is variable !!! (2 byte= for big values)
//                                           53=16bit int
// 77 07 01 00 10 07 00 ff 01 01 62 1b 52 00 53 00 91 01 145W
// 77 07 01 00 10 07 00 ff 01 01 62 1b 52 00 53 00 95 01 149W
// 77 07 01 00 10 07 00 ff 01 01 62 1b 52 00 53 00 8d 01 141W
// length is variable !!! (1 byte for small values)
//                                           52=8bit int 
// 77 07 01 00 10 07 00 ff 01 01 62 1b 52 00 52 76 01 01 
// 77 07 01 00 10 07 00 ff 01 01 62 1b 52 00 52 66 01 01

  // Power (in= pos. out= neg)
  byte sml_16_7_0[] {0x77, 0x07, 0x01, 0x00, 0x10, 0x07, 0x00, 0xff};
  _watt = int16_t(decodeSMLval(payload, sml_16_7_0, sizeof(sml_16_7_0), 15));

  debug_printf("[SML] Power:%d  1.8.0:%05.3f  2.8.0:%05.3f\r\n", _watt, _inputkWh, _outputkWh);

  /*
  String s1 = "SML:";
  s1 += String(_inputkWh);
  s1 += "kWh ";
  s1 += String(_watt);
  s1 += "W";
  //AsyncWebLog.println(s1);
  debug_println(s1);
  */
 
  return true;
}
#endif



