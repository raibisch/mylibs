#include "SmartGrid.h"

//----------------------------------------------------public Functions----------------------------------------------------

//default constructor 
SmartGrid::SmartGrid(){} // private constructor

SmartGrid::SmartGrid(const char* url)
{
  _sHosturl.reserve(40);
  _sHosturl= url;

  //debug_printf("EPEX-URL: %s\rc\n", _sEPEXurl);
}

/*-----------------------------
destructor for class, not needed by Arduino but for complete class. Calls Arduino end function
*/
SmartGrid::~SmartGrid(){}

bool SmartGrid::init()
{
#ifdef CALC_HOUR_ENERGYPRICE
  loadCost_monthday();
  loadCost_month();
#endif 
  
  _isLoaded = true;
  getAppRules();
  refreshWebData(true);

  debug_println("SmartGrid-InitOK!");
  
  return true;
}

/// @brief parse String from config.txt
/// @param psSGRule 
bool SmartGrid::calcSmartGridfromConfig(const char* psSGRule)
{
   char buf1[30]; 
   XPString sgString(buf1, 30);
   char buf2[20];
   XPString sgRuleMode(buf2,20);

   sgString = psSGRule;
   uint8_t hour1; 
   uint8_t hour2;
   uint    var1, var2;
   SG_RULEMODE rule;

   debug_printf("\r\nSG-Rule-String: %s\r\n", sgString.c_str());
   if (sgString.length() < 10)
   {
    debug_printf("\r\n sgRuleMode-String not defined\r\n");
    return false;
   }
   sgString = psSGRule;
  
   XParts parts= sgString.split(",");
   hour1      = atol(parts.get_next());
   hour2      = atol(parts.get_next());
   var1       = atol(parts.get_next()); // SG-Ready Mode for heatpump !!
   var2       = atol(parts.get_next()); // cent for EPEX_LIMIT-Rules, kW for PV_LIMIT-Rules
   sgRuleMode = parts.get_next();

   //  example hour1=18(today), hour2=4(at next day)   --> hour1=18,hour2=28
   if (hour2 < hour1)
   {
    hour2 = hour2 + 24;
   }
   //debug_printf("RULE-String: %s\r\n", sgRuleMode.c_str());
   

   if ((hour1 >= 0 && hour1 < SG_HOURSIZE) && (hour2 >=0 && hour2 < SG_HOURSIZE) && (var1 > 0))
   {
      if (sgRuleMode.index_of("OFF") >=0)
      {
        // disable rule switching every hour
        debug_println("Rule OFF");
        rule = SG_RULE_OFF;
      }
      else
      if (sgRuleMode.index_of("FIX") == 0)
      {
        debug_println("RULE: FIX");
        rule = SG_RULE_FIX;
      }
      else 
      if (sgRuleMode.index_of("EPEX_HIGHHOUR") >=0)
      {
         debug_println("RULE: EPEX_HIGHHOUR");
          rule = SG_RULE_EPEX_HIGHHOUR;
      }
      else 
      if (sgRuleMode.index_of("EPEX_LOWHOUR") >= 0)
      {
           debug_println("RULE: EPEX_LOWHOUR");
         rule = SG_RULE_EPEX_LOWHOUR;
      } 
       else 
      if (sgRuleMode.index_of("EPEX_LOWLIMIT") >= 0)
      {
         debug_println("RULE: EPEX_LOWLIMIT");
         rule = SG_RULE_EPEX_LOWLIMIT;
      } 
      else 
      if (sgRuleMode.index_of("EPEX_HIGHLIMIT") >= 0)
      {
         debug_println("RULE: EPEX_HIGHLIMIT");
         rule = SG_RULE_EPEX_HIGHLIMIT;
      } 
      else
      {
        rule = SG_RULE_UNVALID;
        debug_println("*** unvalid SG rule")
       return false;
      }
   }
   else
   {
    debug_println("*** unvalid SG rule string")
    return false;
   }

   debug_printf("calcSGReadyTime hour1:%d hour2:%d mode=%d  var2:%d  rulemode:%d\r\n", hour1, hour2, var1, var2, uint(rule));
   if (rule == SG_RULE_OFF)
   {
     // no not switch every hour
     bRule_OFF = true;
     return true;
   }
   else
   if (rule == SG_RULE_FIX)
   {
    for (size_t i = hour1; i <= hour2; i++)
    {
       smartgrid_hour[i].var1 = var1;
    }  
   }
   else
   if ((rule >= SG_RULE_EPEX_LOWHOUR) && (rule <= SG_RULE_EPEX_HIGHLIMIT))
   {
    for (size_t i = hour1; i <=hour2; i++)
    {
         debug_printf("epex_price[%d]:%d  limit:%d\r\n",i,unsorted_userhour_price[i], var2);
         if ((rule == SG_RULE_EPEX_HIGHLIMIT) && (unsorted_userhour_price[i] > var2*10))
         {
           smartgrid_hour[i].var1 = var1; // =SGready Mode
         }
         else
         if ((rule == SG_RULE_EPEX_LOWLIMIT) && (unsorted_userhour_price[i] < var2*10))
         {
           smartgrid_hour[i].var1 = var1; // = SGready Mode
         }
    }
    
    int tmp_hour = 0;
    GetSortedIndices2(unsorted_userhour_price, sorted_epexhour_price, hour1, hour2);
    if (rule == SG_RULE_EPEX_HIGHHOUR)
    {
      if ((hour2 - hour1) > var2)
      {
        for (size_t i = (hour2-hour1); i > (hour2-hour1-var2); i--)
        {
          tmp_hour = sorted_epexhour_price[i];
          smartgrid_hour[tmp_hour].var1 = var1;
        }
      }
      //tmp_hour = sorted_epexhour_price[(hour2-hour1)];  // last element
      //smartgrid_hour[tmp_hour].var1 = var1;
    }
    else
    if (rule == SG_RULE_EPEX_LOWHOUR)
    {
      if ((hour2 - hour1) > var2)
      {
       for (size_t i = 0; i < var2; i++)
       {
          tmp_hour = sorted_epexhour_price[i];
          smartgrid_hour[tmp_hour].var1 = var1;
       }
      }
      //tmp_hour = sorted_epexhour_price[0]; // frist element
      //smartgrid_hour[tmp_hour].var1 = var1; 
    }
   }
   return true;
}


