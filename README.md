# Blink LED with Button and Timer Interruption

This project makes the LED LD2 of the Nucleo-STM32F446RE at a frequency of `1/c` Hz. The frequency is controlled by the push of the user button B1. Each time the button is pushed, the frequency is divided by 2 (`c` increases by 1). The LED is off when `c` equals 0. The button is managed by an interrupt service routine (ISR). The time blink is managed by timer interruption.

The system uses the TIM2 interrupt to detect the end of the note. The interrupt is configured with the following settings:

| Parameter   | Value           |
| ----------- | --------------- |
| Interrupt   | TIM2_IRQHandler |
| Priority    | 1               |
| Subpriority | 0               |

## Ejercicio: parpadeo con interrupción del TIM2

**Cree el proyecto** `blink_led_interr_button_interr`. **Se trata de una modificación del programa** `blink_led_button_interr` **donde la acción de esperar no se toma al leer mediante polling el valor del** `SysTick`, **sino mediante una interrupción del *timer* 2** (`TIM2`).

1. **Crear la función** `port_led_timer_setup()` **que inicializa el *timer* 2** (`TIM2`) **y hacerla pública.**

- Habilitar la **fuente de reloj** del *timer* 2: `RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN`.
- **Deshabilitar** el *timer* 2 limpiando el bit `CEN` del registro `CR1`.
- Poner el contador `CNT`, el *prescaler* `PSC` y el periodo `ARR` a 0.
- **Limpiar las interrupciones** pendientes en el bit `UIF` del registro `SR`.
- **Habilitar la interrupción** del *timer* 2: `TIM2->DIER |= TIM_DIER_UIE`.
- Configurar la **prioridad** de la interrupción del *timer* 2 a 2 y la subprioridad a 0 en el *NVIC*: 
  
  `NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0))`.
- Habilitar la **interrupción global del periférico** en el *NVIC*: `NVIC_EnableIRQ(TIM2_IRQn)`.

2 . **Crear la función** `port_led_timer_delay_ms(uint32_t delay_ms)` **que implementa la espera mediante la interrupción periódica del *timer* 2 al valor** `delay_ms`, **y hacerla pública.**

- **Deshabilitar** el *timer* 2 limpiando el bit `CEN` del registro `CR1` para no tener interrupciones mientras se configura.
- Poner el contador `CNT` a 0.
- **Calcular el valor del periodo** `ARR` **en función del** *prescaler* `PSC`.
  
  - La ecuación del periodo de interrupción del *timer* es `T_interr = f_clk / ((PSC + 1) * (ARR + 1))`.
  - La frecuencia del reloj del sistema es 16 MHz y está definida en la variable `SystemCoreClock`.
  - **Comprobar** que el valor del periodo `ARR` ni el *prescaler* `PSC` superan el valor máximo de 16 bits (65535).
  - Tener en cuenta que el valor de interrupción recibido es en milisegundos y hay que **convertirlo a segundos**.
  - **Trabajar con precisión doble en todas las variables y constantes.** *castear* los valores de la frecuencia del reloj del sistema y del periodo de interrupción a `double`. *e.g.*:
    - `double f_clk = (double)SystemCoreClock`
    - `double T_interr = (double)delay_ms / 1000.0`
    - `1.0` en lugar de `1` para que el resultado sea `double`.
  - **Forzar la actualización de registros** activando el bit `UG`: `TIM2->EGR |= TIM_EGR_UG`.
  - **Habilitar el *timer* 2** limpiando el bit `CEN` del registro `CR1`
  
3 . **En el fichero** `interr.c`, **implementar la función** `TIM2_IRQHandler()` **que se ejecuta cuando se produce la interrupción del *timer* 2.**

  - Cuando se produce la interrupción, **limpiar el bit** `UIF` del registro `SR`.
  - Llamar a la función `port_led_toggle()` para cambiar el estado del LED si el contador de pulsaciones `c` es distinto de 0.
  - Gestionar el indicador de botón pulsado `button_pressed` adecuadamente.
  
   - **En la ISR del botón, llamar a la función** `port_led_timer_delay_ms()` con el valor adecuado para el parpadeo del LED.

4 . **En** `main.c`:

  - **Quitar** del `main()` la gestión del parpadeo del LED.
  - Como las variables `c` y `button_pressed` se gestionan en las ISRs, **quitamos su declaración y en** `interr.c` **ya no hace falta declararlas como** `extern`.
  - **Llamar a la función** `port_led_timer_setup()` en el `main()` para inicializar el *timer* 2.

## References

- **[1]**: [Documentation available in the Moodle of the course](https://moodle.upm.es/titulaciones/oficiales/course/view.php?id=785#section-0)
- **[2]**: [Embedded Systems with ARM Cortex-M Microcontrollers in Assembly Language and C (Fourth Edition)](https://web.eece.maine.edu/~zhu/book/index.php) for explanations and examples of use of the ARM Cortex-M microcontrollers in C with CMSIS.
- **[3]**: [Programming with STM32: Getting Started with the Nucleo Board and C/C++](https://ingenio.upm.es/primo-explore/fulldisplay?docid=34UPM_ALMA51126621660004212&context=L&vid=34UPM_VU1&lang=es_ES&search_scope=TAB1_SCOPE1&adaptor=Local%20Search%20Engine&tab=tab1&query=any,contains,Programming%20with%20STM32:%20Getting%20Started%20with%20the%20Nucleo%20Board%20and%20C%2FC%2B%2B&offset=0) for examples of use of the STM32 microcontrollers with the HAL of ST.
- **[4]**: [The C Programming Language](https://ingenio.upm.es/primo-explore/fulldisplay?docid=34UPM_ALMA2151866130004212&context=L&vid=34UPM_VU1&lang=es_ES&search_scope=TAB1_SCOPE1&adaptor=Local%20Search%20Engine&isFrbr=true&tab=tab1&query=any,contains,C%20Programming%20Language)
- **[5]**: [Nucleo Boards Programming with th STM32CubeIDE](https://www.elektor.com/products/nucleo-boards-programming-with-the-stm32cubeide) for examples of use of the STM32 microcontrollers with the STM32CubeIDE.
