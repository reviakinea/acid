#include "timer1.h"

static void timer1_gpio_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN;

    GPIOA->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9   | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF8_1 | GPIO_CRH_CNF9_1 | GPIO_CRH_CNF10_1;
    GPIOA->CRH |= GPIO_CRH_MODE8  | GPIO_CRH_MODE9  | GPIO_CRH_MODE10;
    
    GPIOB->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_CNF14   | GPIO_CRH_CNF15);
    GPIOB->CRH |= GPIO_CRH_CNF13_1 | GPIO_CRH_CNF14_1 | GPIO_CRH_CNF15_1;
    GPIOB->CRH |= GPIO_CRH_MODE13  | GPIO_CRH_MODE14  | GPIO_CRH_MODE15;
}

#define TIMER1_OUTPUT_DISABLE 0
#define TIMER1_OUTPUT_ENABLE  1
static void timer1_set_output_enable(int state)
{
    if (state == TIMER1_OUTPUT_ENABLE) {
        TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1NE;
        TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2NE;
        TIM1->CCER |= TIM_CCER_CC3E | TIM_CCER_CC3NE;
    } else {
        TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE);
        TIM1->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE);
        TIM1->CCER &= ~(TIM_CCER_CC3E | TIM_CCER_CC3NE);
    }
}

#define TIMER1_POLARITY_NORMAL   0
#define TIMER1_POLARITY_INVERTED 1
static void timer1_set_output_polarity(int polarity)
{
    if (polarity == TIMER1_POLARITY_INVERTED) {
        /* output idle state HIGHT */
        TIM1->CR2 |= TIM_CR2_OIS1 | TIM_CR2_OIS1N;
        TIM1->CR2 |= TIM_CR2_OIS2 | TIM_CR2_OIS2N;
        TIM1->CR2 |= TIM_CR2_OIS3 | TIM_CR2_OIS3N;

        /* active LOW */
        TIM1->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1NP;
        TIM1->CCER |= TIM_CCER_CC2P | TIM_CCER_CC2NP;
        TIM1->CCER |= TIM_CCER_CC3P | TIM_CCER_CC3NP;
    } else {
        /* output idle state LOW */
        TIM1->CR2 &= ~(TIM_CR2_OIS1 | TIM_CR2_OIS1N);
        TIM1->CR2 &= ~(TIM_CR2_OIS2 | TIM_CR2_OIS2N);
        TIM1->CR2 &= ~(TIM_CR2_OIS3 | TIM_CR2_OIS3N);

        /* active HIGHT */
        TIM1->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
        TIM1->CCER &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
        TIM1->CCER &= ~(TIM_CCER_CC3P | TIM_CCER_CC3NP);
    }

}

#define TIMER1_DEADTIME_6US 136
#define TIMER1_DEADTIME_5US 120
#define TIMER1_DEADTIME_4US 96
static void timer1_deadtime(int dt)
{
    /* Tdts = Tck_int */
    TIM1->CR1 &= ~TIM_CR1_CKD;
    TIM1->BDTR &= ~TIM_BDTR_DTG;

    TIM1->BDTR |= (dt & TIM_BDTR_DTG);
}


void timer1_init()
{

    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    /* set timer counter to 0 */
    TIM1->CNT = 0x0000;

    /* set timer prescaller to 1 */
    TIM1->PSC = 0x0000;

    /* enable preload for autoreload register */
    TIM1->CR1 |= TIM_CR1_ARPE;

    /* set autoreload register */
    TIM1->ARR = PWM_MAX_VALUE;

    /* enable count */
    TIM1->CR1 |= TIM_CR1_CEN;

    /* generate update event to update counter,
     * direction and autoreload register */
    TIM1->EGR |= TIM_EGR_UG;

    /* set pwm1 mode (OCxM = 110) and enable preload (OCxPE = 1)
     * for output/compare channel 1 and channel 2 */
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 |
        TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 |
        TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;

    /* set pwm1 mode (OCxM = 110) and enable preload (OCxPE = 1)
     * for output/compare channel 3 */
    TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 |
        TIM_CCMR2_OC3PE;

//    /* enable output/compare outputs (CCxE = 1)
//     * and complementary outputs (CCxNE = 1) */
//    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E |
//        TIM_CCER_CC3E | TIM_CCER_CC1NE | TIM_CCER_CC2NE |
//        TIM_CCER_CC3NE;
        
    timer1_set_output_enable(TIMER1_OUTPUT_ENABLE);
    timer1_set_output_polarity(TIMER1_POLARITY_INVERTED);

    timer1_deadtime(TIMER1_DEADTIME_5US);

    timer1_gpio_init();

    /* set initial pwm values */
    timer1_u_load(0);
    timer1_v_load(0);
    timer1_w_load(0);
    
    /* main output enable */
    // TODO: OSSI and OSSR bits
    timer1_output_enable();

    /* enable UPDATE interrupt request */
    TIM1->DIER |= TIM_DIER_UIE;
}

