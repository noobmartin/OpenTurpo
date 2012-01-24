
#include <avr/eeprom.h>

/* The following are command definitions for the
 * serial protocol used to control engine parameters
 * and monitor sensors in real-time.
 */
enum{
  SET  = 'a',
  READ = 'b'
}COMMAND;

enum{
  ALL =             'a',
  LAMBDA =          'b',
  FUEL_PUMP_RELAY = 'c',
  AMM =             'd',
  AMM_TEMP =        'e',
  ENGINE_TEMP =     'f',
  INJECTOR_RELAY =  'g',
  DME_RELAY =       'h',
  RPM =             'i',
  FUEL_CONSUMPTION = 'j',
  FAN =             'k',
  IAC =             'l'
}DEVICE;

enum{
  ON =   'a',
  OFF =  'b'
}OPERATION;

enum{
  DISABLED =  'a',
  LO =        'b',
  HI =        'c'
}FAN_MODE;

typedef struct{
  byte iac;
  byte fan;
  byte dme_relay;
  byte fuel_pump;
  byte inj_relay;
  float lambda;
  float amm;
  float amm_temp;
  float rpm;
  float engine_temp;
  float fuel_consumption;
}full_sensor_data;

/* This value is for E85.
 * For unleaded gasoline, use 14.7
 */
static float afr = 9.76;

/* The following is a table of the density of pure
 * ethanol at various temperatures in centigrade and g/cc.
 */
float fuel_dens_avg = 0.802;
float fuel_density_temperatures[32] =   
{3,4,5,6,7,8,9,10,11,12,16,17,18,19,20,21,22,23,24,25,29,30,31,32,33,34,35,36,37,38,39,40};
float fuel_density[32] =
{
    0.8037,0.80290,0.80207,0.80123,0.80039,0.79956,0.79872,
    0.79788,0.79704,0.79620,0.79283,0.79198,0.79114,0.79029,
    0.78945,0.78860,0.78775,0.78691,0.78606,0.78522,0.78182,
    0.78097,0.78012,0.77927,0.77841,0.77756,0.77671,0.77585,
    0.77500,0.77414,0.77329,0.77244
};

/* This table contains the air mass corresponding to the analog values
 * between 0-1023 that is read off the air mass meter.
 */