bool SmartGrid::setHourVar1(uint8_t hour, int var1)
{
  //uint8_t h = _pESP32ntp->getTimeInfo()->tm_hour;
  if (hour >= SG_HOURSIZE)
  {
    return false;
  }
  smartgrid_hour[hour].var1 = var1;
  //debug_printf("sgready_settings[%d].%d\r\n", hour, var1);
  return true;
}

int SmartGrid::getHourVar1(uint8_t hour)
{
  if (hour >= SG_HOURSIZE)
  {
    return 0;
  }
  return smartgrid_hour[hour].var1;
}


/* var2 z.Z. nicht benoetigt
bool SmartGrid::setHourVar2(uint8_t hour, int var2)
{
  //uint8_t h = _pESP32ntp->getTimeInfo()->tm_hour;
  if (hour >= 24)
  {
    return false;
  }
  smartgrid_hour[hour].var2 = var2;
  debug_printf("SmartGrid.SetHourVar2 hour[%d] var2[%d]\r\n", hour, var2);
  return true;
}

/*
int SmartGrid::getHourVar2(uint8_t hour)
{
  return smartgrid_hour[hour].var2;
}
*/

#ifdef CALC_HOUR_ENERGYPRICE

#define MAX_COST_FILE_SIZE 600

bool SmartGrid::saveCost(const char* filename, String  & data)
{
  fs::File cFile = myFS.open(filename, "w");
  if (!cFile) 
  {
    debug_println("ERROR: Failed to open config file for writing");
    return false;
  }
  cFile.print(data);
  cFile.close();
  debug_printf("%s saved!\r\n",filename);
 
  return true;
}

