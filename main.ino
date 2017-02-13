void setup() {
  // do your stuff, like setup WiFi
}

void loop() {

    int rawLevel = analogRead(A0);

    // the 10k?/47k? voltage divider reduces the voltage, so the ADC Pin can handle it
    // According to Wolfram Alpha, this results in the following values:
    // 10k?/(47k?+10k?)*  5v = 0.8772v
    // 10k?/(47k?+10k?)*3.7v = 0.649v
    // 10k?/(47k?+10k?)*3.1v = 0.544
    // * i asumed 3.1v as minimum voltage => see LiPO discharge diagrams
    // the actual minimum i've seen was 467, which would be 2.7V immediately before automatic cutoff
    // a measurement on the LiPo Pins directly resulted in >3.0V, so thats good to know, but no danger to the battery.

    // convert battery level to percent
    int level = map(rawLevel, 500, 649, 0, 100);

    // i'd like to report back the real voltage, so apply some math to get it back
    // 1. convert the ADC level to a float
    // 2. divide by (R2[1] / R1 + R2)
    // [1] the dot is a trick to handle it as float
    float realVoltage = (float)rawLevel / 1000 / (10000. / (47000 + 10000));
    
    // build a nice string to send to influxdb or whatever you like
    char dataLine[64];
    // sprintf has no support for floats, but will be added later, so we need a String() for now
    sprintf(dataLine, "voltage percent=%d,adc=%d,real=%s,charging=%d\n",
        level < 150 ? level : 100, // cap level to 100%, just for graphing, i don't want to see your lab, when the battery actually gets to that level
        rawLevel,
        String(realVoltage, 3).c_str(),
        rawLevel > 850 ? 1 : 0 // USB is connected if the reading is ~870, as the voltage will be 5V, so we assume it's charging
    );

    udp.beginPacket({192, 168, 1, 1}, 8089);
    udp.print(dataLine);
    udp.endPacket();

    delay(1000);

}