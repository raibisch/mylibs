#ifndef SMART_GRID_H
#define SMART_GRID_H

#include <Arduino.h>
#include <WiFiClientSecure.h>          // Build-in
#include <HTTPClient.h>
//#include <Preferences.h>

#include "fs_switch.h"

#include "ESP32ntp.h"
#include "XPString.h"



// Wandelt Source-Code Variablen-Nam varVALUE_i_ABL_minAmpere en in String um :-))
#define GETVARNAME(Variable) (#Variable)


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


   
 //#define SG_1DAYHOUR_SIZE 24
 #define SG_2DAYHOUR_SIZE 48
 #define SG_HOURSIZE SG_2DAYHOUR_SIZE

/* todo include in Class 
.. maybe with compiler switch
void initHistory()
{
   hist.begin("history", false); // use history
   ABL_Wh_Sum_old = hist.getULong64("whsum",0);
   ABL_Wh_Sum_akt = ABL_Wh_Sum_old;
   debug_printf("restored ABL_Wh_Sum_old= %d\r\n",ABL_Wh_Sum_old);
   SYS_RestartCount = hist.getInt("restart",0);
   SYS_TimeoutCount = hist.getInt("timeout",0);
   SYS_ChargeCount =  hist.getInt("charge",0);
   debug_printf("restored = %d\r\n",SYS_RestartCount);
}
*/
   
   #define DEFAULT_SGREADY_MODE 3 // bei alpha-innotec '3' sonst '2'
   struct SG_HOUR_DATA
   {
    int var1 = DEFAULT_SGREADY_MODE; // SG-ready-mode for heatpump
    //int var2 = 0;  // z.Z.nicht benoetigt
   };

   enum SG_RULEMODE
   {
    SG_RULE_FIX = 1,
    SG_RULE_EPEX_LOWHOUR,   
    SG_RULE_EPEX_HIGHHOUR, 
    SG_RULE_EPEX_LOWLIMIT,
    SG_RULE_EPEX_HIGHLIMIT,
    //SG_RULE_PV_MIN_KW,  //  not implemented
    //SG_RULE_BAT_MIN_KW  //  not implemented
    SG_RULE_UNVALID = 0xFF
   };


  #ifdef CALC_HOUR_ENERGYPRICE
  struct SG_HISTORY_PRICE_DATA
  {
    float sum = 0;
    //float flexprice_day =0;
    float month [12] {0};   // Month-index 0..11
    float monthday[32] {0}; // Day-index   1..31 
    float hour[24] {0};     // Hour-index  0..23
  };
 struct SG_HISTORY_WH_DATA
  {
    uint32_t sum = 0;
    //float flexprice_day =0;
    uint32_t month [12] {0};   // Month-index 0..11
    uint32_t monthday[32] {0}; // Day-index   1..31 !! 0 is unused !!
    uint32_t hour[24] {0};     // Hour-index  0..23
  };
#endif

class SmartGrid
{
  private:
   SmartGrid(); // private constructor

   bool _bUpdate= true;
   String sHourValueToday  = "";
   String sHourValueNext   = "";
   String sEPEXdateToday   = "00-00-0000";
   String sEPEXdateNext    = "00-00-0000";

  #ifdef CALC_HOUR_ENERGYPRICE
    // price in cent... spaeter als funktion
    float akt_flexprice = 0;
    float akt_fixprice  = 0;
    //float flexprice_sum = 0;
    //float flexprice_day =0;
    float flexprice_month [12] {0};
    float flexprice_monthday[32] {0};
    float flexprice_hour[24] {0};
    /// sollte ersetzt werden durch:


    //float fixprice_sum = 0;
    //float fixprice_day = 0;
    float fixprice_month[12] {0};
    float fixprice_monthday[32] {0};
    float fixprice_hour[24] {0};
    // ... sollte ersetzt werden durch:
    // fixprice wird dann aus den Wh-Werten mit fixprice berechnet !!
    // Vorteil: der fixprice kann nachträglich noch verändert werden !!
#endif
  
  protected:
    String _sHosturl= "";
    bool _isLoaded = false;
    struct tm *_looptime_now_tm = NULL;
   
  
    void GetSortedIndices(int16_t numbers[], int16_t indices[], uint16_t size);
    void GetSortedIndices2(int16_t numbers[], int16_t indices[], uint16_t begin, uint16_t end);

   // https-client (for EPEX API call)
  
   //Preferences nvsData;  //  by JG: new not testet  

   virtual float calcUserkWhPrice(float rawprice) {  /* define what to do*/ return 0;}; // override 
   virtual void  setAppOutputFromRules(uint8_t hour){ /* define what do do*/};           // override

  //#define SERVER_URL "api.energy-charts.info"
  //#define GET_STRING    "GET /price?bzn=DE-LU&start=2023-01-01T00%3A00%2B01%3A00&end=2023-01-01T23%3A45%2B01%3A00 HTTP/1.1"
  //#define GET_DAYAHEAD  "GET /price?bzn=DE-LU&start=%s-%s-%sT00%3A00%2B01%3A00&end=%s-%s-%sT23%3A45%2B01%3A00 HTTP/1.1"
  //#define HOST_STRING "HOST: api.engery-charts.info"
  public:
    SmartGrid(const char sEpex[]);
    ~SmartGrid ();
  
    bool init();
    bool calcSmartGridfromConfig(const char* psSGRule);
    virtual bool getAppRules(){return false;};         
    //void setSmartGridRules();
    bool setHourVar1(uint8_t hour, int var1);
    int  getHourVar1(uint8_t hour);

    // z.Z. nicht benoetigt
    //bool setHourVar2(uint8_t hour, int var1);
    //int  getHourVar2(uint8_t hour);

    void refreshWebData(bool b=true) { _bUpdate = b;};
    String getWebHourValueString(bool next);
    float getUserkWhPrice(uint8_t hour);
  
    String getWebDate(bool next);
    bool   downloadWebData(time_t* time_now, bool next);
    void   loop(time_t* time_now);
    //String getPVData(tm time);

   SG_HOUR_DATA smartgrid_hour[SG_HOURSIZE];

   int16_t unsorted_userhour_price[SG_HOURSIZE];
   int16_t sorted_epexhour_price[SG_HOURSIZE];

    // jetzt alles noch public ...
    virtual float getUserkWhFixPrice() {return 30.0;}; // override !
    void calcHourPrice(time_t* time_now, float akt_kwh);


  #ifdef CALC_HOUR_ENERGYPRICE
    //float getAktFixprice() {return akt_fixprice;};
    //float getAktFlexprice() {return akt_flexprice;};

    float getFlexprice_hour(uint8_t hour);
    float getFlexprice_monthday(uint8_t monthday);
    float getFlexprice_month(uint8_t month);

    float getFixprice_hour(uint8_t hour);
    float getFixprice_monthday(uint8_t monthday);
    float getFixprice_month(uint8_t month);

    bool saveCost(const char* filename, String & data);
    bool   loadCost_monthday();
    bool updateCost_monthday(uint8_t monthday);
  
    bool   loadCost_month();
    bool updateCost_month(uint8_t month);
 
    SG_HISTORY_PRICE_DATA sg_flexprice_history;
    SG_HISTORY_WH_DATA sg_wh_history;
    float hourprice_kwh_start = 0;
  #endif 
};

#endif

