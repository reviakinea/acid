#include "usart.h"
#include <stm32f10x.h>

static void usart_gpio_init(void)
{
    /* Enable PORTC and AFIO module clocking */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    /* remap USART3 Rx to PC11, Tx to PC10 */
    AFIO->MAPR &= ~AFIO_MAPR_USART3_REMAP;
    AFIO->MAPR |=  AFIO_MAPR_USART3_REMAP_PARTIALREMAP;

    /* Push-pull output, alternative function */
    GPIOC->CRH |=  GPIO_CRH_MODE10;
    GPIOC->CRH &= ~GPIO_CRH_CNF10;
    GPIOC->CRH |=  GPIO_CRH_CNF10_1;

    /* Pulled-up input */
    GPIOC->CRH &= ~GPIO_CRH_MODE11;
    GPIOC->CRH &= ~GPIO_CRH_CNF11;
    GPIOC->CRH |=  GPIO_CRH_CNF11_1;
    GPIOC->ODR |=  GPIO_ODR_ODR11;
}

void usart_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    USART3->CR1 |= USART_CR1_UE;

    /* 460.8 kbps */
    USART3->BRR = (3 << 4) + 4;

    USART3->CR1 |= USART_CR1_RE | USART_CR1_TE;

    uart_gpio_init();
}

void function_name()
{
    /* code */
}
