#ifndef STM32L4xx_HAL_AL_H
#define STM32L4xx_HAL_AL_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#define HAL_MAX_DELAY      0xFFFFFFFFU

typedef enum
{
  HAL_OK       = 0x00,
  HAL_ERROR    = 0x01,
  HAL_BUSY     = 0x02,
  HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;

uint32_t HAL_GetTick(void);
void HAL_Delay(uint32_t Delay);

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);

#ifdef __cplusplus
}
#endif

#endif /* STM32L4xx_HAL_AL_H */
