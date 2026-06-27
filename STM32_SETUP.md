# STM32 Toolchain Setup Guide

> **Verification status**: Toolchain research performed via web search (June 2026).
> No `arm-none-eabi-gcc` cross-compiler is available in the Claude Code environment,
> so all project code in this repo has been written and reviewed manually against
> confirmed STM32 HAL/CubeMX conventions — **not compiled or hardware-tested**.
> You must generate the CubeMX project skeleton, paste in the USER CODE sections,
> build, and test on real hardware.

---

## Section 0 — Toolchain Decision

### IDE Choice: STM32CubeIDE (Eclipse) vs STM32Cube for VS Code

**Research findings (June 2026):**

ST now maintains **two officially supported IDE paths**:

| Tool | Status | Notes |
|------|--------|-------|
| **STM32CubeIDE** (Eclipse-based) | Mature, stable | Most complete feature set; strongest debugger; somewhat monolithic/aging UI |
| **STM32Cube for VS Code** (extension pack) | Full release v3.0+ since early 2026 | Modern UI; actively developed; VS Code's superior code editing; debugger still catching up to Eclipse |

As of early 2026, the VS Code extension crossed from pre-release to **full release at v3.0.0+**, with ST actively pushing it as the forward path alongside STM32CubeIDE. ST explicitly positioned the March 2026 release as part of the "STM32C5 series launch and wider evolution of the STM32Cube ecosystem."

**Decision for this repo:**

> **Recommend STM32Cube for VS Code (extension pack v3.0+)** as the primary tool,
> since this repo already operates inside VS Code. It provides a modern, integrated
> experience without leaving your editor. If you encounter debugger limitations or
> need advanced Eclipse-only features, fall back to standalone STM32CubeIDE.
>
> Both tools work with the same CubeMX-generated project structure and the same
> USER CODE BEGIN/END conventions — switching between them does not invalidate
> any code in this repo.

### STM32CubeMX Version (November 2025 and later)

**Current version: STM32CubeMX 6.16.0** (released November 2025)

**Critical change as of November 2025**: STM32CubeMX is now **standalone only** — it is no longer bundled inside STM32CubeIDE. You must install CubeMX separately and launch it independently to generate project skeletons.

**USER CODE BEGIN/END blocks**: This convention is confirmed current in v6.16.0. All custom code in this repo is written to live within these blocks, so CubeMX regeneration (e.g. if you change pin assignments) will not erase your custom logic.

Example of the convention:
```c
/* USER CODE BEGIN Includes */
#include "my_driver.h"
/* USER CODE END Includes */

/* USER CODE BEGIN WHILE */
while (1)
{
    // Your main loop code here
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
}
/* USER CODE END 3 */
```

---

## Section 1 — Board Quick Reference

### Board 1: Blue Pill (STM32F103C8T6)

| Property | Value |
|----------|-------|
| CubeMX Part Number | **STM32F103C8T6** |
| Core | Cortex-M3 |
| Max Clock | 72 MHz |
| Flash / RAM | 64 KB / 20 KB |
| Onboard ST-Link | **No** — needs external ST-Link V2 or USB-serial adapter for UART bootloader |
| Onboard USB | No native USB application port (USB D+ pull-up resistor often missing or wrong) |
| Onboard LED | **PC13, active-LOW** |
| CAN bus | Yes (bxCAN on F103) |
| I2S | No |

**PC13 LED quirk**: PC13 is in the low-power/backup domain. When using it as a GPIO output, CubeMX may set the ASOE bit in `BKP_RTCCR` which hijacks the pin for RTC alarm/second output. If the LED behaves unexpectedly, clear that bit in your USER CODE init section:
```c
/* USER CODE BEGIN 2 */
__HAL_RCC_BKP_CLK_ENABLE();
HAL_PWR_EnableBkUpAccess();
BKP->RTCCR &= ~BKP_RTCCR_ASOE;  // Clear alarm/second output enable
/* USER CODE END 2 */
```