float amm_table[1024] = {
 -3662.152539, -3201.209664, -2931.575367, -2740.266789, -2591.876327, 
 -2470.632492, -2368.122290, -2279.323914, -2200.998195, -2130.933452, 
 -2067.552183, -2009.689617, -1956.461216, -1907.179415, -1861.299155, 
 -1818.381039, -1778.065665, -1740.055320, -1704.100618, -1669.990577, 
 -1637.545118, -1606.609307, -1577.048885, -1548.746742, -1521.600115, 
 -1495.518341, -1470.421023, -1446.236540, -1422.900812, -1400.356280, 
 -1378.551048, -1357.438164, -1336.975011, -1317.122790, -1297.846078, 
 -1279.112445, -1260.892127, -1243.157743, -1225.884044, -1209.047702, 
 -1192.627115, -1176.602243, -1160.954462, -1145.666432, -1130.721983, 
 -1116.106010, -1101.804384, -1087.803867, -1074.092041, -1060.657240, 
 -1047.488493, -1034.575466, -1021.908416, -1009.478148, -997.275971, 
 -985.293665, -973.523446, -961.957937, -950.590139, -939.413405, 
 -928.421419, -917.608173, -906.967946, -896.495289, -886.185004, 
 -876.032136, -866.031947, -856.179915, -846.471713, -836.903203, 
 -827.470421, -818.169570, -808.997011, -799.949252, -791.022943, 
 -782.214868, -773.521933, -764.941169, -756.469717, -748.104827, 
 -739.843851, -731.684240, -723.623535, -715.659368, -707.789453, 
 -700.011587, -692.323640, -684.723557, -677.209353, -669.779108, 
 -662.430967, -655.163135, -647.973876, -640.861509, -633.824406, 
 -626.860992, -619.969738, -613.149166, -606.397839, -599.714365, 
 -593.097395, -586.545618, -580.057762, -573.632591, -567.268906, 
 -560.965541, -554.721364, -548.535273, -542.406197, -536.333096, 
 -530.314955, -524.350790, -518.439640, -512.580571, -506.772674, 
 -501.015062, -495.306872, -489.647264, -484.035416, -478.470530, 
 -472.951826, -467.478544, -462.049943, -456.665298, -451.323904, 
 -446.025071, -440.768126, -435.552413, -430.377290, -425.242129, 
 -420.146319, -415.089260, -410.070369, -405.089072, -400.144811, 
 -395.237040, -390.365223, -385.528838, -380.727373, -375.960328, 
 -371.227212, -366.527546, -361.860860, -357.226695, -352.624600, 
 -348.054136, -343.514869, -339.006377, -334.528246, -330.080068, 
 -325.661447, -321.271993, -316.911321, -312.579058, -308.274836, 
 -303.998294, -299.749078, -295.526842, -291.331244, -287.161952, 
 -283.018636, -278.900976, -274.808655, -270.741364, -266.698799, 
 -262.680660, -258.686654, -254.716493, -250.769894, -246.846578, 
 -242.946274, -239.068712, -235.213629, -231.380765, -227.569866, 
 -223.780682, -220.012967, -216.266478, -212.540978, -208.836233, 
 -205.152013, -201.488092, -197.844247, -194.220260, -190.615915, 
 -187.031001, -183.465309, -179.918634, -176.390774, -172.881531, 
 -169.390709, -165.918117, -162.463563, -159.026863, -155.607833, 
 -152.206291, -148.822059, -145.454964, -142.104831, -138.771490, 
 -135.454775, -132.154520, -128.870563, -125.602743, -122.350903, 
 -119.114887, -115.894541, -112.689716, -109.500261, -106.326031, 
 -103.166880, -100.022666, -96.893249, -93.778489, -90.678250, 
 -87.592398, -84.520799, -81.463322, -78.419839, -75.390221, 
 -72.374342, -69.372080, -66.383311, -63.407914, -60.445771, 
 -57.496764, -54.560777, -51.637696, -48.727407, -45.829799, 
 -42.944761, -40.072187, -37.211967, -34.363997, -31.528172, 
 -28.704389, -25.892545, -23.092541, -20.304277, -17.527655, 
 -14.762578, -12.008951, -9.266679, -6.535669, -3.815829, 
 -1.107068, 1.590705, 4.277577, 6.953637, 9.618971, 
 12.273666, 14.917804, 17.551471, 20.174749, 22.787719, 
 25.390462, 27.983058, 30.565585, 33.138122, 35.700746, 
 38.253532, 40.796556, 43.329892, 45.853615, 48.367795, 
 50.872506, 53.367819, 55.853803, 58.330528, 60.798064, 
 63.256477, 65.705835, 68.146205, 70.577652, 73.000241, 
 75.414037, 77.819103, 80.215502, 82.603296, 84.982547, 
 87.353316, 89.715663, 92.069648, 94.415329, 96.752766, 
 99.082015, 101.403135, 103.716180, 106.021209, 108.318275, 
 110.607434, 112.888740, 115.162246, 117.428006, 119.686073,
 121.936498, 124.179333, 126.414630, 128.642437, 130.862807, 
 133.075787, 135.281428, 137.479777, 139.670882, 141.854792,
 144.031554, 146.201213, 148.363817, 150.519410, 152.668039,
 154.809748, 156.944581, 159.072583, 161.193797, 163.308266,
 165.416033, 167.517141, 169.611631, 171.699544, 173.780923,
 175.855808, 177.924239, 179.986256, 182.041899, 184.091207,
 186.134220, 188.170975, 190.201511, 192.225865, 194.244076,
 196.256181, 198.262215, 200.262217, 202.256221, 204.244265,
 206.226382, 208.202610, 210.172982, 212.137532, 214.096297,
 216.049308, 217.996601, 219.938208, 221.874163, 223.804498,
 225.729246, 227.648440, 229.562110, 231.470289, 233.373009,
 235.270300, 237.162193, 239.048719, 240.929908, 242.805791,
 244.676397, 246.541756, 248.401897, 250.256849, 252.106642,
 253.951303, 255.790862, 257.625346, 259.454783, 261.279201,
 263.098628, 264.913090, 266.722615, 268.527229, 270.326960,
 272.121833, 273.911874, 275.697110, 277.477566, 279.253268,
 281.024241, 282.790511, 284.552101, 286.309037, 288.061344,
 289.809046, 291.552166, 293.290729, 295.024759, 296.754278,
 298.479312, 300.199882, 301.916012, 303.627725, 305.335043,
 307.037988, 308.736585, 310.430853, 312.120816, 313.806495,
 315.487912, 317.165088, 318.838045, 320.506803, 322.171385,
 323.831810, 325.488100, 327.140275, 328.788355, 330.432361,
 332.072312, 333.708229, 335.340132, 336.968040, 338.591972,
 340.211949, 341.827988, 343.440110, 345.048334, 346.652677,
 348.253159, 349.849799, 351.442614, 353.031623, 354.616844,
 356.198295, 357.775995, 359.349960, 360.920209, 362.486758,
 364.049626, 365.608830, 367.164386, 368.716312, 370.264625,
 371.809341, 373.350477, 374.888050, 376.422076, 377.952572,
 379.479553, 381.003036, 382.523036, 384.039571, 385.552654,
 387.062303, 388.568533, 390.071358, 391.570795, 393.066859,
 394.559564, 396.048926, 397.534961, 399.017681, 400.497104,
 401.973242, 403.446111, 404.915724, 406.382098, 407.845245,
 409.305179, 410.761916, 412.215468, 413.665851, 415.113077,
 416.557160, 417.998114, 419.435952, 420.870688, 422.302336,
 423.730908, 425.156417, 426.578878, 427.998302, 429.414703,
 430.828094, 432.238486, 433.645894, 435.050330, 436.451806,
 437.850334, 439.245927, 440.638598, 442.028358, 443.415220,
 444.799196, 446.180297, 447.558536, 448.933924, 450.306474,
 451.676196, 453.043103, 454.407206, 455.768517, 457.127046,
 458.482806, 459.835808, 461.186062, 462.533580, 463.878373,
 465.220452, 466.559828, 467.896512, 469.230515, 470.561846,
 471.890518, 473.216541, 474.539924, 475.860679, 477.178817,
 478.494346, 479.807278, 481.117624, 482.425392, 483.730594,
 485.033239, 486.333337, 487.630898, 488.925933, 490.218450,
 491.508460, 492.795973, 494.080997, 495.363543, 496.643621,
 497.921239, 499.196407, 500.469135, 501.739431, 503.007306,
 504.272768, 505.535826, 506.796490, 508.054768, 509.310670,
 510.564205, 511.815381, 513.064208, 514.310694, 515.554848,
 516.796678, 518.036194, 519.273404, 520.508316, 521.740939,
 522.971281, 524.199352, 525.425159, 526.648710, 527.870014,
 529.089080, 530.305915, 531.520527, 532.732925, 533.943116,
 535.151109, 536.356912, 537.560532, 538.761978, 539.961257,
 541.158377, 542.353346, 543.546171, 544.736861, 545.925422,
 547.111863, 548.296191, 549.478414, 550.658538, 551.836572,
 553.012523, 554.186398, 555.358204, 556.527950, 557.695641,
 558.861285, 560.024890, 561.186463, 562.346010, 563.503538,
 564.659055, 565.812568, 566.964084, 568.113609, 569.261150,
 570.406714, 571.550309, 572.691940, 573.831615, 574.969339,
 576.105121, 577.238966, 578.370881, 579.500873, 580.628948,
 581.755113, 582.879373, 584.001736, 585.122208, 586.240796,
 587.357505, 588.472341, 589.585312, 590.696424, 591.805682,
 592.913093, 594.018662, 595.122397, 596.224303, 597.324386,
 598.422652, 599.519107, 600.613758, 601.706609, 602.797668,
 603.886939, 604.974429, 606.060143, 607.144088, 608.226269,
 609.306692, 610.385362, 611.462285, 612.537467, 613.610914,
 614.682631, 615.752623, 616.820896, 617.887456, 618.952308,
 620.015458, 621.076911, 622.136672, 623.194747, 624.251141,
 625.305860, 626.358908, 627.410292, 628.460016, 629.508085,
 630.554506, 631.599282, 632.642419, 633.683923, 634.723798,
 635.762050, 636.798683, 637.833703, 638.867114, 639.898922,
 640.929131, 641.957747, 642.984774, 644.010218, 645.034082,
 646.056373, 647.077095, 648.096252, 649.113850, 650.129893,
 651.144386, 652.157333, 653.168740, 654.178611, 655.186951,
 656.193765, 657.199056, 658.202830, 659.205090, 660.205843,
 661.205092, 662.202842, 663.199096, 664.193861, 665.187140,
 666.178937, 667.169257, 668.158105, 669.145485, 670.131400,
 671.115857, 672.098858, 673.080408, 674.060511, 675.039172,
 676.016395, 676.992184, 677.966543, 678.939476, 679.910988,
 680.881083, 681.849765, 682.817038, 683.782906, 684.747373,
 685.710444, 686.672122, 687.632411, 688.591315, 689.548838,
 690.504985, 691.459759, 692.413164, 693.365205, 694.315884,
 695.265206, 696.213175, 697.159794, 698.105068, 699.049000,
 699.991594, 700.932854, 701.872783, 702.811386, 703.748666,
 704.684627, 705.619272, 706.552606, 707.484631, 708.415352,
 709.344772, 710.272895, 711.199724, 712.125264, 713.049517,
 713.972487, 714.894178, 715.814594, 716.733737, 717.651612,
 718.568221, 719.483569, 720.397658, 721.310493, 722.222076,
 723.132412, 724.041503, 724.949353, 725.855965, 726.761343,
 727.665490, 728.568409, 729.470104, 730.370578, 731.269835,
 732.167877, 733.064708, 733.960331, 734.854749, 735.747966,
 736.639985, 737.530809, 738.420441, 739.308885, 740.196143,
 741.082219, 741.967116, 742.850838, 743.733386, 744.614764,
 745.494976, 746.374025, 747.251912, 748.128643, 749.004219,
 749.878644, 750.751921, 751.624052, 752.495041, 753.364891,
 754.233604, 755.101184, 755.967634, 756.832956, 757.697153,
 758.560229, 759.422187, 760.283028, 761.142757, 762.001376,
 762.858887, 763.715294, 764.570600, 765.424807, 766.277918,
 767.129936, 767.980864, 768.830704, 769.679460, 770.527133,
 771.373728, 772.219246, 773.063691, 773.907064, 774.749370,
 775.590610, 776.430787, 777.269903, 778.107963, 778.944967,
 779.780920, 780.615822, 781.449678, 782.282490, 783.114260,
 783.944991, 784.774685, 785.603346, 786.430975, 787.257576,
 788.083150, 788.907700, 789.731230, 790.553741, 791.375236,
 792.195717, 793.015187, 793.833649, 794.651104, 795.467556,
 796.283007, 797.097459, 797.910915, 798.723377, 799.534847,
 800.345329, 801.154824, 801.963334, 802.770863, 803.577413,
 804.382985, 805.187583, 805.991209, 806.793864, 807.595552,
 808.396275, 809.196034, 809.994833, 810.792674, 811.589558,
 812.385489, 813.180468, 813.974498, 814.767581, 815.559719,
 816.350915, 817.141171, 817.930488, 818.718870, 819.506318,
 820.292835, 821.078423, 821.863084, 822.646820, 823.429633,
 824.211527, 824.992501, 825.772560, 826.551705, 827.329938,
 828.107261, 828.883677, 829.659187, 830.433794, 831.207500,
 831.980306, 832.752216, 833.523231, 834.293352, 835.062583,
 835.830925, 836.598381, 837.364951, 838.130639, 838.895447,
 839.659376, 840.422428, 841.184606, 841.945911, 842.706345,
 843.465912, 844.224611, 844.982446, 845.739418, 846.495530,
 847.250782, 848.005178, 848.758719, 849.511408, 850.263245,
 851.014233, 851.764374, 852.513670, 853.262123, 854.009734,
 854.756505, 855.502439, 856.247537, 856.991802, 857.735234,
 858.477836, 859.219609, 859.960556, 860.700679, 861.439979,
 862.178457, 862.916117, 863.652959, 864.388986, 865.124199,
 865.858600, 866.592190, 867.324973, 868.056949, 868.788120,
 869.518488, 870.248054, 870.976822, 871.704791, 872.431964,
 873.158344, 873.883930, 874.608726, 875.332732, 876.055952,
 876.778385, 877.500035, 878.220902, 878.940989, 879.660296,
 880.378827, 881.096582, 881.813563, 882.529772, 883.245211,
 883.959880, 884.673783, 885.386920, 886.099292, 886.810903,
 887.521753, 888.231844, 888.941177, 889.649755, 890.357578,
 891.064649, 891.770969, 892.476539, 893.181362, 893.885438,
 894.588769, 895.291358, 895.993205, 896.694312, 897.394681,
 898.094313, 898.793209, 899.491372, 900.188803, 900.885502,
 901.581473, 902.276716, 902.971233, 903.665026, 904.358095,
 905.050443, 905.742071, 906.432980, 907.123172, 907.812648,
 908.501411, 909.189461, 909.876799, 910.563428, 911.249349,
 911.934562, 912.619071, 913.302876, 913.985978, 914.668379,
 915.350081, 916.031085, 916.711392, 917.391003, 918.069921,
 918.748147, 919.425681, 920.102526, 920.778683, 921.454152,
 922.128937, 922.803037, 923.476455, 924.149192, 924.821248,
 925.492626, 926.163327, 926.833353, 927.502703, 928.171381,
 928.839387, 929.506723, 930.173390, 930.839389, 931.504722,
 932.169389, 932.833393, 933.496735, 934.159416, 934.821437,
 935.482799, 936.143505, 936.803554, 937.462950, 938.121692,
 938.779782, 939.437221, 940.094011, 940.750154, 941.405649,
 942.060499, 942.714704, 943.368267, 944.021188, 944.673469,
 945.325110, 945.976114, 946.626480, 947.276212
};

