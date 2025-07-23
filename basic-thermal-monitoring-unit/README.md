# Thermal Monitoring Unit for Industrial Controllers (PoC)

## Overview

This repository contains the PoC of a **Thermal Monitoring Unit** developed for an industrial control system. It simulates a real-world embedded application running on the **STM32F401RE Nucleo board**, without any external peripherals or sensors.

This project demonstrates professional-level skills in:

* Bare-metal embedded development (no RTOS, no HAL)
* Direct register manipulation and peripheral control
* Cortex-M architecture (ARMv7-M) and STM32F4 peripheral usage

![](thermal-monitoring-unit.gif)

---

## 🎯 Objective

Design and implement a **temperature monitoring subsystem** capable of:

* Reading the **internal temperature sensor** via ADC1 channel 16
* Reporting system health via UART logs
* Triggering **visual thermal alarms** based on temperature thresholds
* Supporting operator-triggered diagnostic logs via EXTI/button
* Demonstrating precise timer-based scheduling, blinking, and debouncing

---

## 💡 Key Concepts

* **Bare-Metal Development** (no CMSIS/HAL abstractions)
* **ARM Cortex-M4 register-level programming**
* **ADC1** for internal temperature sensor (channel 16)
* **UART** serial communication with formatted logging
* **Timers (TIM2, TIM3, TIM5)** for:
  * Manual debounce
  * Periodic events
  * Alarm blinking
* **EXTI13** interrupt for user button (B1)
* **Integer-based fixed-point temperature conversion**
* **Stateful LED alarm patterns (1 Hz and 5 Hz blinking)**

---

## 🔍 How It Works

### Internal Temperature Sensing

* The STM32F401RE's **TSVREFE** bit is set in `ADC->CCR` to enable internal temperature sensor.
* ADC1 (channel 16) performs a single conversion, and the raw ADC value is converted to temperature using integer arithmetic.

### Logging System

* **UART** sends temperature logs every 5 seconds.
* Logs include severity levels: `[INFO]`, `[WARNING]`, and `[ERROR]`.

### LED Alarms

* When thresholds are exceeded:
  * **Warning**: LED blinks at 1 Hz
  * **Error**: LED blinks at 5 Hz
* TIM3 controls blinking patterns.

### Button Press & Debounce

* User can press the blue push-button to manually request a temperature reading.
* EXTI13 triggers TIM2 for a **500ms debounce window**.

### Timer Roles

| Timer | Role                       |
| ----- | -------------------------- |
| TIM2  | Debounce logic             |
| TIM3  | Blinking frequency control |
| TIM5  | 5-second log interval      |

### Diagrams

```mermaid
flowchart LR

    %% MAIN FLOW
    main[main] --> init[uart_init<br>led_init<br>btn_init<br>btn_irq_init<br>temp_sensor_init]
    init --> t5_start[tim5_start]
    t5_start --> main_loop{trigger_log}
    main_loop -->|true| handle_log[handle_log_task:<br>temp_sensor_read<br>adc_to_centi_celsius<br>evaluate_temperature<br>trigger_log = false<br>tim5_start]
    handle_log --> main_loop
    main_loop -->|false| main_loop

    %% TIM2
    tim2[TIM2 IRQ] --> tim2_expires{TIM2->SR & TIM_SR_UIF}
    tim2_expires-->|true| stopt2[stop TIM2<br>debounce_in_progress = false]
    tim2_expires-->|false| tim2_skip1[nothing]

    %% TIM3
    tim3[TIM3 IRQ] --> tim3_expires{TIM3->SR & TIM_SR_UIF}
    tim3_expires -->|true| ledact[stop TIM3<br>toggle LED]
    tim3_expires -->|false| tim3_skip1[nothing]
    ledact --> isw{led_warn_active}
    isw -->|true| stw[tim3_start WARN_MS]
    isw -->|false| ise{led_error_active}
    ise -->|true| ste[tim3_start ERROR_MS]
    ise -->|false| loff[led_off]

    %% TIM5
    tim5[TIM5 IRQ] --> tim5_expires{TIM5->SR & TIM_SR_UIF}
    tim5_expires -->|true| stopt5[stop TIM5<br>send_log_flag = true]
    tim5_expires -->|false| tim5_skip1[nothing]

    %% EXTI13
    exti[EXTI13 IRQ] --> clear_exti --> chkdeb{debounce_in_progress}
    chkdeb -->|true| skip1[nothing]
    chkdeb -->|false| startdeb[debounce_in_progress = true<br>start TIM2 - 500ms<br>send_log_flag = true]
```

---

## 🔧 Tools & Requirements

* **MCU**: STM32F401RE (Cortex-M4F)
* **Board**: STM32 Nucleo F401RE
* **Toolchain**: GCC ARM / STM32CubeIDE
* **Language**: C (bare-metal)
* **Debug**: ST-Link, UART terminal

---

## ⚠️ Common Pitfalls

* ADC values from internal temperature sensor vary significantly based on calibration and VREF.
* Debouncing must be handled carefully to avoid false triggers during EXTI handling.
* TIMx\_SR\_UIF (update interrupt flag) must be manually cleared.