/// @brief  load from 'cost_akt_month.txt'
/// @return 
bool SmartGrid::loadCost_monthday()
{
  fs::File cFile = myFS.open("/cost_akt_month.txt", "r");
  if (!cFile) 
  {
    debug_println("Failed to open cost_akt_month.txt file");
    return false;
  }

  char cbuf[MAX_COST_FILE_SIZE];
  XPString cStr(cbuf,sizeof(cbuf));

  char cbuftmp[10];
  XPString ctmp(cbuftmp,sizeof(cbuftmp));

  char cbufline[20];
  XPString cLine(cbufline,sizeof(cbufline));

  cStr = cFile.readString();
  cFile.close();

  if (cStr.index_of("day") < 1)
  {
    debug_println("** ERROR: unvalid data file");
    return false;
  }

  uint ipos1,ipos2=0;
  // monthday 1..31 !!!
  for (size_t i = 1; i <= 31; i++)
  {
    ctmp = "";
    ctmp.format((char*)"\n%02d;", i);
    ipos1 = cStr.index_of(ctmp);
    // for extradebugging
    /*
    debug_print("HEX: ");
    for (size_t i = 0; i < 50; i++)
    {
        debug_printf("%02x ", cStr[i]);
    }
    */

    //debug_printf("\r\ncost Day:%s  = ",ctmp.c_str());
    
    if ((ipos1 > 0) && (ipos1 < MAX_COST_FILE_SIZE))
    { 
      cLine = "0;0;0;";
      ipos2 = cStr.index_of(";\r\n", ipos1+1);
      if (ipos2 == -1) // if vs-code store EOL = '\n' ...cost me many hours of debugging
      {
        ipos2 = cStr.index_of(";\n", ipos1+1);
      }

      //debug_printf("pos1:%d, pos2:%d \r\n", ipos1,ipos2);
      if ((ipos2 < MAX_COST_FILE_SIZE) && (ipos2 > ipos1))
      {
        cStr.substring(cLine,ipos1+4, ipos2-ipos1+1-3);
      }
      else
      {
        debug_print("*** ERROR: wrong index in line");
      }
      //debug_printf("%s",cLine.c_str());
      char spartsbuf[10];
      XPString sparts(spartsbuf,sizeof(spartsbuf));
      XParts parts= cLine.split(";");
      sparts = parts.get_next();
      //debug_printf("parts1: %s  ", sparts.c_str());
      fixprice_monthday[i]= atof(sparts);
      debug_printf("fixprice Day:%02d: %.2f \r\n",i ,fixprice_monthday[i]);

      sparts = parts.get_next();
      //debug_printf("parts2: %s  ", sparts.c_str());
      flexprice_monthday[i] = atof(sparts);
      debug_printf("flexprice Day:%02d: %.2f \r\n", i, flexprice_monthday[i]);
    }
    debug_println();
  }
  delay(500);
  return true;
}


bool SmartGrid::updateCost_monthday(uint8_t akt_day)
{
  char buftmp[20];
  XPString stmp(buftmp, sizeof(buftmp));

  String smdfile =  "\"day\";\"fixprice\";\"flexprice\";\r\n";
  for (size_t i = 1; i <= 31; i++)
  {
    stmp = "";
    stmp.format((char*)"%02d;", i);
    smdfile += stmp;
    stmp = "";
    stmp.format((char*)"%.2f;", getFixprice_monthday(i));
    smdfile += stmp;
    stmp = "";
    stmp.format((char*)"%.2f;\r\n", getFlexprice_monthday(i));
    smdfile += stmp;
  }
  
  debug_printf("updateCost_akt_month_day:\r\n", smdfile);
  return saveCost("/cost_akt_month.txt", smdfile);
  
  /*
  // todo: not testet !!! 
  // anstelle von dem da 
  File cFile = myFS.open("/cost_akt_month.txt", "w");
  if (!cFile) 
  {
    debug_println("ERROR: Failed to open 'cost_akt_month.txt' file for writing");
    return false;
  }
  cFile.print(smdfile);
  cFile.close();
  debug_println("cost_akt_month.txt save OK!");
  */
}


