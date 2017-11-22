char Byte = 0; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
      Byte = Serial.read();
      delay(500);
      Serial.print(Byte);
   }
}