**CubeMX peripheral settings (Blue Pill common)**:
- System Core → RCC → HSE: Crystal/Ceramic Resonator (if 8 MHz crystal fitted; most Blue Pills have one)
- System Core → SYS → Debug: Serial Wire
- Clock Configuration: Set SYSCLK to 72 MHz via PLL

**Flashing without ST-Link (UART bootloader)**:
1. Move BOOT0 jumper to position **1** (toward 3.3V)
2. Connect USB-serial adapter: TX→PA10, RX→PA9, GND→GND
3. Press RESET button
4. Flash via STM32CubeProgrammer (UART connection)
5. Move BOOT0 back to **0** and press RESET to run firmware

**Flashing with external ST-Link V2**:
- Connect: SWDIO→PA13, SWCLK→PA14, GND→GND, 3.3V→3.3V
- BOOT0 stays at **0**
- Flash via STM32CubeIDE or STM32CubeProgrammer (ST-Link connection) — no jumper changes needed

---

### Board 2: Nucleo-F401RE

| Property | Value |
|----------|-------|
| CubeMX Part Number | **STM32F401RETx** (or select "NUCLEO-F401RE" as board in CubeMX) |
| Core | Cortex-M4F |
| Max Clock | 84 MHz |
| Flash / RAM | 512 KB / 96 KB |
| Onboard ST-Link | **Yes** (ST-Link V2-1 embedded; exposes virtual COM port) |
| Onboard USB | Yes (via ST-Link; application USB OTG FS on PA11/PA12) |
| Onboard User LED | **LD2 = PA5** |
| Onboard User Button | **B1 = PC13** (active-LOW, pull-up to 3.3V) |
| CAN bus | **No** — F4 series does NOT include CAN (F1/F3/F7/H7 do; F4 does not) |
| I2S | Yes |
| USB OTG FS | Yes (PA11/PA12) |

**Arduino shield header mapping** (commonly referenced):
- D13 = PA5 (onboard LED, also SPI SCK)
- D12 = PA6 (MISO), D11 = PA7 (MOSI)
- D10 = PB6 (SPI CS), A0 = PA0, A1 = PA1...A5 = PB1
- UART2 (used for ST-Link virtual COM): PA2 (TX), PA3 (RX)

**CAN bus note**: CAN is **not available** on STM32F401. Projects referencing CAN in the project list are substituted with an alternative — see Nucleo BASIC project 17's README for details.

**CubeMX board selection tip**: In CubeMX, you can either select the raw MCU `STM32F401RETx` or select the board `NUCLEO-F401RE` (under "Board Selector" tab). Selecting the board pre-configures the ST-Link UART and onboard LED/button pins, saving setup time.

**Flashing**: The onboard ST-Link handles everything. Connect the Nucleo via the CN1 USB connector (Mini-B or Micro-B depending on revision), and it appears as an ST-Link in CubeIDE / CubeProgrammer. No jumpers needed.

---

### Board 3: Black Pill (STM32F411CEU6)

| Property | Value |
|----------|-------|
| CubeMX Part Number | **STM32F411CEUx** |
| Core | Cortex-M4F |
| Max Clock | 100 MHz |
| Flash / RAM | 512 KB / 128 KB |
| Onboard ST-Link | **No** — uses USB DFU bootloader or external ST-Link |
| Onboard USB | **Yes** — native USB Full-Speed on PA11/PA12; primary flashing method |
| Onboard LED | **PC13, active-LOW** (same as Blue Pill physical placement) |
| BOOT0 Button | Onboard button labeled "BOOT0" (WeAct board) |
| NRST Button | Onboard RESET button |
| CAN bus | **No** — same F4 limitation as Nucleo |
| I2S | Yes (SPI1/SPI2/SPI3 can be configured as I2S) |
| USB OTG FS | Yes (PA11/PA12) |

