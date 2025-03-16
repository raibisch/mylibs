
# Luxtronik Smart-Home-Interface (SHI) MODBUS-TCP

## Modbus-Holding-Register (read and write): 

0-based ... or add +1 for 1-based register counting
 
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
| LPC Mode[0=No-Limit;1=Soft-Limit;2=Hard-Limit]| 100040   | 0        |
| PC Limit 0.1 kW ->0 - 300                     | 100042   | 300      |
| ...                                           |          |          |
| Sperre Kühlung    [0=Aus;1=Ein]               | 100052   | 0        |
| Sperre Schwimmbad [0=Aus;1=Ein]               | 100053   | 0        |




## Modbus-Input-Register (read):

| Function                         | Input   | demo-val|
|:--------------------------------:|:--------|:--------|
|           Betriebsmodus          |  10003  |  0      |
|              ??                  |  10004  |  3      |  
|              ??                  |  10005  |  1      |
|              ??                  |  10007  |  0      |
|              ??                  |  10008  |  0      |
|...
|         input_ruecklauf          |  10100  |  241    |
|       input_ruecklauf_soll       |  10101  |  243    |
|               ??                 |  10102  |   50    |
|           input_reg10            |  10103  |  500    |
|           input_reg11            |  10104  |  150    | 
|          input_vorlauf           |  10105  |  241    |
|           input_reg13            |  10106  |   0     |
|           input_reg14            |  10107  |  228    |
|      input_aussentemperatur      |  10108  |  112    |
|...
|           input_ww_ist           |  10120  |  509    |
|          input_ww_soll           |  10121  |  500    |
|      input_pumpenopti_zeit       |  10122  |  300    |
|        input_tdi_solltemp        |  10123  |  650    |
|           input_reg20            |  10124  |  440    |
|...
|    input_mischkreis1_vorlauf     |  10140  |  240    |
|  input_mischkreis1_vorlauf_soll  |  10141  |  190    |
|           input_reg23            |  10142  |  200    |
|           input_reg24            |  10143  |  450    |
|           input_reg25            |  10150  |   0     |
|           input_reg26            |  10151  |   0     |
|           input_reg27            |  10152  |  200    |
|           input_reg28            |  10153  |  450    |
|           input_reg29            |  10160  |   0     |
|           input_reg30            |  10161  |   0     |
|           input_reg31            |  10162  |  200    |
|           input_reg32            |  10163  |  450    |
|           input_reg33            |  10201  |   0     |
|           input_reg34            |  10202  |   0     |
|           input_reg35            |  10203  |   20    |
|           input_reg36            |  10204  |   20    |
|           input_reg37            |  10205  |   0     |
|           input_reg38            |  10206  |   0     |
|           input_reg39            |  10207  |   0     |
|...
|           input_reg40            |  10300  |   0     |
|           input_reg41            |  10301  |   0     |
|           input_reg42            |  10302  |   8     |
|           input_reg43            |  10310  |   0     |
|  input_leistungsaufnahme_gesamt  |  10311  | 32093   |
|           input_reg45            |  10312  |   0     |
| input_leistungsaufnahme_heizung  |  10313  | 24932   |
|           input_reg47            |  10314  |   0     |
|    input_leistungsaufnahme_ww    |  10315  |  7161   |
|           input_reg49            |  10316  |   0     |
|           input_reg50            |  10317  |   0     |
|           input_reg51            |  10318  |   0     |
|           input_reg52            |  10319  |   0     |
|           input_reg53            |  10400  |   3     |
|           input_reg54            |  10401  |   90    |
|           input_reg55            |  10402  |   1     | 

