#include "LuxWebsocket.h"

// Wandelt Source-Code Variablen-Name in String um :-))
#define GETVARNAME(Variable) (#Variable)

#define LUXPRINT_BUF_LEN 100
char luxprintbuf[LUXPRINT_BUF_LEN];

#define WSRX_BUF_LEN 8600
char wsbufrx[WSRX_BUF_LEN]; 
XPString luxRxString(wsbufrx, WSRX_BUF_LEN);

#define WSTX_BUF_LEN 20
char wsbuftx[WSTX_BUF_LEN];
XPString luxTxString(wsbuftx, WSTX_BUF_LEN);

#define LUXID_BUF_LEN 12
char luxbufid[LUXID_BUF_LEN];
XPString luxIdString(luxbufid, LUXID_BUF_LEN);

#define LUXVALUE_BUF_LEN 25
#define LUXDATE_BUF_LEN  30

char _bufurl[16];
XPString _url(_bufurl, sizeof(_bufurl));
char _bufpassword[32];
XPString _password(_bufpassword, sizeof(_bufpassword));
char _bufuser[32];
XPString _user(_bufuser, sizeof(_bufuser));

// only way to define static in header --> init in cpp-file !!
WS_POLL_STATUS       LuxWebsocket::_poll_state  = WS_POLL_STATUS::NOT_CONNECTED;
WStype_t             LuxWebsocket::_wstype      = WStype_t::WStype_DISCONNECTED;
LUX_HEATPUMP_STATUS  LuxWebsocket::_hp_state    = LUX_HEATPUMP_STATUS::OFF;

//----------------------------------------------------public Functions----------------------------------------------------

/// @brief default constructor
LuxWebsocket::LuxWebsocket()
{   
}

/// @brief destructor for class, not needed by Arduino but for complete class. Calls Arduino end function
LuxWebsocket::~LuxWebsocket()
{
}

/// @brief 
/// @param type 
/// @param payload 
/// @param length 
void LuxWebsocket::wsEvent(WStype_t type, uint8_t * payload, size_t length) 
{
  _wstype = type;
	switch(type) {
		case WStype_DISCONNECTED: 
      //AsyncWebLog.println("[Websocket] Disconnected!");
      debug_println("[WS] Disconnected")
      _poll_state = NOT_CONNECTED;
			break;
		case WStype_CONNECTED:
      //AsyncWebLog.println("\r\n[Websocket] connected");
      debug_println("\r\n[WS] connected!");
      _poll_state = SEND_LOGIN;
			break;
		case WStype_TEXT:
      luxRxString = (char*) payload;
			// der gesammte payload im debug_print !!
      //debug_printf("[Websocket] get text: %s\r\n", payload);
      XMLParser();
			break;
    default:
		//case WStype_BIN:
      // no binary Data from Luxtronic
		//case WStype_ERROR:			
		//case WStype_FRAGMENT_TEXT_START:
		//case WStype_FRAGMENT_BIN_START:
		//case WStype_FRAGMENT:
		//case WStype_FRAGMENT_FIN:
      debug_printf("Websocket ERROR: %d", type);
      //AsyncWebLog.println("[WSc] **ERROR**: " + String(type));
      _poll_state = NOT_CONNECTED;
		 break;
	}
}

/// @brief 
/// @return 
bool LuxWebsocket::wsConnect()
{
  debug_print("[WS] start connect ");
  if (_poll_state == NOT_CONNECTED)
  {  
     LUX_LOG_STRING2("connect to", _url.c_str());
     //AsyncWebLog.println("connect to:" + sIP);
	   _wsClient.begin(_url.c_str(), 8214, "/", "Lux_WS");
     _poll_state = SEND_LOGIN;
    return true;
  }
  return false;
}

bool LuxWebsocket::init(const char * url,  const char* password)
{
  _url = url;
  _password  = password;
  _hp_state   = LUX_HEATPUMP_STATUS::OFF;
  _poll_state = WS_POLL_STATUS::NOT_CONNECTED;
	// event handler
	_wsClient.onEvent(wsEvent);
	//_wsClient.setReconnectInterval(5000);
  //_wsClient.enableHeartbeat(6000,500,2);
  wsConnect();
  debug_println("[WS] init OK!");
  return true;
}

void LuxWebsocket::setpolling(bool on)
{
  if (_poll_state < WS_POLL_STATUS::IS_CONNECTED)
  return;

  if (on)
  {
     _poll_state = WS_POLL_STATUS::SEND_LOGIN;
  }
  else
  {
    _poll_state = WS_POLL_STATUS::NO_POLLING;
  }
}

/// @brief buffer for fetch message
char csvfetchbuffer [200];
XPString sCSVfetch(csvfetchbuffer, sizeof(csvfetchbuffer));

