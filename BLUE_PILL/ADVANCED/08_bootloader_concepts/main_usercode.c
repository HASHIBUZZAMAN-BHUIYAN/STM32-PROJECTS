/**
 * main_usercode.c — 08_bootloader_concepts
 *
 * Two separate code regions, presented in one file for study.
 *
 * SECTION A: Bootloader (link at 0x08000000, max 4 KB)
 *   - Checks PB0 on startup
 *   - If pressed: stays in UART bootloader waiting loop
 *   - If released: validates and jumps to application at 0x08001000
 *
 * SECTION B: Application (separate project, link at 0x08001000)
 *   - Relocates VTOR to 0x08001000
 *   - Blinks LED on PC13 rapidly
 *
 * NOT tested on hardware. Reference implementation only.
 *
 * IMPORTANT: The bootloader and application are TWO separate CubeMX/CubeIDE
 * projects with different linker scripts. This file shows the user code for
 * each. Do not mix them in a single project.
 */


/* ========================================================================
 * SECTION A: BOOTLOADER CODE
 * Place in the bootloader project (FLASH origin = 0x08000000, length = 4K)
 * ======================================================================== */

// ===== USER CODE BEGIN Includes (bootloader) =====
#include <stdint.h>
#include <string.h>
// ===== USER CODE END Includes (bootloader) =====


// ===== USER CODE BEGIN PD (bootloader) =====
#define APP_START_ADDR      0x08001000UL
#define BOOTLOADER_UART_MS  5000U   /* Wait up to 5 s for host in BL mode */

/* Simple SP sanity check: must be inside RAM (0x20000000 .. 0x20005000) */
#define RAM_START           0x20000000UL
#define RAM_END             0x20005000UL
// ===== USER CODE END PD (bootloader) =====


// ===== USER CODE BEGIN 0 (bootloader — before main) =====

/**
 * Disable all interrupts, reset peripherals, and jump to the application.
 * Must be called with interrupts disabled or from a clean state.
 */
static void jump_to_app(uint32_t app_addr)
{
    /* Read application stack pointer and reset handler from vector table */
    uint32_t app_sp = *(volatile uint32_t *)(app_addr);
    uint32_t app_pc = *(volatile uint32_t *)(app_addr + 4U);

    /* Validate stack pointer is within RAM */
    if (app_sp < RAM_START || app_sp > RAM_END) {
        /* Invalid — do not jump */
        return;
    }

    /* Disable all IRQs and clear pending flags */
    __disable_irq();
    for (uint32_t i = 0U; i < 8U; i++) {
        NVIC->ICER[i] = 0xFFFFFFFFUL;
        NVIC->ICPR[i] = 0xFFFFFFFFUL;
    }

    /* Reset SysTick */
    SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL  = 0U;

    /* Set Main Stack Pointer */
    __set_MSP(app_sp);

    /* Re-enable interrupts (application sets up its own NVIC) */
    __enable_irq();

    /* Jump to application reset handler */
    void (*app_reset)(void) = (void (*)(void))app_pc;
    app_reset();

    /* Should never reach here */
    while (1) { ; }
}

/**
 * Minimal UART polling transmit (no HAL dependency, for robustness).
 * Assumes USART1 already configured by HAL init.
 */
static void bl_uart_send_str(const char *str)
{
    while (*str != '\0') {
        /* Wait for TXE (Transmit Data Register Empty) */
        while (!(USART1->SR & USART_SR_TXE)) { ; }
        USART1->DR = (uint8_t)(*str++);
    }
}
// ===== USER CODE END 0 (bootloader) =====


// ===== USER CODE BEGIN 2 (bootloader — inside main, after MX inits) =====
/*
    // PB0 configured as Input Pull-up in CubeMX
    // Active LOW: button connects PB0 to GND

    HAL_Delay(10U); // Debounce

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET) {
        // Button pressed — stay in bootloader
        bl_uart_send_str("\r\n[BL] Bootloader mode. Waiting for host...\r\n");

        uint32_t start = HAL_GetTick();
        while ((HAL_GetTick() - start) < BOOTLOADER_UART_MS) {
            // Placeholder: a real bootloader would implement UART
            // command parsing here (e.g., XMODEM, custom protocol).
            // For demonstration, simply wait and then fall through.
            HAL_Delay(100U);
        }
        bl_uart_send_str("[BL] Timeout. Jumping to app.\r\n");
    }

    // Attempt jump to application
    bl_uart_send_str("[BL] Jumping to application...\r\n");
    HAL_Delay(10U); // Let UART drain

    jump_to_app(APP_START_ADDR);

    // jump_to_app returns only if SP is invalid
    bl_uart_send_str("[BL] ERROR: No valid application at 0x08001000\r\n");
    while (1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(100U); // Rapid blink — error indicator
    }
*/
// ===== USER CODE END 2 (bootloader) =====


/* ========================================================================
 * SECTION B: APPLICATION CODE
 * Separate CubeMX project. Linker script: FLASH origin = 0x08001000
 * ======================================================================== */

// ===== USER CODE BEGIN 2 (application — inside main, after MX inits) =====
/*
    // Relocate vector table to application base address.
    // Required so NVIC dispatches to the correct ISR handlers.
    SCB->VTOR = 0x08001000UL;

    // Optional: transmit a startup message
    // HAL_UART_Transmit(&huart1, (uint8_t *)"[APP] Running!\r\n", 16, 50);
*/
// ===== USER CODE END 2 (application) =====


// ===== USER CODE BEGIN WHILE (application) =====
/*
    // Rapid LED blink — visual confirmation the application is running
    while (1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // PC13, Active-LOW
        HAL_Delay(100U);
    }
*/
// ===== USER CODE END WHILE (application) =====