/* This value represents the injector flow rates.
 * It is in the units of cc/min @ 3 bar of pressure.
 */
float inj = 350;
float inj_hrs = inj*60;
float inj_hrs_mass = inj_hrs*fuel_dens_avg/1000;

int cylinders = 6;
int flywheel_teeth = 142;
int cylinder_deg_offset = 120;
int cmp_interrupt_deg_before_tdc = 0;
int spark_charge_usec = 4000;  // 4000us coil charge time (4ms).

int amm = A0;
int amm_temp = A2;
int lambda = A4;
int engine_temp = A6;
int knock_front = A8;
int knock_rear = A10;
int tps = A12;
int cmp = 2;
int crk = 4;
int injector_bank_one = 6;
int injector_bank_two = 8;
int iac = 10;
int tach = 12;
int ign_1 = 22;
int ign_2 = 26;
int ign_3 = 30;
int ign_4 = 23;
int ign_5 = 49;
int ign_6 = 53;
int fuel_pump = 39;
int inj_relay = 36;
int dme_relay = 31;
int fan_lo = 35;
int fan_hi = 43;

void setup(){
  /* Set the correct input/output pins. */
  pinMode(amm_temp, INPUT);
  pinMode(amm, INPUT);
  pinMode(lambda, INPUT);
  pinMode(engine_temp, INPUT);
  pinMode(injector_bank_one, OUTPUT);
  pinMode(injector_bank_two, OUTPUT);
  pinMode(cmp, INPUT);
  pinMode(crk, INPUT);
  pinMode(iac, OUTPUT);
  pinMode(ign_1, OUTPUT);
  pinMode(ign_2, OUTPUT);
  pinMode(ign_3, OUTPUT);
  pinMode(ign_4, OUTPUT);
  pinMode(ign_5, OUTPUT);
  pinMode(ign_6, OUTPUT);
  pinMode(fuel_pump, OUTPUT);
  pinMode(inj_relay, OUTPUT);
  pinMode(dme_relay, OUTPUT);
  pinMode(fan_lo, OUTPUT);
  pinMode(fan_hi, OUTPUT);
  
  analogWrite(iac, 255);
  
  /* Set up interrupts. */
  attachInterrupt(crk, crank_trigger, RISING);
  attachInterrupt(cmp, ignition, RISING);
  
  /* Set up serial communication. */
  Serial.begin(115200);
}

