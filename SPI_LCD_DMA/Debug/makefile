################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include L4_IOT_Sensors/Drivers/BSP/Components/hts221/subdir.mk
-include L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include BSP/st7735/subdir.mk
-include 3rdParty/FreeRTOS/Source/portable/MemMang/subdir.mk
-include 3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include 3rdParty/FreeRTOS/Source/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Module7_SPI_LCD_DMA
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Module7_SPI_LCD_DMA.elf \

MAP_FILES += \
Module7_SPI_LCD_DMA.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Module7_SPI_LCD_DMA.list \


# All Target
all: main-build

# Main-build Target
main-build: Module7_SPI_LCD_DMA.elf secondary-outputs

# Tool invocations
Module7_SPI_LCD_DMA.elf Module7_SPI_LCD_DMA.map: $(OBJS) $(USER_OBJS) C:\interfacing\Module7_SPI_LCD_DMA\STM32L4S5VITX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Module7_SPI_LCD_DMA.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\interfacing\Module7_SPI_LCD_DMA\STM32L4S5VITX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Module7_SPI_LCD_DMA.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Module7_SPI_LCD_DMA.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Module7_SPI_LCD_DMA.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Module7_SPI_LCD_DMA.elf Module7_SPI_LCD_DMA.list Module7_SPI_LCD_DMA.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