/// @brief  load from 'cost_year.txt'
/// @return 
bool SmartGrid::loadCost_month()
{
  debug_println ("-------loadCost_month");
  File cFile = myFS.open("/cost_akt_year.txt");
  if (!cFile) 
  {
    debug_println("Failed to open cost_akt_year.txt file");
    return false;
  }

  size_t size = cFile.size();
  if (size >= MAX_COST_FILE_SIZE)
  {
    debug_printf("*** ERROR: size of file to big:%d \n", size);
    return false;
  }

  char cbuf[MAX_COST_FILE_SIZE];
  XPString cStr(cbuf,sizeof(cbuf));

  char cbuftmp[10];
  XPString ctmp(cbuftmp,sizeof(cbuftmp));

  char cbufline[25];
  XPString cLine(cbufline,sizeof(cbufline));

  cStr = cFile.readString();
  cFile.close();

  uint ipos1,ipos2=0;
  for (size_t i = 0; i <= 11; i++)
  {
    ctmp = "";
    ctmp.format((char*)"\n%02d;", i);
    ipos1 = cStr.index_of(ctmp);
    //debug_printf("cost Month:%s  = ",ctmp.c_str());
    
    if ((ipos1 > 0) && (ipos1 < MAX_COST_FILE_SIZE))
    { 
      cLine = "0;0;0;";
      ipos2 = cStr.index_of(";\r\n", ipos1+1);
      if (ipos2 == -1) // if vs-code store EOL = '\n' ...cost me many hours of debugging
      {
        ipos2 = cStr.index_of(";\n", ipos1+1);
      }

      //debug_printf("pos1:%d, pos2:%d \r\n", ipos1,ipos2);
      if ((ipos2 < MAX_COST_FILE_SIZE) && (ipos2 > ipos1))
      {
        cStr.substring(cLine,ipos1+4, ipos2-ipos1+1-3);
      }
      else
      {
        debug_print("*** ERROR: wrong index in line");
      }
      //debug_printf("%s",cLine.c_str());
      char spartsbuf[10];
      XPString sparts(spartsbuf,sizeof(spartsbuf));
      XParts parts= cLine.split(";");
      sparts = parts.get_next();
      //debug_printf("parts1: %s  ", sparts.c_str());
      fixprice_month[i]= atof(sparts);
      debug_printf("fixprice month:%02d: %.2f \r\n",i+1 ,fixprice_month[i]);

      sparts = parts.get_next();
      //debug_printf("parts2: %s  ", sparts.c_str());
      flexprice_month[i] = atof(sparts);
      debug_printf("flexprice month:%02d: %.2f \r\n", i+1, flexprice_month[i]);
    }
    debug_println();
  }
  delay(500);
  return true;
}

/// @brief save to 'cost_akt_month.txt'
/// @return 
bool SmartGrid::updateCost_month(uint8_t month)
{
  char buftmp[20];
  XPString stmp(buftmp, sizeof(buftmp));

  String smdfile =  "\"month\";\"fixprice\";\"flexprice\";\r\n";
  for (size_t i = 0; i <= 11; i++)
  {
    stmp = "";
    stmp.format((char*)"%02d;", i);
    smdfile += stmp;
    stmp = "";
    stmp.format((char*)"%.2f;", getFixprice_month(i));
    smdfile += stmp;
    stmp = "";
    stmp.format((char*)"%.2f;\r\n", getFlexprice_month(i));
    smdfile += stmp;
  }
  debug_printf("updateCost_akt_year_month:\r\n");
  return saveCost("/cost_akt_year.txt", smdfile);
}

float SmartGrid::getFlexprice_hour(uint8_t h)
{
  return flexprice_hour[h];
}

float SmartGrid::getFlexprice_monthday(uint8_t d)
{
  if (d == _looptime_now_tm->tm_mday)
  {
    return flexprice_monthday[d] + akt_flexprice;
  }
  return flexprice_monthday[d];
}

