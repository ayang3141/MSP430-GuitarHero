################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
LcdDriver/sharp96/%.obj: ../LcdDriver/sharp96/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/CCS/ccs/tools/compiler/ti-cgt-msp430_20.2.0.LTS/bin/cl430" -vmspx --data_model=restricted -Ooff --use_hw_mpy=F5 --include_path="C:/CCS/ccs/ccs_base/msp430/include" --include_path="C:/Users/Adam Yang/Desktop/ccs_workspace/ece2049e20_lab2_template" --include_path="C:/Users/Adam Yang/Desktop/ccs_workspace/ece2049e20_lab2_template/grlib" --include_path="C:/CCS/ccs/tools/compiler/ti-cgt-msp430_20.2.0.LTS/include" --advice:power_severity=suppress --define=__MSP430F5529__ -g --gcc --printf_support=minimal --diag_warning=225 --display_error_number --abi=eabi --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="LcdDriver/sharp96/$(basename $(<F)).d_raw" --obj_directory="LcdDriver/sharp96" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


