# ESP32-C3 Loopback Test

Eigenständiger Testchip für selbstgebaute UART-/MAX3232-Platinen und Kabel.
**Blau blinkt nur bei einem vollständig und bytegenau zurückgekommenen Testpaket.**
Kein BLE, kein Schachprotokoll, kein PC für den laufenden Test erforderlich.

## Ohne Entwicklungsumgebung flashen

**[Webflasher öffnen](https://parlue.github.io/ESP32-C3-Loopback-Test/)**

1. ESP32-C3 SuperMini mit einem USB-Datenkabel anschließen, seriellen Monitor schließen.
2. Webflasher in einem Desktop-Browser mit Web Serial öffnen, z. B. Chrome oder Edge.
3. „Test-Firmware installieren“ wählen und den USB-Port bestätigen.
4. Nach der Installation neu starten. Falls keine Verbindung möglich ist:
   BOOT gedrückt halten, RESET kurz drücken, BOOT loslassen, erneut versuchen.

Die Installation ersetzt die bisherige Firmware. Vorhandene PGNs vorher sichern;
„Gerät löschen“ entfernt auch gespeicherte Daten. Zum späteren Schachbetrieb
muss wieder die Gateway-Firmware installiert werden.

## Testanschluss

Strom ausschalten, bevor die Brücke gesetzt oder entfernt wird. King/Phoenix
bleiben während des Loopbacktests vollständig vom Datenanschluss getrennt.

| Testumfang | Brücke |
|---|---|
| ESP allein | GPIO21 (TX) mit GPIO20 (RX) verbinden |
| ESP + MAX3232-Platine | Die beiden RS-232-Datenleitungen am DIN-Ausgang verbinden |
| ESP + Platine + Kabel | Die beiden Datenleitungen am freien Kabelende verbinden |

**Nur TX und RX verbinden, niemals Versorgung oder Masse kurzschließen.**
RS-232-Spannungen niemals direkt an GPIO20/21 anschließen. Beim direkten
GPIO-Test externe Treiberausgänge abtrennen; nicht zwei aktive Ausgänge verbinden.

```text
GPIO21 TX → MAX3232 Sender → Kabel → Brücke
GPIO20 RX ← MAX3232 Empfänger ← Kabel ←┘
```

## Ergebnis

- **Blauer Impuls etwa einmal pro Sekunde:** Das letzte Paket kam fehlerfrei zurück.
- **LED aus:** Kein erfolgreicher Rücklauf. Fehlende Versorgung bleibt ebenfalls dunkel.
- Ohne Brücke darf die LED nicht blinken. Ein bereits laufender Impuls dauert
  noch höchstens 200 ms; erst das nächste Paket prüft die entfernte Brücke.

LED: GPIO8, aktiv LOW, wie beim üblichen SuperMini mit blauer Status-LED.
Andere Platinen können abweichende LED-Pins besitzen.

Der Test verwendet 38400 Baud, 8N1 und alle 256 Bytewerte. Das Muster verschiebt
sich pro Runde. Nur genau 256 passende Empfangsbytes ergeben PASS; fehlende,
falsche, zusätzliche oder alte Bytes ergeben FAIL. Empfangsfenster: 200 ms
nach Sendeende. USB mit 115200 Baud zeigt optional die Zähler und Fehlerdetails.
Ein PASS bestätigt nur den eingeschlossenen Signalweg, nicht die korrekte
Pinzuordnung zu einem Schachcomputer oder sämtliche elektrischen Grenzwerte.

## Selbst bauen

PlatformIO installieren, dieses Repository öffnen und ausführen:

```sh
pio run -e loopback
pio run -e loopback -t upload
```

Nur diese Testfirmware wird gebaut. Die Quellen liegen in `src/main.cpp`.
Das fertige Webflasher-Abbild einschließlich Bootloader und Partitionstabelle
liegt unter `docs/firmware/loopback-merged.bin` (Flashadresse 0).
Prüfsummen: `docs/firmware/SHA256SUMS`.

Nach einer Quelländerung das Web-Abbild erneuern:

```sh
python tools/build_web.py
```

Der Script benötigt PlatformIO im verwendeten Python und verwendet dessen
installiertes esptool. GitHub Pages veröffentlicht `main:/docs`.

## Herkunft und Lizenz

Aus dem UART-Diagnosetest von [eChessGw](https://github.com/parlue/eChessGw).
Firmware und Projektdokumentation: PolyForm Noncommercial 1.0.0, siehe LICENSE.
[ESP Web Tools](https://esphome.github.io/esp-web-tools/) wird vom CDN geladen
und steht unter seiner eigenen Lizenz. Keine Verbindung zu Boardherstellern.
