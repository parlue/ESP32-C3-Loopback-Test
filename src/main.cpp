#include <Arduino.h>

// Standalone test chip: blue GPIO8 LED pulses only for a verified loopback.
// No BLE, protocol parser or automatic mode switching; USB logs are optional.
HardwareSerial loopUart(1);
static uint32_t rounds = 0, passed = 0;
static constexpr int kBlueLedPin = 8; // SuperMini LED is active LOW.

void setup() {
  digitalWrite(kBlueLedPin, HIGH);
  pinMode(kBlueLedPin, OUTPUT);
  loopUart.setRxBufferSize(2048);
  loopUart.begin(38400, SERIAL_8N1, 20, 21);
  Serial.begin(115200);
  delay(1500);
  Serial.println("[LOOP] RX=GPIO20 TX=GPIO21, 38400 8N1; BLE OFF; blue pulse=verified 256-byte echo");
}

void loop() {
  digitalWrite(kBlueLedPin, HIGH);
  uint8_t sent[256], received[512];
  // Exercise every byte, shifting the pattern each round to detect stale data.
  for (unsigned i = 0; i < sizeof(sent); ++i) sent[i] = uint8_t(i + rounds);
  unsigned stale = 0;
  while (loopUart.available() && stale < 4096) { loopUart.read(); ++stale; }
  const size_t written = loopUart.write(sent, sizeof(sent));
  loopUart.flush();
  size_t count = 0;
  unsigned overflow = 0;
  const uint32_t start = millis();
  while (millis() - start < 200) {
    for (int budget = 0; budget < 512 && loopUart.available(); ++budget) {
      const uint8_t byte = uint8_t(loopUart.read());
      if (count < sizeof(received)) received[count++] = byte;
      else ++overflow;
    }
    delay(1);
  }
  unsigned mismatches = 0;
  int first = -1;
  for (size_t i = 0; i < count && i < sizeof(sent); ++i) {
    if (received[i] != sent[i]) { ++mismatches; if (first < 0) first = int(i); }
  }
  const bool ok = written == sizeof(sent) && count == sizeof(sent) &&
                  !mismatches && !overflow && !stale;
  ++rounds;
  if (ok) {
    ++passed;
    digitalWrite(kBlueLedPin, LOW);
    delay(200);
    digitalWrite(kBlueLedPin, HIGH);
  }
  Serial.printf("[LOOP] round=%lu %s TX=%u RX=%u mismatches=%u extra=%u stale=%u passed=%lu/%lu\r\n",
    (unsigned long)rounds, ok ? "PASS" : "FAIL", unsigned(written), unsigned(count),
    mismatches, overflow, stale, (unsigned long)passed, (unsigned long)rounds);
  if (first >= 0) Serial.printf("[LOOP] first mismatch index=%d sent=%02X received=%02X\r\n",
    first, sent[first], received[first]);
  delay(ok ? 550 : 750);
}