**DFU flashing procedure (no ST-Link needed)**:
1. Press and hold the **BOOT0** button
2. While holding BOOT0, briefly press and release the **NRST** (Reset) button
3. Wait ~0.5 seconds, then release BOOT0
4. Board enters USB DFU mode — appears as "STM32 BOOTLOADER" in Device Manager
5. Flash using **STM32CubeProgrammer** (USB DFU connection) or `dfu-util`
6. Press NRST to exit DFU mode and run firmware

**Known WeAct board quirk**: PA10 (UART1 RX) is left floating on the WeAct Black Pill PCB. This makes it susceptible to noise during startup, which can cause the MCU to accidentally enter UART ISP mode. Add a 10kΩ pull-up resistor from PA10 to 3.3V if you experience erratic boot behavior.

**USB CDC application note**: The Black Pill's native USB means you can implement USB CDC (virtual COM port from your application firmware) without any external adapters. This is a primary differentiator vs Blue Pill. The ST-Link virtual COM on Nucleo achieves the same ergonomic result but through different hardware.

---

## Section 2 — Software Installation

### Option A: STM32Cube for VS Code (Recommended for this repo)

1. Open VS Code → Extensions → search **"STM32CubeIDE for Visual Studio Code"** (publisher: STMicroelectronics)
2. Install the extension pack — it installs:
   - STM32CubeIDE for Visual Studio Code
   - ST MCU Finder
   - C/C++ extension
   - CMake Tools
