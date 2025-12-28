################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/mq4/mq4.c 

OBJS += \
./Drivers/BSP/mq4/mq4.o 

C_DEPS += \
./Drivers/BSP/mq4/mq4.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/mq4/%.o Drivers/BSP/mq4/%.su Drivers/BSP/mq4/%.cyclo: ../Drivers/BSP/mq4/%.c Drivers/BSP/mq4/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"G:/github/new/NewConnector/Drivers/BSP" -I"G:/github/new/NewConnector/Drivers/SYSTEM" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-mq4

clean-Drivers-2f-BSP-2f-mq4:
	-$(RM) ./Drivers/BSP/mq4/mq4.cyclo ./Drivers/BSP/mq4/mq4.d ./Drivers/BSP/mq4/mq4.o ./Drivers/BSP/mq4/mq4.su

.PHONY: clean-Drivers-2f-BSP-2f-mq4