/// @brief CSV-Message for html fetch
/// @param all  --> LONG OR SHORT MESSAGE
/// @return 
const char* LuxWebsocket::getCSVfetch(bool all)
{
  sCSVfetch =  getval(LUX_VAL_TYPE::BETRIEBSZUSTAND,true);      // 3
  sCSVfetch += getval(LUX_VAL_TYPE::TEMP_WW_IST, true);         // 4
  sCSVfetch += getval(LUX_VAL_TYPE::TEMP_AT_IST, true);         // 5
  sCSVfetch += getval(LUX_VAL_TYPE::TEMP_RL_IST, true);         // 6
  sCSVfetch += getval(LUX_VAL_TYPE::TEMP_RL_SOLL, true);        // 7
  sCSVfetch += getval(LUX_VAL_TYPE::TEMP_VL_IST, true);         // 8

  sCSVfetch += getval(LUX_VAL_TYPE::POWER_IN, true);            // 9
  sCSVfetch += getval(LUX_VAL_TYPE::POWER_OUT, true);           // 10
 
  sCSVfetch += getval(LUX_VAL_TYPE::ENERGY_IN_HE, true);        // 11
  sCSVfetch += getval(LUX_VAL_TYPE::ENERGY_IN_WW, true);        // 12
  sCSVfetch += getval(LUX_VAL_TYPE::ENERGY_IN_SUM, true);       // 13

  sCSVfetch += getval(LUX_VAL_TYPE::ENERGY_OUT_HE, true);       // 14
  sCSVfetch += getval(LUX_VAL_TYPE::ENERGY_OUT_WW, true);       // 15
  sCSVfetch += getval(LUX_VAL_TYPE::ENERGY_OUT_SUM, true);      // 16
 
  sCSVfetch += getval(LUX_VAL_TYPE::COP_DAY_HE, true);          // 17
  sCSVfetch += getval(LUX_VAL_TYPE::COP_DAY_WW, true);          // 18
  sCSVfetch += getval(LUX_VAL_TYPE::COP_DAY_SUM, true);         // 19
 
  sCSVfetch += getval(LUX_VAL_TYPE::PUMP_FLOW, true);           // 20 Pumpe l/h
  sCSVfetch += getval(LUX_VAL_TYPE::PUMP_PWM, true);            // 21 Pumpe PWM
  sCSVfetch += getval(LUX_VAL_TYPE::OUTPUT_WW, true);           // 22 WW-Ventil
  sCSVfetch += getval(LUX_VAL_TYPE::OUTPUT_VD_HEIZ, true);      // 23 VD-Heiz-ON
 
  sCSVfetch += getval(LUX_VAL_TYPE::VD_RPM, true);              // 24 Verdichter RPM
  sCSVfetch += getval(LUX_VAL_TYPE::TEMP_VD_IN, true);          // 25 Verdichter Ansaug Temp
  sCSVfetch += getval(LUX_VAL_TYPE::ND_PRESSURE, true);         // 26 Verdichter ND Bar 
  sCSVfetch += getval(LUX_VAL_TYPE::HD_PRESSURE, true);         // 27 Verdichter HD Bar 
  sCSVfetch += getval(LUX_VAL_TYPE::EEV_HEAT, true);            // 28 EEV Heizen %
  sCSVfetch += getval(LUX_VAL_TYPE::EEV_COOL, true);            // 29 EEV Kuehlen %

  sCSVfetch += getval(LUX_VAL_TYPE::DEFROST_PERCENT, true);     // 30 Abtaubedarf %
  sCSVfetch += getval(LUX_VAL_TYPE::VENT_RPM, true);            // 31 Ventilator RPM
  sCSVfetch += getval(LUX_VAL_TYPE::TEMP_WQ_IN, true);          // 32 Temp Waerme-Quelle Ein 
  sCSVfetch += getval(LUX_VAL_TYPE::DEFROST_LASTTIME, true);    // 33 Letzte Abtauung

  sCSVfetch += getval(LUX_VAL_TYPE::WP_ONTIME, true);           // 34 WP-Ontime String "hh:mm:ss"
  sCSVfetch += getval(LUX_VAL_TYPE::TEMP_VD_HEI, true);         // 35 VD Heiz. Temp 
  sCSVfetch += getval(LUX_VAL_TYPE::VD_RPM_SOLL, true);         // 36 VD RPM Soll

  return sCSVfetch.c_str();
 
}

char valbuffer [18];
XPString sVal(valbuffer, sizeof(valbuffer));
const char* LuxWebsocket::getval(LUX_VAL_TYPE valtype, bool bAddComma)
{
   sVal = "--";
   if (valtype < _LUX_TYPE_XMLVAL_MAX)
   {
     sVal = _idvalues[valtype].sValue;
   }
   
   switch (valtype)
   { 
   // intern calculated:
   case COP_SUM:
      sVal = (energyOutSum) / float(energyInSum);
      break;
   case COP_HE:
      sVal = (energyOutHE) / float(energyInHE);
      break;
   case COP_WW:
      sVal = (energyOutWW) / float(energyInWW);
       break;
   case COP_DAY_SUM:
      sVal = COPdaySumLux;
      break;
   case COP_DAY_HE:
      sVal = COPdayHE;
      break;
   case COP_DAY_WW:
     sVal = COPdayWW;
     break;
   case COP_ACTUAL:
     sVal = atof(_idvalues[POWER_OUT].sValue) / atof(_idvalues[POWER_IN].sValue);
     break;
   // webservice polling status
   case STATUS_POLL:  
     switch (_poll_state)
     {
       case WS_POLL_STATUS::SEND_LOGIN:
         sVal = "LOGIN";
         break;
       case WS_POLL_STATUS::SEND_GET_INFO:
        sVal = "GET_INFO";
        break;
       case WS_POLL_STATUS::SEND_REFRESH:
        sVal = "REFRESH";
        break;
       case WS_POLL_STATUS::NO_POLLING:
        sVal = "NO_POLLING";
        break;
       default:
        sVal = "NOT_CONNECTED";
        break;
     }
     break;
   // heatpump status
   case STATUS_HEATPUMP:
     switch (_hp_state)
     {
     case LUX_HEATPUMP_STATUS::OFF:
       sVal = "AUS";
       break;
     case LUX_HEATPUMP_STATUS::HEATING:
       sVal = "HEIZUNG";
       break;
     case LUX_HEATPUMP_STATUS::WW:
       sVal = "WARMWASSER";
       break;
     case LUX_HEATPUMP_STATUS::DEFROST:
       sVal = "ABTAUEN";
       break;
     default:
       sVal = "status?";
       break;
     }
     break;
   default:
     break;
   }
   
   if (bAddComma)
   {
    sVal+= ",";
   }
   return sVal.c_str();
}

 float LuxWebsocket::getvalf(LUX_VAL_TYPE valtype)
 {
   return atof(getval(valtype, false));
 }

 uint LuxWebsocket::getvalui(LUX_VAL_TYPE valtype)
 {
   return atoi(getval(valtype, false));
 }

