# 📏 Ultrasonic Distance Sensor (HC-SR04) with STM32F407

A high-precision distance measurement project using the **STM32F407VGT6 (Discovery Board)** and the **HC-SR04 ultrasonic sensor**. This implementation uses hardware **Input Capture** for microsecond accuracy rather than simple software delays.

---

## 🚀 Features
* **Precise Measurement:** Uses STM32 Timer Input Capture to measure the Echo pulse.
* **Metric Units:** Real-time distance calculation in centimeters (cm).
* **Non-Blocking:** Designed to run efficiently without stalling the CPU.
* **Interrupt Driven:** (Optional) Can be configured to trigger on Echo rising/falling edges.

## 🛠 Hardware Setup
### Components
* STM32F407 Discovery Board
* HC-SR04 Ultrasonic Sensor
* 5V Power Source (The HC-SR04 requires 5V, but STM32 pins used for Echo must be **5V tolerant**)

### Pin Mapping
| HC-SR04 Pin | STM32 Pin | Mode | Description |
| :--- | :--- | :--- | :--- |
| **VCC** | 5V | Power | 5V Supply |
| **GND** | GND | Ground | Common Ground |
| **Trig** | **PA1** | GPIO Out | 10µs Trigger Pulse |
| **Echo** | **PA0** | TIM2_CH1 | Input Capture (Timer 2) |

---

## ⚙️ How it Works

The HC-SR04 works on the "Time of Flight" principle:
1. The STM32 sends a **10µs HIGH pulse** to the `Trig` pin.
2. The sensor emits an 8-cycle sonic burst at **40kHz**.
3. The `Echo` pin goes **HIGH** the moment the burst is sent and **LOW** when the reflection is received.
4. The STM32 measures the duration ($T$) the Echo pin was high.

### The Math
The distance is calculated using the speed of sound ($343 m/s$ or $0.0343 cm/µs$):

$$Distance (cm) = \frac{Time (µs) \times 0.0343}{2}$$

---

## 📂 Installation & Usage

1. **Clone the Repo:**
   ```bash
   git clone [https://github.com/your-username/stm32-ultrasonic-hcsr04.git](https://github.com/your-username/stm32-ultrasonic-hcsr04.git)
