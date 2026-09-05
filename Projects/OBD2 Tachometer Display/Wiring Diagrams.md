# Wiring Diagrams — OBD2 Tachometer Display

Mermaid versions of the harness drawings. Source of truth for pin numbers is the [[OBD2 Tachometer Display]] note, §7 pin plan.

> [!warning] Step 0 first
> Everything below assumes the bike has the **6-pin** DLC (CAN). Verify by eye before trusting any of it.

---

## 1. System overview

```mermaid
flowchart LR
    subgraph BIKE["Bike"]
        DLC["Honda 6-pin DLC<br/>(red, under seat)"]
        BAT["Battery / fuse tap<br/>12–14.5 V"]
        SW["Switched circuit<br/>(tail light / switched fuse)"]
    end

    subgraph SIG["Signal path — CAN-H / CAN-L only"]
        ADP["Honda 6-pin → OBD2 16-pin<br/>adapter cable"]
        SPL["OBD2 splitter<br/>1 male → 2 female"]
        PB["Port B<br/>free for scanner"]
        PIG["Device pigtail<br/>pins 6 + 14 only"]
        XCV["SN65HVD230<br/>3.3 V transceiver<br/>120 Ω terminator removed"]
    end

    subgraph PWR["Power path — separate circuit"]
        FUSE["2 A inline fuse"]
        RLY["Ignition relay<br/>SPST"]
        TVS["TVS SMBJ18A<br/>across buck input"]
        BUCK["12 V → 5 V buck<br/>2 A automotive"]
    end

    subgraph DEV["Device"]
        MCU["ESP32-S3 N16R8<br/>TWAI listen-only<br/>500 kbps · 29-bit"]
        LCD["1.28 inch GC9A01<br/>240×240 round<br/>SPI"]
        BH["BH1750<br/>light sensor · I²C"]
        BTN["2× buttons<br/>NEXT / BACK<br/>RC debounced"]
    end

    DLC --> ADP --> SPL
    SPL --> PB
    SPL --> PIG -->|twisted pair ≤ 30 cm| XCV -->|TX · RX| MCU
    BAT --> FUSE --> RLY --> TVS --> BUCK -->|5 V| MCU
    SW -->|coil ~150 mA| RLY
    MCU -->|SPI + PWM BL| LCD
    BH -->|SDA · SCL| MCU
    BTN --> MCU
```

---

## 2. Signal harness (RT-HARN-SIG rev B)

Two conductors only. Listen-only — the device never transmits. Mated connector pairs are shown as one junction.

```mermaid
flowchart LR
    X1["X1 · Honda 6-pin DLC<br/>CAN-H pin ?<br/>CAN-L pin ?<br/><i>Step 0 — not yet verified</i>"]
    X2["X2 · OBD-II junction J1962<br/>adapter female + splitter<br/>pin 6 = CAN-H<br/>pin 14 = CAN-L"]
    X3["X3 · Device pigtail<br/>J1962 male, solderable<br/>pads 6 + 14 only<br/><i>nothing on 16 / 4 / 5</i>"]
    X4["X4 · SN65HVD230<br/>CANH · CANL<br/>3V3 · GND · D · R<br/>RS → GND · Vref open"]
    X5["X5 · ESP32-S3 N16R8<br/>3V3 · GND<br/>TWAI-TX GPIO5<br/>TWAI-RX GPIO4"]

    X1 -->|"W1 · purchased adapter<br/>0.5 mm² · 0.4 m"| X2
    X2 -->|"WM · mated pair<br/>no wire"| X3
    X3 -->|"W2 · CAN stub<br/>twisted 2–3 turns/cm<br/>0.5 mm² · ≤ 0.25 m"| X4
    X4 -->|"W3 · logic tail<br/>4 × 0.34 mm² · 0.15 m"| X5
```

**Check before proceeding:** ignition off, ~60 Ω between X2 pin 6 and pin 14 (two 120 Ω terminators in parallel). Open circuit → the adapter's CAN pins aren't populated.

---

## 3. OBD2 pigtail → transceiver → ESP32 (pin level)

Six wires total: two to the bike, four to the ESP32. One jumper stays on the module.

```mermaid
flowchart LR
    subgraph OBD["OBD2 pigtail (J1962 male)"]
        P6["pin 6 · CAN-H"]
        P14["pin 14 · CAN-L"]
        P16["pin 16 · +12 V — NOT USED"]
        P45["pin 4/5 · GND — NOT USED"]
    end

    subgraph TR["SN65HVD230 breakout"]
        CANH["CANH"]
        CANL["CANL"]
        TERM["120 Ω terminator<br/>❌ DESOLDERED"]
        V33["3V3"]
        GND["GND"]
        D["D (TXD in)"]
        R["R (RXD out)"]
        RS["RS"]
        VREF["Vref — open"]
    end

    subgraph ESP["ESP32-S3 N16R8"]
        E33["3V3"]
        EGND["GND"]
        E5["GPIO5 · TWAI TX"]
        E4["GPIO4 · TWAI RX"]
    end

    P6 ==>|twisted pair| CANH
    P14 ==>|twisted pair| CANL
    V33 --- E33
    GND --- EGND
    E5 --> D
    R --> E4
    RS -.->|jumper to GND<br/>high-speed mode| GND

    style P16 fill:#fdd,stroke:#c00
    style P45 fill:#fdd,stroke:#c00
    style TERM fill:#fdd,stroke:#c00
```