float SmartGrid::getFlexprice_month(uint8_t m)
{
  if (m == _looptime_now_tm->tm_mon)
  { 
    return flexprice_month[m] + akt_flexprice;
  }
  return flexprice_month[m];
}

// Fixprice ... in Zukunft aus Wh und akt FixPrice bestimmen
float SmartGrid::getFixprice_hour(uint8_t h)
{
  return fixprice_hour[h];
}

float SmartGrid::getFixprice_monthday(uint8_t d)
{
  if (d == _looptime_now_tm->tm_mday)
  {
   return fixprice_monthday[d] + akt_fixprice;
  }
  return fixprice_monthday[d];
}

float SmartGrid::getFixprice_month(uint8_t m)
{
  if (m == _looptime_now_tm->tm_mon)
  {
   return fixprice_month[m] + akt_fixprice;
  }
  return fixprice_month[m];
}


////////////////////////////////////////////////////////////
/// @brief calc hour price
uint8_t  last_hour = 99;
uint8_t  last_day  = 99;
uint8_t last_month  = 99;


void SmartGrid::calcHourPrice(time_t* time, float akt_kwh)
{
  if ((akt_kwh < 100) || (akt_kwh >= 99999))
  {
    debug_println("*** calcHourPrice: no valid Wh value");
    return;
  }

  struct tm *now_tm = localtime(time);
  uint8_t akt_hour = now_tm->tm_hour;
  uint8_t akt_monthday = now_tm->tm_mday;
  uint8_t akt_month = now_tm->tm_mon;
 
  if (akt_monthday == 0)
  {
     debug_println("*** calcHourPrice: no valid time");
    return;
  }

  //debug_printf("akt_hour:%d last_hour:%d\r\n",akt_hour,last_hour); 
  if (last_hour != akt_hour) 
  {
    updateCost_monthday(akt_monthday);
    updateCost_month(akt_month);
    if (last_hour < 24)  // 24 is for 1-day history not for dayahead !
    {
     if (akt_hour==0)
     {
        for (size_t i = 0; i < 24; i++) // 24 for 1-day history
        {
        flexprice_hour[i] = 0;
        fixprice_hour[i]  = 0;
        }

        if ((akt_month == 0) && (akt_monthday == 1))
        {
          for (size_t m = 1; m < 32; m++)
          {
            flexprice_monthday[m] = 0;
            fixprice_monthday[m] = 0;
          }

          for (size_t j = 0; j < 12; j++)
          {
            flexprice_month[j] = 0;
            fixprice_month[j]  = 0;
          } 
        }
     }

     //flexprice_hour[akt_hour]         += flexprice_hour[last_hour];
     //flexprice_sum                    += flexprice_hour[last_hour];
     flexprice_monthday[akt_monthday] += flexprice_hour[last_hour];
     flexprice_month[akt_month]       += flexprice_hour[last_hour];

     //fixprice_hour[akt_hour]          += fixprice_hour[last_hour];
     //fixprice_sum                     += fixprice_hour[last_hour];
     fixprice_monthday[akt_monthday]  += fixprice_hour[last_hour];
     fixprice_month[akt_month]        += fixprice_hour[last_hour];

     hourprice_kwh_start = akt_kwh; 
     last_hour = akt_hour;
   }
   else
   {
      // init
      debug_println("*** init flexprice_hour and fix_price_hour");
      hourprice_kwh_start = akt_kwh;
      last_hour = akt_hour;
      for (size_t i = 0; i < 24; i++) // 24 for 1-day history
      {
        flexprice_hour[i] = 0;
        fixprice_hour[i]  = 0;
      }
   }
 } // last_hour != akt_hour

  /*
  debug_println("----------------------------------------");
  debug_printf("akt_hour:%d akt_day:%d akt_month:%d\r\n", akt_hour, akt_monthday, akt_month);

  debug_printf("Akt. kwh  : %.4f\r\n",akt_kwh);
  debug_printf("  wh_start: %.4f\r\n", hourprice_kwh_start);
  */
 
if (akt_kwh > hourprice_kwh_start)
{
    akt_flexprice = ((akt_kwh - hourprice_kwh_start) * getUserkWhPrice(akt_hour)) / 100;
    akt_fixprice  = ((akt_kwh  -hourprice_kwh_start) * getUserkWhFixPrice()) / 100;
   
    flexprice_hour[akt_hour] = akt_flexprice;
    fixprice_hour[akt_hour]  = akt_fixprice;
}
  //debug_printf("3 flexprice_wh_start: %f\r\n",hourprice_wh_start);
 
  /*
  debug_printf("hour kWh      : %.2f \r\n",akt_kwh - hourprice_kwh_start);
  debug_printf("hour price    : %.2f ct\r\n\r\n",getUserkWhPrice(akt_hour));
  debug_printf("akt_flexprice : %.2f Euro\r\n ",akt_flexprice);
  debug_printf("FlexPrice_hour: %.2f Euro\r\n ",flexprice_hour[akt_hour]);
  debug_printf("FlexPrice_day : %.2f Euro\r\n",flexprice_monthday[akt_monthday]);
  debug_printf("FlexPrice_sum : %.2f Euro\r\n",flexprice_month[akt_month]); 
  */

}
#endif


