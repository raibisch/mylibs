
# Luxtronik Smart-Home-Interface (SHI) MODBUS-TCP

## MODBUS 'Holding-Register' (read and write): 

Register zählweise: 0-based ...oder +1 for 1-based (z.B. für modpoll).
 
| Function                                      | Register | def.-val |
| :---------------------------------------------|---------:| :--------| 
| Heiz.Mode [0=Aus;1=Setpoint;2=Offset]         | 10000    | 0        |
| Heiz. Setpoint [0.1 K]                        | 10001    | 350      |
| Heiz. Offset   [0.1 K]                        | 10002    | 0        | 
| ...                                           |          |          |
| WW Mode   [0=Aus;1=Setpoint;2=Offset]         | 10005    | 0        |
| WW Setpoint [0.1 K]                           | 10006    | 400      |
| WW Offset   [0.1 K]                           | 10007    | 0        |
| ...                                           |          |          |
| MK1Heiz.Mode [0=Aus;1=Setpoint;2=Offset]      | 10000    | 0        |
| MK1Heiz. Setpoint [0,1 K]                     | 10001    | 350      |
| MK1Heiz. Offset   [0,1 K]                     | 10002    | 0        | 
| ...                                           |          |          |
| LPC Mode[0=No-Limit;1=Soft-Limit;2=Hard-Limit]| 10040   | 0        |
| PC Limit 0.1 kW ->0 - 300                     | 100042   | 300      |
| ...                                           |          |          |
| Sperre Kühlung    [0=Aus;1=Ein]               | 10052   | 0        |
| Sperre Schwimmbad [0=Aus;1=Ein]               | 10053   | 0        |


## MODBUS 'Input-Register' (read):

| Function                         | Input   | demo-val  |
|:---------------------------------|:--------|:----------|
|(??)                              |  10000  |  1        |
|...
|Betriebsart 1..5  (*1)            |  10002  |  0...5    |
|HEI=3 WW=2 1? ABT=2 OFF=1         |  10003  |  3, 2     |
|HEI=1 WW=3    ABT=1 OFF=1         |  10004  |  1, 3     |
|...
|              ??                  |  10006  |  0        |
|              ??                  |  10007  |  0        |
|...
|Temp. x10 RL-Soll                 |  10100  |  241      |
|Temp. x10 RL-Ist                  |  10101  |  243      |
|  ?? (temp_offset ?)              |  10102  |   50      |
|  ?? (veränderlich?)              |  10103  |  380      |
|  ?? (min. Rueckl-temp ?)         |  10104  |  150      | 
|Temp x10 VL-Ist                   |  10105  |  241      |
|  ??                              |  10106  |   0       |
|Temp x10 RL (soll) ???            |  10107  |  228, 262 |
|Temp x10 Aussentemperatur         |  10108  |  112      |
|...
|Temp x10 WW-Ist                   |  10120  |  381      |
|Temp x10 WW-Soll                  |  10121  |  400      |
|Sec  Pumpenopti_zeit              |  10122  |  300      |
|Temp x10 Tdi_solltemp             |  10123  |  650      |
|Temp x10 WW-Soll-Max ??           |  10124  |  400      |
|...
|Temp x10 Mischkreis1_VL           |  10140  |  750      |
|Temp x10 Mischkreis1_VL_soll      |  10141  |  190      |
|           input_reg23            |  10142  |  200      |
|           input_reg24            |  10143  |  450      |
|...
|           input_reg25            |  10150  |   0       |
|           input_reg26            |  10151  |   0       |
|           input_reg27            |  10152  |  200      |
|           input_reg28            |  10153  |  450      |
|...
|           input_reg29            |  10160  |   0       |
|           input_reg30            |  10161  |   0       |
|           input_reg31            |  10162  |  200      |
|           input_reg32            |  10163  |  450      |
|...
|           input_reg33            |  10201  |   0       |
|           input_reg34            |  10202  |   0       |
|           input_reg35            |  10203  |   20      |
|           input_reg36            |  10204  |   20      |
|           input_reg37            |  10205  |   0       |
|           input_reg38            |  10206  |   0       |
|           input_reg39            |  10207  |   0       |
|...
|kW x10 Power-OUT Heiz. (3.4kW=34) |  10300  |  34       |
|kW x10 Power-In  Elekt.(0.8kW=8)  |  10301  |   8       |
|? x10 Power-In min ??             |  10302  |   6       |
|...
| ??                               |  10310  |   0       |
|kWh x10 Elekt. Arbeit Ges.        |  10311  | 24321     |
| ??                               |  10312  |   0       |
|kWh x10 Elekt. Arbeit Heiz        |  10313  | 21314     |
| ??                               |  10314  |   0       |
|kWh x10 Elekt. Arbeit WW          |  10315  |  3007     |
| ??                               |  10316  |   0       |
| ??                               |  10317  |   0       |
| ??                               |  10318  |   0       |
| ??                               |  10319  |   0       |
|...
| Firmware V3.xx-x                 |  10400  |   3       |
| Firmware Vx.90-x                 |  10401  |  90       |
| Firmware Vx.xx-4                 |  10402  |   4       | 

(*1) Input-Register 10002:   
0 = Heiz  
1 = WW  
2 = Schwimmbad  
3 = EVU-Sperre  
4 = Abtauen  
5 = Aus  
6 = Ext Quelle  
7 = Kühlung  



[back -->  LuxtronikSHI howto](LuxtronikSHI.md)