/* private functions */

/// @brief calculate kwh values and save energy per day
/// @param bDayOld 
void LuxWebsocket::calcDayEnergyValues(bool bNewDay)
{
   energyInSum = atof(_idvalues[ENERGY_IN_SUM].sValue) * 1000;
   energyOutSum= atof(_idvalues[ENERGY_OUT_SUM].sValue)* 1000;

   energyInWW  = atof(_idvalues[ENERGY_IN_WW].sValue)  * 1000;
   energyOutWW = atof(_idvalues[ENERGY_OUT_WW].sValue) * 1000;

   energyInHE  = atof(_idvalues[ENERGY_IN_HE].sValue)  * 1000;
   energyOutHE = atof(_idvalues[ENERGY_OUT_HE].sValue) * 1000;

   if (energyInSum == 0)
   {
    return;
   }

   if (bNewDay || energyOldInSum == 0)
   {
    energyOldInSum    = energyInSum;
    energyOldOutSum   = energyOutSum;
    
    energyOldInWW     = energyInWW;
    energyOldOutWW    = energyOutWW;
    
    energyOldInHE     = energyInHE;
    energyOldOutHE    = energyOutHE;
   } 
}


/// @brief  calculate COP of actual day
void LuxWebsocket::calcCopDay()
{  
   if (energyInSum > 0) //luxtronic data availible
   {
     if (energyInSum > energyOldInSum)
     {
       COPdaySumLux = (energyOutSum - energyOldOutSum) / float(energyInSum - energyOldInSum);
     }

     if (energyInWW > energyOldInWW)
     {
      COPdayWW = (energyOutWW - energyOldOutWW) / float(energyInWW - energyOldInWW);
     }
     //debug_printf("COP-DAY-WW : %.2f\r\n",luxdata.COPdayWW);
     //AsyncWebLog.println("COP-Day-WW: " + String(luxdata.COPdayWW,2));

     if (energyInHE > energyOldInHE)
     {
      COPdayHE = (energyOutHE - energyOldOutHE) / float(energyInHE - energyOldInHE);
      //debug_printf("COP-DAY-HE: %.2f\r\n",luxdata.COPdayHE); 
     }
   }
}


char _tmpfindbuf[50];
XPString _tmpfind(_tmpfindbuf, 50);
bool LuxWebsocket::XMLGETInfo(const char* cpFind, LUX_VAL_TYPE valtype, int endoffset)
{
  _tmpfind = cpFind;
  XPString xpID(_idvalues[valtype].sID, 11);
  XPString xpValue(_idvalues[valtype].sValue, sizeof(ID_VALUE_PAIR::sValue)-1);
  bool bRet =  luxRxString.substringBeetween(xpValue,_tmpfind.c_str(), _tmpfind.length(), "</value>", endoffset);
  int pos1 = luxRxString.index_of(cpFind);
  bRet = luxRxString.substringBeetween(xpID, pos1-30, "<item id=", 10, cpFind, -8);
#ifdef DEBUG_PRINT
  _tmpfind.substringEndwith(_tmpfind, 0, "</name>");
   if (bRet > 0)
   {
     Serial.printf("[LUX] %22s:\t ID: %9s   Value:%8s\r\n", _tmpfind.c_str(), xpID.c_str(), xpValue.c_str());
   }
   else { debug_printluxvalerror(_tmpfind.c_str());}  
#endif
 
  return bRet;
}

bool LuxWebsocket::XMLGETInfo(int pos0, const char* cpFind, LUX_VAL_TYPE valtype, int endoffset)
{
  _tmpfind = cpFind;
  XPString xpID(_idvalues[valtype].sID, 13);
  xpID.reset();
  XPString xpValue(_idvalues[valtype].sValue, 15);
  bool bRet =    luxRxString.substringBeetween(xpValue, pos0, _tmpfind.c_str(), _tmpfind.length(), "</value>", endoffset);
#ifdef DEBUG_PRINT
  if (!bRet)
  {
     int pos1 = luxRxString.index_of(cpFind, pos0);
     int pos2 = luxRxString.index_of("</value>, pos1");
     Serial.printf("[LUX] ERROR XMLGETInfo pos0:%d  pos1:%d, pos2:%d %22s:\t   Value:%18s\r\n", pos0, pos1, pos2,_tmpfind.c_str(), xpValue.c_str());
  }
#endif
  int pos1 = luxRxString.index_of(cpFind, pos0);
  bRet = luxRxString.substringBeetween(xpID, pos1-30, "<item id=", 10, cpFind, -8);
#ifdef DEBUG_PRINT
  _tmpfind.substringEndwith(_tmpfind, 0, "</name>");
   if (bRet > 0)
   {
     Serial.printf("[LUX] %22s:\t ID: %9s   Value:%8s\r\n", _tmpfind.c_str(), xpID.c_str(), xpValue.c_str());
   }
   else { debug_printluxvalerror(_tmpfind.c_str());}  
#endif

  return bRet;
}