void loop(){
  /* In here we continuously adjust fuel injector
   * duty cycle based on air entering the engine.
   * The operation is simple, and uses the signal from
   * the air mass meter to set the correct fuel amount
   * on average based on the air:fuel ratio.
   */
  float air = amm_table[analogRead(amm)];
  byte duty_cycle = 255*(air/(afr*cylinders))/inj_hrs_mass;
  analogWrite(injector_bank_one, duty_cycle);
  analogWrite(injector_bank_two, duty_cycle);
  
  /* Lambda = 1 corresponds to 0.45V -> 92 when converted to digital.
   * Acceptable lambda values lie between 0.2 - 0.6 V which corresponds to 
   * This code continuously adapts the AMM table and should as such only be used when
   * tuning this software for a specific AMM.
   * Once the AMM has been recorded to acceptable precision, this code should not be used.
   */
  int oxygen = analogRead(lambda);
  Serial.print("Oxygen value ");
  Serial.println(oxygen);
  int delta = 3;
  // Running too lean.
  if(oxygen < 41){
    amm_table[analogRead(amm)] -=delta;
  }
  // Running too rich.
  else if(oxygen > 123){
    amm_table[analogRead(amm)] +=delta;
  }
  
}

void ignition(){
  /* This function is interrput triggered.
   * It is used for precise ignition timing for
   * all cylinders.
   * All cylinders complete their total four strokes
   * in two engine revolutions, i.e 720 degrees.
   * Firing order is 1-5-3-6-2-4.
   */
   
   /* The following code is used to check that the interrupt is working.
    * It can also be used in conjunction with the test code for the crank interrupt
    * to calculate how many crank pulses there are per cam pulse.
    */
    
   static int counter = 0;
   
   counter++;
   
   Serial.print("Cam pulses: ");
   Serial.println(counter);
   
   /*
   detachInterrupt(cmp);
   
   // This can be adjusted to vary ignition timing in degrees depending on engine load, RPM etc.
   int fire_deg_before_tdc = 15;

   // As engine RPM varies, so does the time corresponding to one degree.
   float usec_per_deg = ((60/(RPM))/360)*1000000;
   
   // The interrupt is triggered cmp_interrupt_deg_before_tdc degrees before tdc.
   int sleep_deg_before_fire = cmp_interrupt_deg_before_tdc - fire_deg_before_tdc;

   // We also need to take in to account the time it takes to charge the coil.
   float sleep_usec_before_fire = usec_per_deg*sleep_deg_before_fire - spark_charge_usec;
   
   // First cylinder - the coil is fired when the signal goes low.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_1, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_1, LOW);

   // After the first cylinder, all remaining cylinders have the same time offsets.
   sleep_usec_before_fire = usec_per_deg*cylinder_deg_offset - spark_charge_usec;
   
   // Fifth cylinder.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_5, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_5, LOW);
   
   // Third cylinder.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_3, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_3, LOW);
   
   // Sixth cylinder.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_6, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_6, LOW);
   
   // Second cylinder.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_2, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_2, LOW);
   
   // Fourth cylinder.
   delayMicroseconds(sleep_usec_before_fire);
   digitalWrite(ign_4, HIGH);
   delayMicroseconds(spark_charge_usec);
   digitalWrite(ign_4, LOW);

   attachInterrupt(cmp, ignition, RISING);
   
   */
}

