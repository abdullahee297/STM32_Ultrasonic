#include "general.h"
#include "defines.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"


volatile uint32_t msTicks;      //counts 1ms timeTicks
void SysTick_Handler(void) {
	msTicks++;
}

//  Delays number of Systicks (happens every 1 ms)
static void Delay(__IO uint32_t dlyTicks){                                              
  uint32_t curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

void setSysTick(void){
	// ---------- SysTick timer (1ms) -------- //
	if (SysTick_Config(SystemCoreClock / 1000)) {
		// Capture error
		while (1){};
	}
}

void InitGPIOs()
{
 //Initialize your Push Button here
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitTypeDef var1;    
     var1.GPIO_Mode=GPIO_Mode_OUT;
     var1.GPIO_OType=GPIO_OType_PP;
     var1.GPIO_PuPd=GPIO_PuPd_NOPULL;
     var1.GPIO_Speed=GPIO_Speed_50MHz;
     var1.GPIO_Pin=GPIO_Pin_14|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;
     
     GPIO_Init(GPIOD, &var1); 
}


// Global variables for Live Watch
volatile uint32_t time = 0;
volatile uint32_t distance = 0;
volatile float analogue_dist  = 0;

void InitUltrasonicGPIO(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    // Enable GPIOA clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // Configure Trigger pin (PA0) as output
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure Echo pin (PA1) as input
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void InitTimer(void) {
    TIM_TimeBaseInitTypeDef TIM_BaseStruct;

    // Enable TIM2 clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // Configure TIM2
    TIM_BaseStruct.TIM_Prescaler = 84 - 1; // 1 MHz timer frequency (1 탎 resolution)
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseStruct.TIM_Period = 0xFFFFFFFF; // Maximum counter value
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_BaseStruct);

    // Start TIM2
    TIM_Cmd(TIM2, ENABLE);
}

uint32_t MeasureEchoPulse(void) {
    uint32_t startTime, stopTime, pulseDuration;

    // Wait for Echo pin to go HIGH
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1));

    // Record the start time
    startTime = TIM_GetCounter(TIM2);

    // Wait for Echo pin to go LOW
    while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1));

    // Record the stop time
    stopTime = TIM_GetCounter(TIM2);

    // Calculate the pulse duration in microseconds
    if (stopTime >= startTime) {
        pulseDuration = stopTime - startTime;
    } else {
        pulseDuration = (0xFFFFFFFF - startTime) + stopTime;
    }

    return pulseDuration;
}

int main() {
    //uint32_t returnCode;

    // Initialize system clock
    //returnCode = Config_Systick_168MHz();

    setSysTick();

    // Initialize GPIOs for ultrasonic sensor
    InitUltrasonicGPIO();
    InitGPIOs(); // For LED output (GPIOD)

    // Initialize timer for pulse measurement
    InitTimer();

    while (1) {
        // Trigger pulse: Set Trigger pin HIGH for ~10탎
        GPIO_SetBits(GPIOA, GPIO_Pin_0);
        for (volatile int i = 0; i < 100; i++); // Short delay for ~10탎
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);

        // Measure Echo pulse duration
        time = MeasureEchoPulse();

        // Convert pulse duration to distance in cm
        // Speed of sound = 343 m/s, so 1 cm takes ~29.1 탎 round trip
        distance = time / 58;

        // Simulated analogue_dist calculation (if needed)
        analogue_dist = 25.0 + (distance * 0.1); // Example calculation

        // LED indication based on distance
        if (distance <= 10) {
            GPIO_SetBits(GPIOD, GPIO_Pin_13); // Turn on LED
        } else {
            GPIO_ResetBits(GPIOD, GPIO_Pin_13); // Turn off LED
        }

        // Small delay for stability
        Delay(100);
    }

    return 0;
}