/// Idee:
// Alternative API
// https://api.awattar.de/v1/marketdata  (von jetzt +24h)
// https://api.awattar.de/v1/marketdata?start=1737068400000 (Start unix-timestamp milisec +24h)



/// ******* get actual Price form EPEX ****************************
/// @brief  open HTTP-Client GET data from "api.energy-charts.info"
bool SmartGrid::downloadWebData(time_t* t_now, bool next)
{
  WiFiClientSecure httpsclient; // 9.01.2025 jetzt hier local
  String sRawEPEX = "";
  //bool bNextDay = false;
  struct tm *now_tm = localtime(t_now);

  char output_day[11];
 
  // Set Data-Time String for GET
  // day-ahead !!
  if (next)
  {
    time_t nextday = *t_now + 86400;
    struct tm *nextday_tm = localtime(&nextday);
    strftime(output_day, sizeof(output_day), "%G-%m-%d", nextday_tm);
    sEPEXdateNext = output_day;
    debug_printf("\r\n--> downloadWebData nexday: %s\r\n", sEPEXdateNext);
  }
  // today
  else
  {
     strftime(output_day, sizeof(output_day), "%G-%m-%d", now_tm);
     sEPEXdateToday = output_day;
     debug_printf("\r\n--> downloadWebData today: %s\r\n", sEPEXdateToday);
  }
 
  //    ( datum aktuell ersetzten ! )
  //String sget = "GET /price?bzn=DE-LU&start=" + String(output_nextday) + "T00%3A00%2B01%3A00&end=" + String(output_nextday) + "T23%3A45%2B01%3A00 HTTP/1.1";
  // für test im Browser: 
  // https://api.energy-charts.info/price?bzn=DE-LU&start=2025-01-10  

  // neu nur den Tag eingeben
  String sget = "GET /price?bzn=DE-LU&start="; 
         sget += output_day;
         sget += " HTTP/1.1";
  //AsyncWebLog.println(sget);
  debug_println(sget);
  delay(500);
  
  httpsclient.setInsecure();
  //httpsclient.setTimeout(10); // notwendig ?? --> bringt nichts!
  debug_printf("\nStart connection to:  %s \r\n", _sHosturl.c_str());
  if (!httpsclient.connect(_sHosturl.c_str(), 443))
  {
    Serial.println("Connection failed!");
    return false;
  }
  else 
  {
    debug_printf("Connected to: %s \r\n",_sHosturl.c_str());
    httpsclient.println(sget);
    httpsclient.println("Host: api.energy-charts.info");
    //httpsclient.println("Connection: close");
    httpsclient.println();

    while (httpsclient.connected()) 
    {
      String line = httpsclient.readStringUntil('\n');
      if (line == "\r") {
        debug_println("headers received");
        break;
      }
    }
    sRawEPEX = "";
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (httpsclient.available()) 
    {
      char c = httpsclient.read();
      sRawEPEX += c;
      debug_write(c);
    }
   
    //httpsclient.println("Connection: close");
    //httpsclient.println();
    //httpsclient.flush();
    delay(200);

    httpsclient.stop();
    debug_println();
    int start_price = sRawEPEX.indexOf("price")+8;
    int end_price   = sRawEPEX.indexOf("]",start_price);

     /*
    {"license_info":"CC BY 4.0 (creativecommons.org/licenses/by/4.0) from Bundesnetzagentur | SMARD.de",
    "unix_seconds":[1714773600,1714777200,1714780800,1714784400,1714788000,1714791600,1714795200,1714798800,1714802400,1714806000,1714809600,1714813200,1714816800,1714820400,1714824000,1714827600,1714831200,1714834800,1714838400,1714842000,1714845600,1714849200,1714852800,1714856400],
    "price":[90.8,87.79,86.26,87.72,90.64,98.34,94.19,90.02,78.48,62.38,41.17,31.18,24.93,14.53,13.34,26.03,63.62,79.36,98.01,121.92,135.4,113.56,90.26,83.6],
    "unit":"EUR/MWh","deprecated":false}
    */
    
    // by JG:  Anpassung für 48h 
    if (next)
    {
       sHourValueNext = sRawEPEX.substring(start_price, end_price);
       if (((sHourValueNext.indexOf("t available")) > 0) || (sHourValueNext.length() < 100))
       {
         sHourValueNext = "999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999";
       }
       int pos1 = 0;
       int pos2 = 0;
      for (size_t i = 24; i < 48; i++) // Achtung hier 24 !!
      {
        pos2 = sHourValueNext.indexOf(',', pos1+1);
        //String s = epex.substring(pos1,pos2);

        //debug_printf("EPEX-String:pos1=%d, pos2=%d  :",pos1,pos2);
        //debug_println(s);
        //debug_printf("EPEX-String %d: %s\r\n",i, s.c_str());
        float fprice =  sHourValueNext.substring(pos1,pos2).toFloat(); 
        //debug_printf("Epex-Raw  hour:%d:  price: %f\r\n",i,fprice);
        float fpriceUser = calcUserkWhPrice(fprice);
        //fprice =  ((fprice / 10.0) + fprice*0.019) + 17.51;
        unsorted_userhour_price[i] = int16_t(fpriceUser*10); // wegen Rundungsfehler *10!
        pos1 = pos2+1;
      }
    }
    else
    {
      sHourValueToday = sRawEPEX.substring(start_price, end_price);
      sHourValueNext = "";
      int pos1 = 0;
      int pos2 = 0;
      
      /*
      if (now_tm->tm_hour < 14)
      {
         sHourValueNext = "";
         for (size_t i = 24; i < SG_HOURSIZE; i++)
         {
          sHourValueNext += "999";
          if (i < SG_HOURSIZE-1)
          {sHourValueNext += ",";};
          unsorted_userhour_price[i] = 999;
          setHourVar1(i,0);
         }
      }
      */
     
      for (size_t i = 0; i < 24; i++) // Achtung hier 24 !!
      {
        pos2 = sHourValueToday.indexOf(',', pos1+1);
        //String s = epex.substring(pos1,pos2);

        //debug_printf("EPEX-String:pos1=%d, pos2=%d  :",pos1,pos2);
        //debug_println(s);
        //debug_printf("EPEX-String %d: %s\r\n",i, s.c_str());
        float fprice =  sHourValueToday.substring(pos1,pos2).toFloat(); 
        //debug_printf("Epex-Raw  hour:%d:  price: %f\r\n",i,fprice);
        float fpriceUser = calcUserkWhPrice(fprice);
        //fprice =  ((fprice / 10.0) + fprice*0.019) + 17.51;
        unsorted_userhour_price[i] = int16_t(fpriceUser*10); // wegen Rundungsfehler *10!
        pos1 = pos2+1;
      }
    }
 }
 return true;
}