void crank_trigger(){
  /* In this function we use the trigger pulses from
   * the crankshaft to continuously update engine
   * RPM.
   */
   
   /* The following code is used for testing that interrupts are triggered.
    * It is also used to calculate the number of interrupts per engine revolution,
    * which will be needed to later continuously calculating engine RPM.
    */
    
  static int counter = 0;
  
  counter++;
  Serial.print("Crank pulses: ");
  Serial.println(counter);
   
  /* 
  static unsigned long last_time = 0;
  static int pulses = 0;
  if(pulses >= flywheel_teeth){
    unsigned long current_time = micros();
    unsigned long time_diff = current_time - last_time;
    static float RPM = 60000000/time_diff;
    pulses = 0;
  }
  else
    pulses++;
  */
}

void serialEvent(){
  /* This function handles communication with the outside world
   * via the serial communication interface.
   */
   byte command = Serial.read();
   Serial.print("Command received");
   Serial.flush();
   switch(command){
    case SET:
      processSet();
      break;
    case READ:
      processRead();
      break;
   }
}

void processSet(){
   byte device = Serial.read();
   Serial.flush();
   byte operation = Serial.read();
   Serial.flush();
   
   switch(device){
    case ALL:
      if(operation == ON){
        digitalWrite(fuel_pump, HIGH);
        digitalWrite(inj_relay, HIGH);
        digitalWrite(dme_relay, HIGH);
        Serial.println("All relays online.");
      }
      else{
        digitalWrite(fuel_pump, LOW);
        digitalWrite(inj_relay, LOW);
        digitalWrite(dme_relay, LOW);
        Serial.println("All relays offline.");
      }
      break;
     
    case FUEL_PUMP_RELAY:
      if(operation == ON){
        digitalWrite(fuel_pump, HIGH);
        Serial.println("Fuel pump online.");
      }
      else{
        digitalWrite(fuel_pump, LOW);
        Serial.println("Fuel pump offline."); 
      }
      break;
    case INJECTOR_RELAY:
     if(operation == ON){
       digitalWrite(inj_relay, HIGH);
       Serial.println("Injectors online."); 
     }
     else{
       digitalWrite(inj_relay, LOW);
       Serial.println("Injectors offline."); 
     }
     break;
    case DME_RELAY:
     if(operation == ON){
       digitalWrite(dme_relay, HIGH);
       Serial.println("DME relay online."); 
     }
     else{
       digitalWrite(dme_relay, LOW);
       Serial.println("DME relay offline.");
     }
     break;
     case FAN:
      if(operation == DISABLED){
        digitalWrite(fan_hi, LOW);
        digitalWrite(fan_lo, LOW);
        Serial.println("Engine fan disabled.");
      }
      else if(operation == LO){
        digitalWrite(fan_hi, LOW);
        digitalWrite(fan_lo, HIGH);
        Serial.println("Engine fan set to low.");
      }
      else if(operation == HI){
        digitalWrite(fan_lo, LOW);
        digitalWrite(fan_hi, HIGH);
        Serial.println("Engine fan set to high.");
      }
     break;
     case IAC:
       analogWrite(iac, operation);
       Serial.println("IAC value set.");
     break;
     case AMM:
       write_amm_to_eeprom();
     break;
  }
}

