# Black Pill BASIC/15 — Multiple Timer Channels (TIM1 CH1-CH4)

## Overview
Uses all four channels of TIM1 (an advanced timer) to generate four independent PWM signals at different duty cycles simultaneously. Demonstrates multi-channel PWM configuration and the concept of the complementary output (CH1N) available on advanced timers.

## TIM1 Channels on Black Pill
| Channel | Pin | Notes |
|---------|-----|-------|
| TIM1_CH1 | PA8 | PWM output |
| TIM1_CH2 | PA9 | PWM output (shares with USART1_TX — avoid if UART needed) |
| TIM1_CH3 | PA10 | PWM output (shares with USART1_RX) |
| TIM1_CH4 | PA11 | PWM output (shares with USB_DM) |
| TIM1_CH1N | PB13 | Complementary of CH1 |
| TIM1_CH2N | PB14 | Complementary of CH2 |
| TIM1_CH3N | PB15 | Complementary of CH3 |

**Note**: For this project, use CH1 (PA8) and CH4 (PA11) to avoid conflicts with USART1 and USB.

## CubeMX Configuration
| Item | Setting |
|------|---------|
| TIM1 | PSC=99, ARR=999 → 1 kHz PWM |
| CH1 | PWM Mode 1, CCR1=100 (10%) |
| CH4 | PWM Mode 1, CCR4=700 (70%) |
| USART1 | Disabled (CH2/CH3 conflict) OR use on separate project |

## PWM Duty Cycle
CCR/ARR × 100 = duty %:
- CH1: CCR=100 → 10%
- CH4: CCR=700 → 70%

## Expected Behavior
- PA8 outputs 10% duty 1 kHz PWM → dim LED
- PA11 outputs 70% duty 1 kHz PWM → bright LED
- Duty cycles increment/decrement over time creating chase effect

## Advanced Timer Features
TIM1 is an "advanced" timer. Beyond basic PWM:
- Dead-time insertion for H-bridge control
- Break input (fault detection)
- Complementary outputs for 3-phase motor control
- These features are NOT used in this basic demo