bool  LuxWebsocket::XMLREFRESH(LUX_VAL_TYPE valtype, int endoffset)
{
  bool bRet = false;
  int pos0 = luxRxString.index_of(_idvalues[valtype].sID);
  XPString xpValue(_idvalues[valtype].sValue, sizeof(ID_VALUE_PAIR::sValue)-1);
#ifdef DEBUG_PRINT
  switch (valtype)
  {
     case LUX_VAL_TYPE::TEMP_VL_IST:
      _tmpfind = GETVARNAME(TEMP_VL_IST);
      break;
      case LUX_VAL_TYPE::TEMP_RL_IST:
      _tmpfind = GETVARNAME(TEMP_RL_IST);
      break;
      case LUX_VAL_TYPE::TEMP_RL_SOLL:
      _tmpfind = GETVARNAME(TEMP_RL_SOLL);
      break;
      case LUX_VAL_TYPE::TEMP_WW_IST:
      _tmpfind = GETVARNAME(TEMP_WW_IST);
      break;
      case LUX_VAL_TYPE::TEMP_AT_IST:
      _tmpfind = GETVARNAME(TEMP_AT_IST);
      break;
      case LUX_VAL_TYPE::TEMP_AT_MID:
      _tmpfind = GETVARNAME(TEMP_AT_MID);
      break;
      case LUX_VAL_TYPE::TEMP_HEGAS:
      _tmpfind = GETVARNAME(TEMP_HEGAS);
      break;
      case LUX_VAL_TYPE::TEMP_VD_HEI:
      _tmpfind = GETVARNAME(TEMP_VD_HEI);
      break;
      case LUX_VAL_TYPE::TEMP_WQ_IN:
      _tmpfind = GETVARNAME(TEMP_WQ_IN);
      break;
      case LUX_VAL_TYPE::TEMP_VD_IN:
      _tmpfind = GETVARNAME(TEMP_VD_IN);
      break;

   /* VD_RPM,
      VD_RPM_SOLL,
      VD_EEV,
      VD_ND,
      VD_HD,
      PUMP_FLOW,     
      ND_PRESSURE,
      ND_PRESSURE,
    */
      case LUX_VAL_TYPE::VD_RPM:
       _tmpfind = GETVARNAME(VD_RPM);
       break;
       case LUX_VAL_TYPE::VD_RPM_SOLL:
       _tmpfind = GETVARNAME(VD_RPM_SOLL);
       break;
      case LUX_VAL_TYPE::EEV_HEAT:
       _tmpfind = GETVARNAME(EEV_HEAT);
       break;
       case LUX_VAL_TYPE::EEV_COOL:
       _tmpfind = GETVARNAME(EEV_COOL);
       break;
      case LUX_VAL_TYPE::PUMP_FLOW:
       _tmpfind = GETVARNAME(PUMP_FLOW);
       break;
        case LUX_VAL_TYPE::HD_PRESSURE:
       _tmpfind = GETVARNAME(HD_PRESSURE);
       break;
        case LUX_VAL_TYPE::ND_PRESSURE:
       _tmpfind = GETVARNAME(ND_PRESSURE);
       break;
     
/*    OUTPUT_DEFROST,
      OUTPUT_VD_HEIZ,
      OUTPUT_WW,
      OUTPUT_ZWE1,
      PUMP_PWM,
      VENT_RPM,
*/
      case LUX_VAL_TYPE::OUTPUT_DEFROST:
       _tmpfind = GETVARNAME(OUTPUT_DEFROST);
      break;
      case LUX_VAL_TYPE::OUTPUT_VD_HEIZ:
       _tmpfind = GETVARNAME(OUTPUT_VD_HEIZ);
      break;
      case LUX_VAL_TYPE::OUTPUT_WW:
       _tmpfind = GETVARNAME(OUTPUT_WW);
      break;
      case LUX_VAL_TYPE::OUTPUT_ZWE1:
       _tmpfind = GETVARNAME(OUTPUT_ZWE1);
      break;
      case LUX_VAL_TYPE::PUMP_PWM:
       _tmpfind = GETVARNAME(PUMP_PWM);
      break;
       case LUX_VAL_TYPE::VENT_RPM:
       _tmpfind = GETVARNAME(VENT_RPM);
      break;
       case LUX_VAL_TYPE::WP_ONTIME:
       _tmpfind = GETVARNAME(WP_ONTIME);
      break;

      // Energy
      case LUX_VAL_TYPE::ENERGY_IN_SUM:
      _tmpfind = GETVARNAME(ENERGY_IN_SUM);
       break;
      case LUX_VAL_TYPE::ENERGY_OUT_WW:
       _tmpfind = GETVARNAME(ENERGY_OUT_WW);
       break;
      case LUX_VAL_TYPE::ENERGY_OUT_SUM:
       _tmpfind = GETVARNAME(ENERGY_OUT_SUM);
       break;
      case LUX_VAL_TYPE::ENERGY_IN_HE:
       _tmpfind = GETVARNAME(ENERGY_IN_HE);
       break;
      case LUX_VAL_TYPE::ENERGY_OUT_HE:
       _tmpfind = GETVARNAME(ENERGY_OUT_HE);
       break;
      case LUX_VAL_TYPE::ENERGY_IN_WW:
       _tmpfind = GETVARNAME(ENERGY_IN_WW);
       break;

      // Anlage
      case LUX_VAL_TYPE::BETRIEBSZUSTAND:
      _tmpfind = GETVARNAME(BETRIEBSZUSTAND);
      break; 
      case LUX_VAL_TYPE::POWER_OUT:
       _tmpfind = GETVARNAME(POWER_OUT);
       break;
       case LUX_VAL_TYPE::POWER_IN:
      _tmpfind = GETVARNAME(POWER_IN);
       break;
      case LUX_VAL_TYPE::DEFROST_PERCENT:
      _tmpfind = GETVARNAME(DEFROST_PERCENT);
       break;
      case LUX_VAL_TYPE::DEFROST_LASTTIME:
       _tmpfind = GETVARNAME(DEFROST_LASTTIME);
        break;

      default:
      _tmpfind = "enum:";
      _tmpfind += valtype;
      break;
     }
     //debug_printf("[LUX] pos0=%d valtype: %s ID:%s\r\n",pos0, _tmpfind.c_str(), _idvalues[valtype].sID);
 #endif    
 
  bRet =  luxRxString.substringBeetween(xpValue, pos0,  "<value>", 7 /*size of <value> -1*/, "</value>", endoffset);
  
  // Sonderbehandlung für einige Werte
  switch (valtype)
  {
  case BETRIEBSZUSTAND:
     if (xpValue.index_of("HEI") >= 0)
     {
        _hp_state = LUX_HEATPUMP_STATUS::HEATING;
     }
     else
     if (xpValue.index_of("WW") >= 0)
     {
       _hp_state = LUX_HEATPUMP_STATUS::WW;
     }
     else
     if (xpValue.index_of("ABT") >= 0)
     {
       _hp_state = LUX_HEATPUMP_STATUS::DEFROST;
     }
     else 
     {
       _hp_state = OFF;
     }
     break;
/*  ... for extra debugging
     if (_idvalues[OUTPUT_WW].sValue[0] == 'A')
     {
        debug_printf("[***] OUTPUT_WW  value:%s", _idvalues[OUTPUT_WW].sValue);
     }
   
    debug_printf("  POWER_OUT value:%s   _hp_state %d\r\n",xpValue.c_str(), _hp_state);
*/
  default:
    break;
  }
   
#ifdef DEBUG_PRINT
   if (bRet)
   {
     Serial.printf("[LUX] %22s:\t ID: %9s   Value:%18s\r\n", _tmpfind.c_str(), _idvalues[valtype].sID, xpValue.c_str());
   }
   else { debug_printluxvalerror(_tmpfind.c_str());}   
#endif

  return bRet;
}
/// **********************************************************************
/// @brief parse Luxtronik Webservice (XML with strange structure ;-))
/// ACHTUNG: der Parser ist sprachabhaenig (hier: Deutsch) !! 
/// **********************************************************************
inline void LuxWebsocket::XMLParser()
{
  int xml_pos0;
  bool bRet = false;

  switch (_poll_state)
  {
  case SEND_LOGIN:
    xml_pos0 = luxRxString.index_of(LUX_XMLDEF_INFO);
    if (luxRxString.index_of(LUX_XMLDEF_INFO) > 0)
    {
       luxIdString.reset();
       luxRxString.substringBeetween(luxIdString, "<item id=", 10, "<name>Informationen</name>", -2);
  #ifdef LUX_DEBUG_PRINT
       debug_printluxval("ID", luxIdString.c_str());
  #endif
       //debug_printf("[LUX] ID:\t %s\r\n", luxIdString.c_str());
       //AsyncWebLog.println("Lux-Websocket:Info-ID:"+ String(luxIdString.c_str()));
       //AsyncWebLog.println(String(luxIdString));
       _poll_state = SEND_GET_INFO;
    }
    break;
  case SEND_REFRESH:
     //xml_pos0 = luxRxString.index_of(LUX_XMLDEF_INFO);
     // Items ohne Text (vorher mit GET_INFO holen!)
     //debug_printf("[LUX] REFRESH: %s\r\n", luxRxString.c_str());
     debug_println("[LUX] -- TEMPERATUREN: ---------");
     XMLREFRESH(TEMP_VL_IST,  -3);
     XMLREFRESH(TEMP_RL_IST,  -3);
     XMLREFRESH(TEMP_RL_SOLL, -3);
     XMLREFRESH(TEMP_HEGAS,   -3);
     XMLREFRESH(TEMP_AT_IST,  -3);
     XMLREFRESH(TEMP_AT_MID,  -3);
     XMLREFRESH(TEMP_WW_IST,  -3);
     XMLREFRESH(TEMP_WQ_IN,   -3);
     XMLREFRESH(TEMP_VD_IN,   -3);
     XMLREFRESH(TEMP_VD_HEI,  -3);

     debug_println("[LUX] -- Eingänge: ------------");
     XMLREFRESH(HD_PRESSURE,  -4);
     XMLREFRESH(ND_PRESSURE,  -4);
     XMLREFRESH(PUMP_FLOW,    -4);

     debug_println("[LUX] -- Ausgänge: -------------");
     XMLREFRESH(OUTPUT_DEFROST, 0);
     XMLREFRESH(OUTPUT_WW,      0);
     XMLREFRESH(OUTPUT_VD_HEIZ, 0);
     XMLREFRESH(VD_RPM,        -4); 
     XMLREFRESH(VD_RPM_SOLL,   -4);
     XMLREFRESH(OUTPUT_ZWE1,    0);
     XMLREFRESH(PUMP_PWM,      -2);
     XMLREFRESH(VENT_RPM,      -4);
     XMLREFRESH(EEV_HEAT,      -2);

     debug_println("[LUX] -- Ablaufzeiten: -------------");
     XMLREFRESH(WP_ONTIME,      0);

    debug_println("[LUX] -- Anlagenstatus: ---------");
    XMLREFRESH(POWER_OUT,      -3);
    XMLREFRESH(BETRIEBSZUSTAND, 0);
#ifndef Luxtronik_V3_89
    XMLREFRESH(POWER_IN,       -3); 
    XMLREFRESH(DEFROST_PERCENT,-1);
    XMLREFRESH(DEFROST_LASTTIME,0);
#endif  
    debug_println("[LUX] -- kWh-OUT Wärmemenge:----");
    XMLREFRESH(ENERGY_OUT_HE,  -4); 
    XMLREFRESH(ENERGY_OUT_WW,  -4);        
    XMLREFRESH(ENERGY_OUT_SUM, -4);   

    debug_println("[LUX] -- kWh-IN Leistungsaufnahme:-");
    XMLREFRESH(ENERGY_IN_HE,  -4); 
    XMLREFRESH(ENERGY_IN_WW,  -4);        
    XMLREFRESH(ENERGY_IN_SUM, -4);     

    // für TEST
    // _poll_state = SEND_GET_INFO;
    break;
  case SEND_GET_INFO:                                        
    /*                                                                             used fetch-Index:
     <item id='0x910164'><name>Temperaturen</name>
       <item id='0x98992c'><name>Vorlauf</name><value>20.2°C</value></item>           x = 
       <item id='0x9cfb4c'><name>Rücklauf</name><value>19.9°C</value></item>          x =
       <item id='0x9cfb94'><name>Rückl.-Soll</name><value>15.0°C</value></item>       x =
       <item id='0x9898e4'><name>Heissgas</name><value>28.2°C</value></item>          x =
       <item id='0x9cfc24'><name>Außentemperatur</name><value>19.4°C</value></item>   x =
       <item id='0x9cfc6c'><name>Mitteltemperatur</name><value>18.4°C</value></item>  x =
       <item id='0x9cfcb4'><name>Warmwasser-Ist</name><value>38.6°C</value></item>    x =
       <item id='0x98b1cc'><name>Warmwasser-Soll</name><value>40.0°C</value></item>
       <item id='0x98989c'><name>Wärmequelle-Ein</name><value>21.1°C</value></item>   x =
       <item id='0x9cfabc'><name>Vorlauf max.</name><value>70.0°C</value></item>   
       <item id='0x9e807c'><name>Ansaug VD</name><value>28.5°C</value></item>         x =
       <item id='0x9e810c'><name>VD-Heizung</name><value>41.6°C</value></item>        x =
       <item id='0x98978c'><name>Überhitzung</name><value>8.6 K</value></item>
       <item id='0x9897c4'><name>Überhitzung Soll</name><value>20.0 K</value></item>
       <item id='0x9e78e4'><name>TFL1</name><value>22.7°C</value></item>
       <item id='0x9e792c'><name>TFL2</name><value>24.6°C</value></item>
    <name>Temperaturen</name></item>
    */
     if (luxRxString.index_of("<name>Temperaturen</name>") > 0)  // Temperaturen
     { 
        debug_println("[LUX] -----TEMPERATUREN: -------------------");
        XMLGETInfo(LUX_XMLDEF_TEMP_VL_IST,  TEMP_VL_IST,  -3);
        XMLGETInfo(LUX_XMLDEF_TEMP_RL_IST,  TEMP_RL_IST,  -3);
        XMLGETInfo(LUX_XMLDEF_TEMP_RL_SOLL, TEMP_RL_SOLL, -3);
        XMLGETInfo(LUX_XMLDEF_TEMP_HEGAS,   TEMP_HEGAS,   -3);
        XMLGETInfo(LUX_XMLDEF_TEMP_AT_IST,  TEMP_AT_IST,  -3);
        XMLGETInfo(LUX_XMLDEF_TEMP_AT_MID,  TEMP_AT_MID,  -3);
        XMLGETInfo(LUX_XMLDEF_TEMP_WW_IST,  TEMP_WW_IST,  -3);
        XMLGETInfo(LUX_XMLDEF_TEMP_WQ_IN,   TEMP_WQ_IN,   -3);
        XMLGETInfo(LUX_XMLDEF_TEMP_VD_IN,   TEMP_VD_IN,   -3);
        XMLGETInfo(LUX_XMLDEF_TEMP_VD_HEI,  TEMP_VD_HEI,  -3);
     }

    /* 
    <item id='0x9ce384'><name>Eingänge</name>
     <item id='0x9adfc4'><name>EVU</name><value>Ein</value></item>
     <item id='0x9add54'><name>HD</name><value>Aus</value></item>
     <item id='0x9ae00c'><name>MOT</name><value>Ein</value></item>
     <item id='0x9add9c'><name>HD</name><value>7.24 bar</value></item>
     <item id='0x9adde4'><name>ND</name><value>7.23 bar</value></item>
     <item id='0x9ae1bc'><name>Durchfluss</name><value>--- l/h</value></item>      x fetch-index = 
     <item id='0x9ae30c'><name>EVU 2</name><value>Aus</value></item>
     <item id='0x9ae354'><name>STB E-Stab</name><value>Aus</value></item>
    <name>Eingänge</name></item> 
    */
    debug_println("[LUX] ----- Eingänge: -------------------");
    xml_pos0 = luxRxString.index_of(LUX_XMLDEF_INPUT_MOT);   // tricky da HD mehrmals vorkommt !
    XMLGETInfo(xml_pos0, LUX_XMLDEF_INPUT_HD_PRESSURE,  HD_PRESSURE,  -4);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_INPUT_ND_PRESSURE,  ND_PRESSURE,  -4);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_INPUT_PUMP_FLOW,    PUMP_FLOW,    -4);
   
    /*
     <item id='0x98a0ec'><name>Ausgänge</name>
      <item id='0x9ad3b4'><name>AV-Abtauventil</name><value>Aus</value></item>
      <item id='0x9ad5ac'><name>BUP</name><value>Aus</value></item>                xx  Umschalt-Heiz/WW 0/1
      <item id='0x9ad63c'><name>HUP</name><value>Aus</value></item>                 () nur pos ermitteln da "HUP" doppelt vorkommt           
      <item id='0x9ad564'><name>Ventil.-BOSUP</name><value>Aus</value></item>
      <item id='0x9ad3fc'><name>Verdichter 1</name><value>Aus</value></item>      
      <item id='0x9ad7a4'><name>ZIP</name><value>Aus</value></item>
      <item id='0x9ad8c4'><name>ZUP</name><value>Aus</value></item>
      <item id='0x9ad444'><name>ZWE 1</name><value>Aus</value></item>
      <item id='0x9ad48c'><name>ZWE 2 - SST</name><value>Aus</value></item>
      <item id='0x9aefac'><name>AO 1</name><value>10.00 V</value></item>
      <item id='0x9aefe4'><name>AO 2</name><value>10.00 V</value></item>
      <item id='0x9ad51c'><name>VD-Heizung</name><value>Ein</value></item>         xx Verdichter Heizung 0/1
      <item id='0x9af594'><name>HUP</name><value>0.0 %</value></item>              xx Haupt-Umwaelzpumpe PWM-Wert %
      <item id='0x9af65c'><name>Freq. Sollwert</name><value>0 RPM</value></item> 
      <item id='0x9af6f4'><name>Freq. aktuell</name><value>0 RPM</value></item>    xx Frequ. Verdichter RPM
      <item id='0x9af89c'><name>Ventilatordrehzahl</name><value>0 RPM</value></item>
      <item id='0x9af8e4'><name>EEV Heizen</name><value>0.0 %</value></item>
      <item id='0x9af92c'><name>EEV Kühlen</name><value>0.0 %</value></item>
     <name>Ausgänge</name></item>
    */
    debug_println("[LUX] ----- Ausgänge: -------------------");
    xml_pos0 =  xml_pos0 = luxRxString.index_of(LUX_XMLDEF_OUTPUT);  
    XMLGETInfo(xml_pos0, LUX_XMLDEF_OUTPUT_DEFROST_VENTIL,       OUTPUT_DEFROST, 0);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_OUTPUT_WWVENTIL,             OUTPUT_WW,      0);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_OUTPUT_VDHEI_EIN_AUS,        OUTPUT_VD_HEIZ, 0);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_OUTPUT_VD_RPM,               VD_RPM,        -4); 
    XMLGETInfo(xml_pos0, LUX_XMLDEF_OUTPUT_VD_RPM_SOLL,          VD_RPM_SOLL,   -4); 
    
    XMLGETInfo(xml_pos0, LUX_XMLDEF_OUTPUT_ZWE1,                 OUTPUT_ZWE1,    0);
    xml_pos0 = luxRxString.index_of(LUX_XMLDEF_OUTPUT_ZWE1);  // tricky da HUP mehrmals vorkommt !
    XMLGETInfo(xml_pos0, LUX_XMLDEF_OUTPUT_HUPUMPE_PWM,           PUMP_PWM,      -2);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_OUTPUT_VENT_RPM,              VENT_RPM,      -4);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_OUTPUT_EEVHEI_PERCENT,        EEV_HEAT,      -2);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_OUTPUT_EEVKUE_PERCENT,        EEV_COOL,      -2);
    
    
    /*
    <item id='0x98a134'><name>Ablaufzeiten</name>
      <item id='0x9ae5ec'><name>WP Seit</name><value>00:00:00</value></item>
      <item id='0x9ae664'><name>ZWE1 seit</name><value>00:00:00</value></item>
      <item id='0x9ae72c'><name>Netzeinschaltv.</name><value>00:00:00</value></item>
      <item id='0x9ae764'><name>SSP-Zeit</name><value>00:00:00</value></item>
      <item id='0x9ae794'><name>VD-Stand</name><value>00:48:41</value></item>
      <item id='0x9ae7d4'><name>HRM-Zeit</name><value>00:00:00</value></item>
      <item id='0x9ae81c'><name>HRW-Zeit</name><value>00:48:40</value></item>
      <item id='0x9ae864'><name>TDI seit</name><value>00:00:00</value></item>
      <item id='0x9ae8e4'><name>Sperre WW</name><value>00:00:00</value></item>
      <item id='0x9ae9d4'><name>Freig. ZWE</name><value>00:30:00</value></item>
      <item id='0x9aea5c'><name>Freigabe Kühlung</name><value>11:59:59</value></item>
    <name>Ablaufzeiten</name></item>
    */
    debug_println("[LUX] ----- Ablaufzeiten: -------------------");
    xml_pos0 = luxRxString.index_of(LUX_XMLDEF_ABLAUFZEITEN);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_WP_ONTIME,    WP_ONTIME, 0);

    /*
    <item id='0x91008c'><name>Anlagenstatus</name>
     <item id='0x9affd4'><name>Wärmepumpen Typ</name><value>LP8V</value></item>
     <item id='0x9b0004'><name>Wärmepumpen Typ</name><value>HMDc</value></item>
     <item id='0x9b0034'><name>Hydraulikcode</name><value>000306211</value></item>
     <item id='0x9b00e4'><name>Softwarestand</name><value>V3.89.5</value></item>
     <item id='0x9b0114'><name>Revision</name><value>11451</value></item>
     <item id='0x9b0194'><name>HZ/IO</name><value><![CDATA[<div class='boardInfoLine'><span>  Revision:</span><span>9393</span></div><div class='boardInfoLine'><span>  CPU:</span><span>3</span></div>]]></value></item>
     <item id='0x9b0374'><name>ASB</name><value><![CDATA[<div class='boardInfoLine'><span>  Revision:</span><span>9562</span></div><div class='boardInfoLine'><span>  HW Revision:</span><span>15</span></div>]]></value></item>
     <item id='0x9b05e4'><name>Bedienteil</name><value><![CDATA[<div class='boardInfoLine'><span>  Revision:</span><span>2003</span></div>]]></value></item>
     <item id='0x9b06e4'><name>Inverter SW Version</name><value>52</value></item>
     <item id='0x9b075c'><name>Bivalenz Stufe</name><value>1</value></item>
     <item id='0x9b0794'><name>Betriebszustand</name><value></value></item>
     <item id='0x9b07d4'><name>Heizleistung Ist</name><value>0.00 kW</value></item>  xx (new)
     <item id='xxxxxxxx'><name>Leistungsaufnahme</name>value>0.00 kW</value></item>  xx (V3.90.2 NEW)
     <item id='0x9b098c'><name>Abtaubedarf</name><value>0.0%</value></item>
     <item id='0x9b09c4'><name>Letzte Abt.</name><value>---</value></item>
   <name>Anlagenstatus</name></item>
   */
    debug_println("[LUX] ----- Anlagenstatus: -------------------");
    xml_pos0 = luxRxString.index_of(LUX_XMLDEF_ANLAGENSTATUS);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_BETRIEBSZUSTAND,  BETRIEBSZUSTAND, -0);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_POWER_OUT,        POWER_OUT,       -3);
