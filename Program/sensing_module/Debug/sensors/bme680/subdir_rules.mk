################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
sensors/bme680/%.obj: ../sensors/bme680/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/Applications/ti/ccs900/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/Applications/ti/ccs900/ccs/ccs_base/msp430/include" --include_path="/Users/michal/OneDrive - Politechnika Warszawska/Szkoła/MGR/Magisterka/Program/sensing_module" --include_path="/Users/michal/OneDrive - Politechnika Warszawska/Szkoła/MGR/Magisterka/Program/sensing_module/driverlib/MSP430F5xx_6xx" --include_path="/Applications/ti/ccs900/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/include" --advice:power="none" --define=DEPRECATED --define=__MSP430F6726__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="sensors/bme680/$(basename $(<F)).d_raw" --obj_directory="sensors/bme680" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