String SmartGrid::getWebHourValueString(bool next)
{
 
  if (next)
  {

    return sHourValueNext;
  }
  else
  {
    return sHourValueToday;
  }
  
 }


String SmartGrid::getWebDate(bool next)
{
  if (next)
  {
    return  sEPEXdateNext;
  }
  else
  {
    return sEPEXdateToday;
  }
}

float SmartGrid::getUserkWhPrice(uint8_t hour)
{
  if (hour < SG_HOURSIZE)
  {
   float f = unsorted_userhour_price[hour] / 10.0;
  //debug_printf("UserkWhPrice_raw: hour,%d, price:%d\r\n",hour, unsorted_epexhour_price[hour]);
  //debug_printf("UserkWhPrice*10 : hour,%f, price:%f\r\n",hour, f);
   return f;
  }
  else
  {
    return 0;
  }
}

void SmartGrid::loop(time_t* time_now)
{
    _looptime_now_tm = localtime(time_now);
     // get new EPEX-Data at 15:01
    if ((_looptime_now_tm->tm_hour == 15) && (_looptime_now_tm->tm_min==1) && (_looptime_now_tm->tm_sec < 7))
    { 
      refreshWebData(true);
      delay(1000);
      return;
    }

    // get next EPEX-Data at midnight
    if ((_looptime_now_tm->tm_hour == 00) && (_looptime_now_tm->tm_min==02) && (_looptime_now_tm->tm_sec < 7))
    {
      // neu 14.9.2024
      refreshWebData(true);
  #ifdef CALC_HOUR_ENERGYPRICE
      updateCost_monthday(_looptime_now_tm->tm_mday); 
      updateCost_month(_looptime_now_tm->tm_mon);
  #endif
      delay(1000);
      return;
    }

    // every hour;
    // switch SG-ready relay
    if ((_looptime_now_tm->tm_min==00) && (_looptime_now_tm->tm_sec < 7))
    {
      _bUpdate = false;
      if (this->sHourValueToday.indexOf("999"))
      {_bUpdate = true;}
      if (this->sHourValueNext.indexOf("999") && _looptime_now_tm->tm_hour > 15)
      {_bUpdate = true;}
      
      if (!bRule_OFF)
      {
       setAppOutputFromRules(_looptime_now_tm->tm_hour);
      }
      delay(1000);
      return;
    }
    
    if (_bUpdate) // extra Bool for init at startup
    {
      _bUpdate = false;
      debug_println("Read EPEX");
      _bUpdate = !downloadWebData(time_now,false); // akt. day
      delay(800);
      downloadWebData(time_now, true); // day-ahead hat nicht immer daten !!!
      
      if (_bUpdate) // return false from donloadWEbData !
      {
        debug_println("no response from EPEX !!");
        delay(800);
      }

      getAppRules(); //aus Config.txt daten auswerten und parameter setzten
      if (!bRule_OFF)
      {
       setAppOutputFromRules(_looptime_now_tm->tm_hour);
      }

    }
  
}

/// @privatesection private members

void SmartGrid::GetSortedIndices(int16_t numbers[], int16_t indices[], uint16_t size)
{
  u16_t i , j;
  for ( i = 0 ; i < size ; i++ )
  {
    for (j = i; (j>0) && (numbers[ indices [j-1] ] > numbers [i]) ; j-- )
    {
      indices [ j ] = indices [ j - 1 ];
    }
    indices [ j ] = i;
  }
}

void SmartGrid::GetSortedIndices2(int16_t numbers[], int16_t indices[], uint16_t begin, uint16_t end)
{
  int16_t tmp[SG_HOURSIZE];
  uint16_t j =0;

  for (uint16_t i = begin; i <= end; i++)
  {
    tmp[j] = numbers[i];
  
    j++;
  }
  debug_printf("unsort begin:%d - end:%d :    ", begin, end);
  //printArray(tmp, (end-begin)+1);
  GetSortedIndices(tmp, indices, (end - begin)+1);
  for (uint16_t i = 0; i <= ((end - begin)); i++)
  {
    indices[i] = indices[i]  + begin;
  }
}