#ifndef Luxtronik_V3_89
    XMLGETInfo(xml_pos0, LUX_XMLDEF_POWER_IN,         POWER_IN,        -3); 
    XMLGETInfo(xml_pos0, LUX_XMLDEF_DEFROST_PERCENT,  DEFROST_PERCENT, -1);
    XMLGETInfo(xml_pos0, LUX_XMLDEF_DEFROST_LASTTIME, DEFROST_LASTTIME, 0);
#endif        
     
    /*
    <item id='0x9b0cc4'><name>Wärmemenge</name>
      <item id='0x9c2324'><name>Heizung</name><value>12.5 kWh</value></item>
      <item id='0x9c23fc'><name>Warmwasser</name><value>60.1 kWh</value></item>
      <item id='0x9c26dc'><name>Gesamt</name><value>72.6 kWh</value></item>
     <name>Wärmemenge</name>
    */
    debug_println("[LUX] ----- Wärmemenge: -------------------");
    xml_pos0 = luxRxString.index_of(LUX_XMLDEF_ENERGY_OUT); // Wärmemenge
    if (xml_pos0 > 0)
    {   
      XMLGETInfo(xml_pos0, LUX_XMLDEF_ENERGY_OUT_HE,  ENERGY_OUT_HE,  -4); 
      XMLGETInfo(xml_pos0, LUX_XMLDEF_ENERGY_OUT_WW,  ENERGY_OUT_WW,  -4);        
      XMLGETInfo(xml_pos0, LUX_XMLDEF_ENERGY_OUT_SUM, ENERGY_OUT_SUM, -4);        
    }

    /*
    alt (< 2.90)
     </item><item id='0x9b0c2c'><name>Eingesetzte Energie</name>
       <item id='0x9c20ac'><name>Heizung</name><value>3.5 kWh</value></item>
       <item id='0x9c2b0c'><name>Warmwasser</name><value>20.9 kWh</value></item>
       <item id='0x9c341c'><name>Gesamt</name><value>24.4 kWh</value></item>
     <name>Eingesetzte Energie</name></item>

     neu:
    <item id='0xa50964'><name>Leistungsaufnahme</name>
      <item id='0xa56adc'><name>Heizung</name><value>6.1 kWh</value></item>
      <item id='0xa56b9c'><name>Warmwasser</name><value>61.7 kWh</value></item>
      <item id='0xa56e7c'><name>Gesamt</name><value>67.8 kWh</value></item>
    <name>Leistungsaufnahme</name></item>
    */
    debug_println("[LUX] ----- Leistungsaufnahme: -------------------");
    xml_pos0 = luxRxString.index_of(LUX_XMLDEF_ENERGY_IN, luxRxString.index_of("rmemenge"));
    XMLGETInfo(xml_pos0, LUX_XMLDEF_ENERGY_IN_HE,  ENERGY_IN_HE,  -4); 
    XMLGETInfo(xml_pos0, LUX_XMLDEF_ENERGY_IN_WW,  ENERGY_IN_WW,  -4);        
    XMLGETInfo(xml_pos0, LUX_XMLDEF_ENERGY_IN_SUM, ENERGY_IN_SUM, -4);     
   
    _poll_state = SEND_REFRESH; 
    
    //_poll_state = SEND_LOGIN; 
    // poll_state = SEND_GET_INFO;
    break;
  default:
    break;
  }
} 


void LuxWebsocket::poll(bool bNewDay)
{  
    switch (LuxWebsocket::_poll_state)
    {
    case OPEN_CONNECTION:
       debug_println("luxdata.poll_status= OPEN_CONNECTION");
       wsConnect();
       break;
    case SEND_LOGIN:
        debug_println("[Lux] Send: 'LOGIN;999999'");
       _wsClient.sendTXT("LOGIN;999999");
       break;
    case SEND_GET_INFO:
       luxTxString = "GET;";
       luxTxString += luxIdString;
       debug_printf("[Lux] Send: 'GET;%s'\r\n",luxIdString.c_str());
       _wsClient.sendTXT(luxTxString,luxTxString.length());
       break;
     case SEND_REFRESH:
       _wsClient.sendTXT("REFRESH"); 
      calcDayEnergyValues(bNewDay);
      calcCopDay();
       break; 
    case CLOSE_CONNECTION:
        _wsClient.disconnect();
    case NO_POLLING:
        //AsyncWebLog.println("Luxtronik: NO_POLLING"); // activated in "luxtronik.html" Webpage
        break;
    default:
      break;
    }

}








