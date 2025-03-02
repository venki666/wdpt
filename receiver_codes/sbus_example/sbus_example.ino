#include "sbus.h"

/* SBUS object, reading SBUS */
// bfs::SbusRx sbus_rx(&Serial2, 26, 27, 1, 1);
bfs::SbusRx sbus_rx(&Serial2, 13, 14, true);
/* SBUS object, writing SBUS */
// bfs::SbusTx sbus_tx(&Serial2, 26, 27, 1, 1);
/* SBUS data */
bfs::SbusData data;

void setup() {
  /* Serial to display data */
  Serial.begin(115200);
  while (!Serial) {}

  delay(2000);

  /* Begin the SBUS communication */
  Serial.println("Begin SBUS...");
  sbus_rx.Begin();
  // sbus_tx.Begin();
}

void loop() {

  if (sbus_rx.Read()) {
    /* Grab the received data */
    data = sbus_rx.data();
    /* Display the received data */
    for (int8_t i = 0; i < data.NUM_CH; i++) {
      Serial.print(data.ch[i]);
      Serial.print("\t");
    }
    
    /* Display lost frames and failsafe data */
    Serial.print(data.lost_frame);
    Serial.print("\t");
    Serial.println(data.failsafe);
    /* Set the SBUS TX data to the received data */
    // sbus_tx.data(data);
    /* Write the data to the servos */
    // sbus_tx.Write();
  }
}