3. On first launch, the extension prompts you to install required components (arm-none-eabi toolchain, OpenOCD/ST-Link GDB server, STM32CubeMX standalone)
4. Install STM32CubeMX **separately** (mandatory since November 2025 — it is no longer bundled):
   - Download from [st.com/stm32cubemx](https://www.st.com/en/development-tools/stm32cubemx.html)
   - Current version: **6.16.0** or later

### Option B: Classic STM32CubeIDE (Eclipse)

1. Download from [st.com/stm32cubeide](https://www.st.com/en/development-tools/stm32cubeide.html)
2. Run installer — includes arm-none-eabi toolchain, GDB, OpenOCD, ST-Link server
3. Install STM32CubeMX **separately** (mandatory since November 2025)

### ST-Link Drivers

- **Fresh STM32CubeIDE install**: ST-Link drivers are installed automatically
- **STM32CubeProgrammer**: Includes its own "Install Drivers" option under Help menu
- **Standalone driver package**: STSW-LINK009 (download from st.com) — needed if you only want the driver without the full IDE
- **ST-Link V3 on Windows**: No separate `.inf` driver required — Windows recognizes it natively
- **ST-Link V2 on Windows**: Run STSW-LINK009 installer before connecting hardware

### STM32CubeProgrammer

Useful for flashing without the full IDE (especially for Blue Pill UART mode and Black Pill DFU mode).
Download from [st.com/stm32cubeprog](https://www.st.com/en/development-tools/stm32cubeprog.html).

---

## Section 3 — CubeMX Workflow (Per Project)

Each project's `README.md` specifies which peripherals to enable. The general CubeMX workflow is:

### Step 1 — Create New Project
1. Open STM32CubeMX standalone
2. Click **"ACCESS TO MCU SELECTOR"** or **"ACCESS TO BOARD SELECTOR"**
3. Enter the part number from the board table above
4. Double-click to open the configuration

### Step 2 — Configure Peripherals
Follow the per-project README for exact settings. General pattern:
- **Pinout & Configuration tab**: Click peripheral (e.g. I2C1) → enable as I2C → assign pins shown in README
- **Clock Configuration tab**: Set to max clock for your board (72 MHz / 84 MHz / 100 MHz via PLL)
- **Project Manager tab**: Set project name, location, toolchain (STM32CubeIDE or Makefile)

### Step 3 — Generate Code
- Click **"GENERATE CODE"** button
- CubeMX creates `Core/Src/main.c`, `Core/Inc/main.h`, and peripheral init files
- Each generated file contains `/* USER CODE BEGIN xxx */` / `/* USER CODE END xxx */` comment pairs

### Step 4 — Add Project Code
Open the generated `main.c` and find the USER CODE sections. Copy the blocks from this repo's `main_usercode.c` into the corresponding sections. Each block in `main_usercode.c` is labeled with a comment like:
```c
// ===== USER CODE BEGIN Includes =====
// (paste this block into main.c USER CODE BEGIN Includes)
```

### Step 5 — Build and Flash
- **VS Code extension**: Build button in status bar, Flash via Run & Debug
- **STM32CubeIDE**: Build (Ctrl+B), Debug/Run (F11)
- **STM32CubeProgrammer**: Connect to board, select binary `.elf` or `.hex`, click "Download"

---

## Section 4 — Safety Notes

### 3.3V Logic — Critical Warning

All three boards use **3.3V logic levels**. Many common modules and sensors are designed for 5V systems (Arduino Uno, etc.).

| Scenario | Risk |
|----------|------|
| Connecting a 5V signal to any GPIO pin | Permanent MCU damage |
| Powering a 5V-only sensor from 3.3V | Sensor may not work or give wrong readings |
| Level-shifting a 5V module to connect to STM32 | Safe with proper level shifter (e.g. BSS138-based bidirectional shifter) |

**Safe modules**: Most I2C/SPI sensors sold for Arduino also work at 3.3V (check the datasheet). Commonly safe: BMP280, MPU6050, SSD1306 OLED, most DHT22 modules, HC-SR04 (output needs level shift).

### Power Supply
- Blue Pill: 5V via USB (on-board 3.3V LDO) or direct 3.3V on the 3.3V pin
- Nucleo: 5V via USB CN1; CN6/CN7 headers expose 3.3V and 5V rails
- Black Pill: 5V via USB-C (WeAct board) → on-board 3.3V LDO; or 3.3V direct

### Common Mistakes
1. **Blue Pill PC13 LED polarity**: `HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)` turns the LED **ON** (active-low)
2. **BOOT0 left at 1**: Board runs bootloader instead of your firmware after reset
3. **Forgetting external ST-Link for Blue Pill**: Cannot flash via USB without a USB-CDC bootloader pre-installed; you need either an ST-Link V2 clone or a USB-serial adapter for UART bootloader
4. **F4 boards have no CAN**: If you need CAN bus, use the Blue Pill (F103 has bxCAN)
5. **Black Pill PA10 floating**: Can cause accidental UART ISP entry; add pull-up if needed

---

## Section 5 — FreeRTOS Setup via CubeMX

Used in all ADVANCED project tracks. CubeMX makes FreeRTOS setup straightforward:

1. In CubeMX **Middleware and Software Packs** → select **FreeRTOS**
2. Interface: **CMSIS_V2** (more modern; wraps FreeRTOS in a standard CMSIS-RTOS2 API)
3. Under **Tasks and Queues** tab: add tasks, set stack sizes and priorities
4. CubeMX generates `freertos.c` with task creation and the scheduler start in `MX_FREERTOS_Init()`
5. **Do not** put your task code directly in the CubeMX-generated task function bodies — use USER CODE blocks inside them, or create separate source files and call them from within USER CODE blocks
6. Enable the **USE_TRACE_FACILITY** and **USE_STATS_FORMATTING_FUNCTIONS** config options in FreeRTOSConfig.h if you want runtime stats (useful for ADVANCED projects)

### Heap size
Default FreeRTOS heap (configTOTAL_HEAP_SIZE) is 3072 bytes in CubeMX — increase to 8192–16384 for projects with multiple tasks, queues, and mutexes. Adjust in CubeMX FreeRTOS config panel.

---

*All code in this repo: written following STM32 HAL/CubeMX 6.16 conventions, manually reviewed. Not compiled or hardware-tested — ready for you to generate the CubeMX project, paste in USER CODE sections, and test on real hardware.*