void processRead(){
   byte device = Serial.read();
   Serial.flush();
   
   full_sensor_data sensor_data;
   
   switch (device){
    case IAC:
      /* Do not know if this is correct since iac is set to OUTPUT mode. */
      Serial.write(analogRead(iac));
    break;
    case FAN:
      if(bitRead(PORTD, fan_lo)){
        Serial.write(LO);
      }
      else if(bitRead(PORTD, fan_hi)){
        Serial.write(HI);
      }
      else{
        Serial.write(DISABLED);
      }
    break;
    case DME_RELAY:
      Serial.write(bitRead(PORTD, dme_relay));
    break;
    case INJECTOR_RELAY:
      Serial.write(bitRead(PORTD, inj_relay));
    break;
    case FUEL_PUMP_RELAY:
      Serial.write(bitRead(PORTD, fuel_pump));
    break;
    case LAMBDA:
      Serial.write(analogRead(lambda));
    break;
    case AMM:
      Serial.write(amm_conversion(analogRead(amm)));
    break;
    case AMM_TEMP:
      Serial.write(amm_temp_conversion(analogRead(amm_temp)));
    break;
    case ENGINE_TEMP:
      Serial.write(engine_temp_conversion(analogRead(engine_temp)));
    break;
    case RPM:
      Serial.write(RPM);
    break;
    case ALL:
      if(bitRead(PORTD, fan_lo)){
        sensor_data.fan = LO;
      }
      else if(bitRead(PORTD, fan_hi)){
        sensor_data.fan = HI;
      }
      else{
        sensor_data.fan = DISABLED;
      }
      /* Do not know if the following line is correct or not. */
      sensor_data.iac = analogRead(iac);
      sensor_data.dme_relay = bitRead(PORTD, dme_relay);
      sensor_data.fuel_pump = bitRead(PORTD, fuel_pump);
      sensor_data.inj_relay = bitRead(PORTD, inj_relay);
      sensor_data.lambda = analogRead(lambda);
      sensor_data.amm = amm_conversion(analogRead(amm));
      sensor_data.amm_temp = amm_temp_conversion(analogRead(amm_temp));
      sensor_data.rpm = RPM;
      sensor_data.engine_temp = engine_temp_conversion(analogRead(engine_temp));
      sensor_data.fuel_consumption = 0;
      Serial.write((uint8_t*)&sensor_data, sizeof(full_sensor_data));
    break; 
   }
}

void write_amm_to_eeprom(){
  eeprom_write_block((const void*)amm_table, (void*)0, sizeof(amm_table));
}

float engine_temp_conversion(int engine_temp){
 /* Simple table lookup. */
 return 0; 
}

float amm_conversion(int amm){
  /* Simple table lookup. */
  return 0;
}

float amm_temp_conversion(int amm_temp){
  /* Simple table lookup. */
  return 0;
}
