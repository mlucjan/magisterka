################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
mcu_config/%.obj: ../mcu_config/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/Applications/ti/ccs900/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx -O0 --use_hw_mpy=F5 --include_path="/Applications/ti/ccs900/ccs/ccs_base/msp430/include" --include_path="/Users/michal/Library/Mobile Documents/com~apple~CloudDocs/Szkoła/MGR/Magisterka/Program/Radio/radio_module" --include_path="/Users/michal/Library/Mobile Documents/com~apple~CloudDocs/Szkoła/MGR/Magisterka/Program/Radio/radio_module/driverlib/MSP430FR2xx_4xx" --include_path="/Applications/ti/ccs900/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --advice:power="none" --advice:hw_config=all --define=DEPRECATED --define=__MSP430FR2155__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="mcu_config/$(basename $(<F)).d_raw" --obj_directory="mcu_config" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


