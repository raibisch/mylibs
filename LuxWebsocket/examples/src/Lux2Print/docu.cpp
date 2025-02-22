
// ----------- LUXTRONIK 2.1 Websocket communication ----------------------------
// ===============================================================================
/*  ----only for Test without Luxtronic ------
const char test_login_xml[] PROGMEM = ^
R"=====(
<Content>
 <item id='0x910164'><name>Temperaturen</name>
  <item id='0x98992c'><name>Vorlauf</name><value>20.2°C</value></item>
  <item id='0x9cfb4c'><name>Rücklauf</name><value>19.9°C</value></item>
  <item id='0x9cfb94'><name>Rückl.-Soll</name><value>15.0°C</value></item>
  <item id='0x9898e4'><name>Heissgas</name><value>28.2°C</value></item>
  <item id='0x9cfc24'><name>Außentemperatur</name><value>19.4°C</value></item>
  <item id='0x9cfc6c'><name>Mitteltemperatur</name><value>18.4°C</value></item>
  <item id='0x9cfcb4'><name>Warmwasser-Ist</name><value>38.6°C</value></item>
  <item id='0x98b1cc'><name>Warmwasser-Soll</name><value>40.0°C</value></item>
  <item id='0x98989c'><name>Wärmequelle-Ein</name><value>21.1°C</value></item>
  <item id='0x9cfabc'><name>Vorlauf max.</name><value>70.0°C</value></item>
  <item id='0x9e807c'><name>Ansaug VD</name><value>28.5°C</value></item>
  <item id='0x9e810c'><name>VD-Heizung</name><value>41.6°C</value></item>
  <item id='0x98978c'><name>Überhitzung</name><value>8.6 K</value></item>
  <item id='0x9897c4'><name>Überhitzung Soll</name><value>20.0 K</value></item>
  <item id='0x9e78e4'><name>TFL1</name><value>22.7°C</value></item>
  <item id='0x9e792c'><name>TFL2</name><value>24.6°C</value></item>
  <name>Temperaturen</name></item>

)=====";  


// Python example
//https://gist.github.com/BenPru/ec463ea835fb705b070939c793196f2e

/*
send: -->
LOGIN;

receive:<--
<Navigation id='0x9c7dd0'>
<item id='0x9c91e0'><name>Informationen</name>
  <item id='0x98abe0'><name>Temperaturen</name></item>
  <item id='0x98a1c0'><name>Eingänge</name></item>
  <item id='0x910040'><name>Ausgänge</name></item>
  <item id='0x98a178'><name>Ablaufzeiten</name></item>
  <item id='0x9100d0'><name>Betriebsstunden</name></item>
  <item id='0x9101f0'><name>Fehlerspeicher</name></item>
  <item id='0x9cf950'><name>Abschaltungen</name></item>
  <item id='0x9cf998'><name>Anlagenstatus</name></item>
  <item id='0x98a298'><name>Energiemonitor</name>
   <item id='0x9101a8'><name>Wärmemenge</name></item>
   <item id='0x910118'><name>Eingesetzte Energie</name></item>
  </item>
  <item id='0x910008'><name>GLT</name></item>
 </item>
 </Navigation>


send: -->
GET;0x9c91e0


                                                                                 x = values read
                                                                                 xx new values read
receive: <--
<Content>
 <item id='0x910164'><name>Temperaturen</name>
  <item id='0x98992c'><name>Vorlauf</name><value>20.2°C</value></item>           x
  <item id='0x9cfb4c'><name>Rücklauf</name><value>19.9°C</value></item>          x
  <item id='0x9cfb94'><name>Rückl.-Soll</name><value>15.0°C</value></item>
  <item id='0x9898e4'><name>Heissgas</name><value>28.2°C</value></item>          xx
  <item id='0x9cfc24'><name>Außentemperatur</name><value>19.4°C</value></item>   x
  <item id='0x9cfc6c'><name>Mitteltemperatur</name><value>18.4°C</value></item>  x
  <item id='0x9cfcb4'><name>Warmwasser-Ist</name><value>38.6°C</value></item>
  <item id='0x98b1cc'><name>Warmwasser-Soll</name><value>40.0°C</value></item>   x
  <item id='0x98989c'><name>Wärmequelle-Ein</name><value>21.1°C</value></item>   xx
  <item id='0x9cfabc'><name>Vorlauf max.</name><value>70.0°C</value></item>
  <item id='0x9e807c'><name>Ansaug VD</name><value>28.5°C</value></item>
  <item id='0x9e810c'><name>VD-Heizung</name><value>41.6°C</value></item>        xx
  <item id='0x98978c'><name>Überhitzung</name><value>8.6 K</value></item>        xx
  <item id='0x9897c4'><name>Überhitzung Soll</name><value>20.0 K</value></item>
  <item id='0x9e78e4'><name>TFL1</name><value>22.7°C</value></item>
  <item id='0x9e792c'><name>TFL2</name><value>24.6°C</value></item>
 <name>Temperaturen</name></item>

 <item id='0x9ce384'><name>Eingänge</name>
  <item id='0x9ade2c'><name>ASD</name><value>Aus</value></item>
  <item id='0x9adfc4'><name>EVU</name><value>Ein</value></item>
  <item id='0x9add54'><name>HD</name><value>Aus</value></item>
  <item id='0x9ae00c'><name>MOT</name><value>Ein</value></item>
  <item id='0x9add9c'><name>HD</name><value>7.24 bar</value></item>
  <item id='0x9adde4'><name>ND</name><value>7.23 bar</value></item>
  <item id='0x9ae1bc'><name>Durchfluss</name><value>--- l/h</value></item>      xx
  <item id='0x9ae30c'><name>EVU 2</name><value>Aus</value></item>
  <item id='0x9ae354'><name>STB E-Stab</name><value>Aus</value></item>
 <name>Eingänge</name></item>

 <item id='0x98a0ec'><name>Ausgänge</name>
  <item id='0x9ad3b4'><name>AV-Abtauventil</name><value>Aus</value></item>
  <item id='0x9ad5ac'><name>BUP</name><value>Aus</value></item>                xx  Umschalt-Heiz/WW
  <item id='0x9ad63c'><name>HUP</name><value>Aus</value></item>              
  <item id='0x9ad564'><name>Ventil.-BOSUP</name><value>Aus</value></item>
  <item id='0x9ad3fc'><name>Verdichter 1</name><value>Aus</value></item>
  <item id='0x9ad7a4'><name>ZIP</name><value>Aus</value></item>
  <item id='0x9ad8c4'><name>ZUP</name><value>Aus</value></item>
  <item id='0x9ad444'><name>ZWE 1</name><value>Aus</value></item>
  <item id='0x9ad48c'><name>ZWE 2 - SST</name><value>Aus</value></item>
  <item id='0x9aefac'><name>AO 1</name><value>10.00 V</value></item>
  <item id='0x9aefe4'><name>AO 2</name><value>10.00 V</value></item>
  <item id='0x9ad51c'><name>VD-Heizung</name><value>Ein</value></item>         xx
  <item id='0x9af594'><name>HUP</name><value>0.0 %</value></item>              xx
  <item id='0x9af65c'><name>Freq. Sollwert</name><value>0 RPM</value></item> 
  <item id='0x9af6f4'><name>Freq. aktuell</name><value>0 RPM</value></item>    xx
  <item id='0x9af89c'><name>Ventilatordrehzahl</name><value>0 RPM</value></item>
  <item id='0x9af8e4'><name>EEV Heizen</name><value>0.0 %</value></item>
  <item id='0x9af92c'><name>EEV Kühlen</name><value>0.0 %</value></item>
 <name>Ausgänge</name></item>

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

 <item id='0x9cfa74'><name>Betriebsstunden</name>
  <item id='0x9aef1c'><name>Betriebstund. VD1</name><value>10h</value></item>   
  <item id='0x9aef64'><name>Impulse Verdichter 1</name><value>17</value></item> 
  <item id='0xa52db4'><name>Laufzeit Ø VD1</name><value>00:35</value></item>    
  <item id='0xa52ed4'><name>Betriebstunden ZWE1</name><value>1h</value></item>
  <item id='0xa52fac'><name>Betriebstunden WP</name><value>10h</value></item>   
  <item id='0xa52ff4'><name>Betriebstunden Heiz.</name><value>2h</value></item>
  <item id='0xa5303c'><name>Betriebstunden WW</name><value>8h</value></item>
 <name>Betriebsstunden</name></item>

 <item id='0x98a1fc'><name>Fehlerspeicher</name>
  <item id='0xa5342c'><name>25.05.24 13:46:13</name><value>Inverter nicht kompatibel (813)</value></item>
  <item id='0xa534bc'><name>25.05.24 13:45:38</name><value>Modbus Inverter (791)</value></item>
  <item id='0xa5354c'><name>25.05.24 13:43:50</name><value>Inverter nicht kompatibel (813)</value></item>
  <item id='0xa535dc'><name>24.08.23 13:02:11</name><value>Fühler Verdichterheizung (764)</value></item>
  <item id='0xa5366c'><name>24.08.23 13:00:20</name><value>Fühler Verdichterheizung (764)</value></item>
 <name>Fehlerspeicher</name></item>

 <item id='0x98a254'><name>Abschaltungen</name>
  <item id='0x9afcfc'><name>04.06.24 14:43:31</name><value>keine Anf.</value></item>
  <item id='0x9afc7c'><name>03.06.24 14:35:37</name><value>keine Anf.</value></item>
  <item id='0x9afbfc'><name>02.06.24 14:33:38</name><value>keine Anf.</value></item>
  <item id='0x9afb7c'><name>01.06.24 14:38:37</name><value>keine Anf.</value></item>
  <item id='0x9afafc'><name>31.05.24 14:33:24</name><value>keine Anf.</value></item>
 <name>Abschaltungen</name></item>

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
  <item id='0x9b07d4'><name>Heizleistung Ist</name><value>0.00 kW</value></item>
  <item id='0x9b098c'><name>Abtaubedarf</name><value>0.0%</value></item>
  <item id='0x9b09c4'><name>Letzte Abt.</name><value>---</value></item>
<name>Anlagenstatus</name></item>

<item id='0x9ce3cc'><name>Energiemonitor</name>
  <item id='0x9b0cc4'><name>Wärmemenge</name>
    <item id='0x9c2324'><name>Heizung</name><value>12.5 kWh</value></item>      x
    <item id='0x9c23fc'><name>Warmwasser</name><value>60.1 kWh</value></item>   x
    <item id='0x9c26dc'><name>Gesamt</name><value>72.6 kWh</value></item>       x
  <name>Wärmemenge</name>
  </item><item id='0x9b0c2c'><name>Eingesetzte Energie</name>
    <item id='0x9c20ac'><name>Heizung</name><value>3.5 kWh</value></item>       x
    <item id='0x9c2b0c'><name>Warmwasser</name><value>20.9 kWh</value></item>   x
    <item id='0x9c341c'><name>Gesamt</name><value>24.4 kWh</value></item>       x
  <name>Eingesetzte Energie</name></item>
<name>Energiemonitor</name></item>

 <item id='0x9894e4'><name>GLT</name>
   <item id='0x9c2b74'><name>Art</name><value>Aus</value></item>
 <name>GLT</name></item>

<name>Informationen</name>
</Content>



NEU ab 3.90:

<Content>
  <item id='0x9ddd24'><name>Temperaturen</name>
    <item id='0xa0fe74'><name>Vorlauf</name><value>25.5°C</value></item>
    <item id='0xa4d7e4'><name>Rücklauf</name><value>21.6°C</value></item>
    <item id='0xa4d82c'><name>Rückl.-Soll</name><value>15.0°C</value></item>
    <item id='0xa0fe2c'><name>Heissgas</name><value>29.0°C</value></item>
    <item id='0xa4d8bc'><name>Außentemperatur</name><value>28.1°C</value></item>
    <item id='0xa4d904'><name>Mitteltemperatur</name><value>23.8°C</value></item>
    <item id='0xa4d94c'><name>Warmwasser-Ist</name><value>33.2°C</value></item>
    <item id='0xa511f4'><name>Warmwasser-Soll</name><value>40.0°C</value></item>
    <item id='0xa0fde4'><name>Wärmequelle-Ein</name><value>27.8°C</value></item>
    <item id='0xa4d754'><name>Vorlauf max.</name><value>70.0°C</value></item>
    <item id='0xa511ac'><name>Ansaug VD</name><value>26.8°C</value></item>
    <item id='0xa0fc7c'><name>VD-Heizung</name><value>29.3°C</value></item>
    <item id='0xa0fcc4'><name>Überhitzung</name><value>2.6 K</value></item>
    <item id='0xa0fd0c'><name>Überhitzung Soll</name><value>20.0 K</value></item>
    <item id='0xa5159c'><name>TFL1</name><value>26.9°C</value></item>
    <item id='0xa515e4'><name>TFL2</name><value>25.5°C</value></item>
  <name>Temperaturen</name></item>
  <item id='0x9ddd5c'><name>Eingänge</name>
    <item id='0xa51dcc'><name>ASD</name><value>Aus</value></item><item id='0xa0b4fc'><name>EVU</name><value>Ein</value></item><item id='0xa5083c'><name>HD</name><value>Aus</value></item><item id='0xa0b544'><name>MOT</name><value>Ein</value></item><item id='0xa51d3c'><name>HD</name><value>8.16 bar</value></item><item id='0xa51d84'><name>ND</name><value>8.19 bar</value></item><item id='0xa0b6f4'><name>Durchfluss</name><value>498 l/h</value></item><item id='0xa0b844'><name>EVU 2</name><value>Aus</value></item><item id='0xa0b88c'><name>STB E-Stab</name><value>Aus</value></item>
  <name>Eingänge</name></item>
  <item id='0x9ddd94'><name>Ausgänge</name><item id='0xa0b2f4'><name>AV-Abtauventil</name><value>Aus</value></item><item id='0xa0bcc4'><name>BUP</name><value>Aus</value></item><item id='0xa0bd54'><name>HUP</name><value>Ein</value></item><item id='0xa0bc7c'><name>Ventil.-BOSUP</name><value>Aus</value></item><item id='0xa0b33c'><name>Verdichter 1</name><value>Aus</value></item><item id='0xa0bebc'><name>ZIP</name><value>Aus</value></item><item id='0xa0bfdc'><name>ZUP</name><value>Aus</value></item><item id='0xa0b384'><name>ZWE 1</name><value>Aus</value></item><item id='0xa0b3cc'><name>ZWE 2 - SST</name><value>Aus</value></item><item id='0xa0c2ac'><name>AO 1</name><value>10.00 V</value></item><item id='0xa0ccd4'><name>AO 2</name><value>10.00 V</value></item><item id='0xa0bc44'><name>VD-Heizung</name><value>Aus</value></item><item id='0xa0d274'><name>HUP</name><value>28.6 %</value></item><item id='0xa0d33c'><name>Freq. Sollwert</name><value>0 RPM</value></item><item id='0xa0d3d4'><name>Freq. aktuell</name><value>0 RPM</value></item><item id='0xa0d57c'><name>Ventilatordrehzahl</name><value>0 RPM</value></item><item id='0xa0d5c4'><name>EEV Heizen</name><value>0.0 %</value></item><item id='0xa0d60c'><name>EEV Kühlen</name><value>0.0 %</value></item>
  <name>Ausgänge</name></item><item id='0xa10084'><name>Ablaufzeiten</name><item id='0xa0d764'><name>WP Seit</name><value>00:00:00</value></item><item id='0xa0d7dc'><name>ZWE1 seit</name><value>00:00:00</value></item><item id='0xa0d8a4'><name>Netzeinschaltv.</name><value>00:00:51</value></item><item id='0xa0d8dc'><name>SSP-Zeit</name><value>00:00:00</value></item><item id='0xa0d90c'><name>VD-Stand</name><value>00:00:53</value></item><item id='0xa0d94c'><name>HRM-Zeit</name><value>00:00:00</value></item><item id='0xa0d994'><name>HRW-Zeit</name><value>00:00:52</value></item><item id='0xa0d9dc'><name>TDI seit</name><value>00:00:00</value></item><item id='0xa0da5c'><name>Sperre WW</name><value>00:00:00</value></item><item id='0xa0db4c'><name>Freig. ZWE</name><value>00:20:00</value></item><item id='0xa0dbd4'><name>Freigabe Kühlung</name><value>11:59:07</value></item><name>Ablaufzeiten</name></item><item id='0xa100bc'><name>Betriebsstunden</name><item id='0xa0c5d4'><name>Betriebstund. VD1</name><value>40h</value></item><item id='0xa0c61c'><name>Impulse Verdichter 1</name><value>78</value></item><item id='0xa0c664'><name>Laufzeit Ø VD1</name><value>00:30</value></item><item id='0xa0c784'><name>Betriebstunden ZWE1</name><value>1h</value></item><item id='0xa0c85c'><name>Betriebstunden WP</name><value>40h</value></item><item id='0xa0c8a4'><name>Betriebstunden Heiz.</name><value>2h</value></item><item id='0xa0c8ec'><name>Betriebstunden WW</name><value>37h</value></item><name>Betriebsstunden</name></item><item id='0xa100f4'><name>Fehlerspeicher</name><item id='0xa521d4'><name>25.05.24 13:46:13</name><value>Inverter nicht kompatibel (813)</value></item><item id='0xa52264'><name>25.05.24 13:45:38</name><value>Modbus Inverter (791)</value></item><item id='0xa522f4'><name>25.05.24 13:43:50</name><value>Inverter nicht kompatibel (813)</value></item><item id='0xa52384'><name>24.08.23 13:02:11</name><value>Fühler Verdichterheizung (764)</value></item><item id='0xa52414'><name>24.08.23 13:00:20</name><value>Fühler Verdichterheizung (764)</value></item><name>Fehlerspeicher</name></item><item id='0xa1012c'><name>Abschaltungen</name><item id='0xa52814'><name>10.08.24 15:36:50</name><value>keine Anf.</value></item><item id='0xa52794'><name>09.08.24 14:38:33</name><value>keine Anf.</value></item><item id='0xa52714'><name>08.08.24 14:26:24</name><value>keine Anf.</value></item><item id='0xa52694'><name>07.08.24 14:32:52</name><value>keine Anf.</value></item><item id='0xa52614'><name>06.08.24 11:23:35</name><value>keine Anf.</value></item><name>Abschaltungen</name></item>
  <item id='0xa10164'><name>Anlagenstatus</name>
   <item id='0xa52b64'><name>Wärmepumpen Typ</name><value>LP8V</value></item>
   <item id='0xa52b94'><name>Wärmepumpen Typ</name><value>HMDc</value></item>
   <item id='0xa52bc4'><name>Hydraulikcode</name><value>000306211</value></item>
   <item id='0xa52c74'><name>Softwarestand</name><value>V3.90.0</value></item><item id='0xa52ca4'><name>Revision</name><value>12065</value></item><item id='0xa52d24'><name>HZ/IO</name><value><![CDATA[<div class='boardInfoLine'><span>  Revision:</span><span>11298</span></div><div class='boardInfoLine'><span>  CPU:</span><span>3</span></div>]]></value></item><item id='0xa52dbc'><name>ASB</name><value><![CDATA[<div class='boardInfoLine'><span>  Revision:</span><span>11417</span></div><div class='boardInfoLine'><span>  HW Revision:</span><span>15</span></div>]]></value></item><item id='0xa54f94'><name>Inverter</name><value><![CDATA[<div class='boardInfoLine'><span>  SW Version:</span><span>52</span></div>]]></value></item><item id='0xa551a4'><name>Bedienteil</name><value><![CDATA[<div class='boardInfoLine'><span>  HW Revision:</span><span>2003</span></div>]]></value></item><item id='0xa5530c'><name>Bivalenz Stufe</name><value>1</value></item><item id='0xa55344'><name>Betriebszustand</name><value></value></item><item id='0xa55384'><name>Heizleistung Ist</name><value>0.00 kW</value></item>
   <item id='0xa553cc'><name>Leistungsaufnahme</name><value>0.00 kW</value></item>
   <item id='0xa55584'><name>Abtaubedarf</name><value>0.0%</value></item>
   <item id='0xa555bc'><name>Letzte Abt.</name><value>---</value></item>
  <name>Anlagenstatus</name></item>
  <item id='0xa1019c'><name>Energiemonitor</name>
    <item id='0xa52b2c'><name>Wärmemenge</name>
      <item id='0xa55cec'><name>Heizung</name><value>19.7 kWh</value></item>
      <item id='0xa55db4'><name>Warmwasser</name><value>236.6 kWh</value></item>
      <item id='0xa56094'><name>Gesamt</name><value>256.3 kWh</value></item>
    <name>Wärmemenge</name></item>
    <item id='0xa50964'><name>Leistungsaufnahme</name>
     <item id='0xa56adc'><name>Heizung</name><value>6.1 kWh</value></item>
     <item id='0xa56b9c'><name>Warmwasser</name><value>61.7 kWh</value></item>
     <item id='0xa56e7c'><name>Gesamt</name><value>67.8 kWh</value></item>
    <name>Leistungsaufnahme</name></item>
  <name>Energiemonitor</name></item>
  <item id='0xa0ff04'><name>GLT</name>
    <item id='0xa509ec'><name>Art</name><value>Aus</value>
  </item><name>GLT</name></item>
<name>Informationen</name>
</Content>

*/