---

## 4. Power harness (RT-HARN-PWR rev B)

Independent of the signal path — nothing here touches the diagnostic connector. DIN 72552 terminal numbers: 30 = permanent battery, 15 = switched, 31 = ground.

```mermaid
flowchart TB
    X1["X1 · Battery / fuse tap<br/>terminal 30 · 12–14.5 V"]
    X2["X2 · Inline fuse holder<br/>2 A blade<br/><i>mount at battery end</i>"]
    X3["X3 · Switched circuit<br/>terminal 15 · tail light / switched fuse<br/><i>Step 5 — circuit not yet chosen</i>"]
    X4["X4 · Ignition relay SPST<br/>30 batt in · 87 switched out<br/>85 coil − · 86 coil +"]
    X5["X5 · Buck converter 12→5 V · 2 A<br/>TVS SMBJ18A across IN+ / IN−"]
    X6["X6 · Ground distribution<br/>terminal 31 · one node, one return<br/><i>no ground from OBD pins 4/5</i>"]
    X7["X7 · ESP32-S3 N16R8<br/>5V IN · GND · 3V3<br/>SDA GPIO8 · SCL GPIO18<br/>BTN-A GPIO1 · BTN-B GPIO2"]
    X8["X8 · Button NEXT<br/>IP67 16 mm · NO"]
    X9["X9 · Button BACK<br/>IP67 16 mm · NO"]
    X10["X10 · BH1750<br/>VCC · GND · SDA · SCL"]

    X1 -->|"W1 · RD 0.75 mm²"| X2
    X2 -->|"W2 · RD 0.75 mm² → pin 30"| X4
    X3 -->|"W3 · BU/BK 0.5 mm² → 86 / 85"| X4
    X4 -->|"W4 · RD 0.75 mm² · 87 → IN+"| X5
    X6 -->|"W4 · BK → IN−"| X5
    X1 -->|"W5 · BK 0.75 mm² · GND → chassis"| X6
    X5 -->|"W6 · RD/BK 0.75 mm² · 0.8 m<br/>OUT+ → 5V IN · OUT− → GND"| X7
    X8 -->|"W7 · BN/BK 0.34 mm² · 1 m<br/>handlebar run"| X7
    X9 -->|"W8 · PK/BK 0.34 mm² · 1 m<br/>handlebar run"| X7
    X10 -->|"W9 · RD/BK/WH/GY 0.34 mm²<br/>3V3 · GND · SDA · SCL"| X7
```

**Button RC (each):** 10 kΩ pull-up to 3V3, 100 nF to GND at the GPIO, switch pulls low. ~1 ms time constant. Not optional — the handlebar run is an antenna next to the ignition leads.

**Additional BOM:** TVS SMBJ18A ×1 (at X5), 10 kΩ ×2, 100 nF ×2.

---

## 5. ESP32-S3 N16R8 pin map

```mermaid
flowchart LR
    subgraph MCU["ESP32-S3 N16R8"]
        G4["GPIO4"]
        G5["GPIO5"]
        G9["GPIO9"]
        G10["GPIO10"]
        G11["GPIO11"]
        G12["GPIO12"]
        G13["GPIO13"]
        G14["GPIO14"]
        G8["GPIO8"]
        G18["GPIO18"]
        G1["GPIO1"]
        G2["GPIO2"]
        NO["⛔ GPIO 0 · 3 · 45 · 46 — strapping<br/>⛔ GPIO 35 · 36 · 37 — octal PSRAM"]
    end

    subgraph CAN["SN65HVD230"]
        R["R"]
        D["D"]
    end

    subgraph LCD["GC9A01 1.28 inch LCD"]
        SCLK["SCLK"]
        MOSI["MOSI / SDA"]
        DC["DC"]
        CS["CS"]
        RST["RST"]
        BL["BL"]
        TP["TP_SDA · TP_SCL · TP_INT · TP_RST<br/>— leave unconnected"]
    end

    subgraph I2C["BH1750"]
        SDA["SDA"]
        SCL["SCL"]
    end

    subgraph BTN["Buttons"]
        NEXT["NEXT"]
        BACK["BACK"]
    end

    R --> G4
    G5 --> D
    G12 --> SCLK
    G11 --> MOSI
    G10 --> DC
    G9 --> CS
    G14 --> RST
    G13 -->|LEDC PWM| BL
    G8 <--> SDA
    G18 --> SCL
    NEXT --> G1
    BACK --> G2

    style NO fill:#fdd,stroke:#c00
    style TP fill:#eee,stroke:#999,stroke-dasharray: 5 5
```

LCD VCC → 3V3, GND → GND. Transceiver and BH1750 also on 3V3.

---

## 6. Bench CAN test rig (no bike)

```mermaid
flowchart LR
    TXB["ESP32-WROOM-32<br/>TWAI normal mode<br/>replays recorded frames"]
    T1["SN65HVD230 #1<br/>120 Ω kept"]
    T2["SN65HVD230 #2<br/>120 Ω kept"]
    RXB["ESP32-S3 N16R8<br/>TWAI listen-only<br/>→ GC9A01 gauge"]

    TXB --> T1
    T1 -->|"twisted pair, under 30 cm, terminated both ends"| T2
    T2 --> RXB
```

On the bench **both** transceivers keep their 120 Ω terminators — it's a two-node bus. Remove the one on the bike-side module only when it goes on the real bus.
