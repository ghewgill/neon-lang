if (NOT EXISTS external/IntelRDFPMathLib20U2)
    execute_process(
        COMMAND python ../scripts/extract.py IntelRDFPMathLib20U2.tar.gz .
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
    if (${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
        execute_process(
            COMMAND perl -p -i -e "s/unsigned short int fexcept_t/unsigned int fexcept_t/" external/IntelRDFPMathLib20U2/LIBRARY/src/bid_functions.h
            RESULT_VARIABLE retcode
        )
        if (NOT "${retcode}" STREQUAL "0")
            message(FATAL_ERROR "Fatal error extracting archive")
        endif ()
    endif ()
endif ()
if (WIN32)
    set(dpml_platform
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_asinh.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_asinh.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_cbrt.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_erf.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_erf.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_expm1.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_exp.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_exp.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_lgamma.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_log.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_log.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_tgamma.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_lgamma.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_pow_t_table.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_cbrt_t_table.c
        IntelRDFPMathLib20U2/LIBRARY/float128/dpml_exp.c
    )
endif (WIN32)
add_library(dpml
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_bid.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_bessel.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_cbrt.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_erf.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_exp.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_int.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_inv_hyper.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_inv_trig.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_lgamma.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_log.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_mod.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_powi.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_pow.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_sqrt.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_trig.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_ops.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_ux_ops_64.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_four_over_pi.c
    IntelRDFPMathLib20U2/LIBRARY/float128/dpml_exception.c
    IntelRDFPMathLib20U2/LIBRARY/float128/sqrt_tab_t.c
)
target_compile_options(dpml PRIVATE -DUSE_NATIVE_QUAD_TYPE=0 -Dia32)
if (WIN32)
    target_compile_options(dpml PRIVATE -Dwnt)
else (WIN32)
    target_compile_options(dpml PRIVATE -Defi2 -Dlinux -Dgcc)
endif (WIN32)
if (WIN32)
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_asinh.c         PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;ASINH")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_asinh.c         PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;ACOSH")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_cbrt.c          PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;CBRT;BUILD_FILE_NAME=dpml_cbrt_t_table.c")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_erf.c           PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;ERF;BUILD_FILE_NAME=dpml_erf_t.h")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_erf.c           PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;ERFC;BUILD_FILE_NAME=dpml_erf_t.h")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_expm1.c         PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;EXPM1;USE_CONTROL87")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_exp.c           PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;EXP10;USE_CONTROL87")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_exp.c           PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;EXP2;USE_CONTROL87")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_lgamma.c        PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;DO_LGAMMA;HACK_GAMMA_INLINE=0;BUILD_FILE_NAME=dpml_lgamma_t.h")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_log.c           PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;LOG1P")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_log.c           PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;LOG2;BASE_OF_LOG=1")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_tgamma.c        PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;TGAMMA")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_lgamma.c        PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;BUILD_FILE_NAME=dpml_lgamma_t.h")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_pow_t_table.c   PROPERTIES COMPILE_DEFINITIONS "T_FLOAT")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_cbrt_t_table.c  PROPERTIES COMPILE_DEFINITIONS "T_FLOAT")
    set_source_files_properties(IntelRDFPMathLib20U2/LIBRARY/float128/dpml_exp.c           PROPERTIES COMPILE_DEFINITIONS "T_FLOAT;SPECIAL_EXP")
endif (WIN32)

add_library(bid
    IntelRDFPMathLib20U2/LIBRARY/src/bid128.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_2_str_tables.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_acos.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_acosh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_add.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_asin.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_asinh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_atan.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_atan2.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_atanh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_cbrt.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_compare.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_cos.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_cosh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_div.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_erf.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_erfc.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_exp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_exp10.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_exp2.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_expm1.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_fdimd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_fma.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_fmod.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_frexp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_hypot.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_ldexp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_lgamma.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_llrintd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_log.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_log10.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_log1p.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_log2.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_logb.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_logbd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_lrintd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_lround.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_minmax.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_modf.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_mul.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_nearbyintd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_next.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_nexttowardd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_noncomp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_pow.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_quantexpd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_quantize.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_rem.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_round_integral.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_scalb.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_scalbl.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_sin.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_sinh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_sqrt.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_string.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_tan.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_tanh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_tgamma.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_to_int16.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_to_int32.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_to_int64.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_to_int8.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_to_uint16.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_to_uint32.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_to_uint64.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid128_to_uint8.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_acos.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_acosh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_add.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_asin.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_asinh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_atan.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_atan2.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_atanh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_cbrt.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_compare.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_cos.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_cosh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_div.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_erf.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_erfc.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_exp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_exp10.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_exp2.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_expm1.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_fdimd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_fma.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_fmod.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_frexp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_hypot.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_ldexp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_lgamma.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_llrintd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_log.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_log10.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_log1p.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_log2.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_logb.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_logbd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_lrintd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_lround.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_minmax.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_modf.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_mul.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_nearbyintd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_next.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_nexttowardd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_noncomp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_pow.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_quantexpd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_quantize.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_rem.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_round_integral.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_scalb.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_scalbl.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_sin.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_sinh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_sqrt.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_string.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_sub.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_tan.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_tanh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_tgamma.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_to_bid128.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_to_bid64.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_to_int16.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_to_int32.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_to_int64.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_to_int8.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_to_uint16.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_to_uint32.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_to_uint64.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid32_to_uint8.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_acos.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_acosh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_add.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_asin.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_asinh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_atan.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_atan2.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_atanh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_cbrt.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_compare.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_cos.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_cosh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_div.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_erf.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_erfc.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_exp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_exp10.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_exp2.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_expm1.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_fdimd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_fma.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_fmod.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_frexp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_hypot.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_ldexp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_lgamma.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_llrintd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_log.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_log10.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_log1p.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_log2.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_logb.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_logbd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_lrintd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_lround.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_minmax.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_modf.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_mul.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_nearbyintd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_next.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_nexttowardd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_noncomp.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_pow.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_quantexpd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_quantize.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_rem.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_round_integral.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_scalb.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_scalbl.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_sin.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_sinh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_sqrt.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_string.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_tan.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_tanh.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_tgamma.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_to_bid128.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_to_int16.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_to_int32.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_to_int64.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_to_int8.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_to_uint16.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_to_uint32.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_to_uint64.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid64_to_uint8.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_binarydecimal.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_convert_data.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_decimal_data.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_decimal_globals.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_dpd.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_feclearexcept.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_fegetexceptflag.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_feraiseexcept.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_fesetexceptflag.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_fetestexcept.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_flag_operations.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_from_int.c
    IntelRDFPMathLib20U2/LIBRARY/src/bid_round.c
    IntelRDFPMathLib20U2/LIBRARY/src/strtod128.c
    IntelRDFPMathLib20U2/LIBRARY/src/strtod32.c
    IntelRDFPMathLib20U2/LIBRARY/src/strtod64.c
    IntelRDFPMathLib20U2/LIBRARY/src/wcstod128.c
    IntelRDFPMathLib20U2/LIBRARY/src/wcstod32.c
    IntelRDFPMathLib20U2/LIBRARY/src/wcstod64.c
)
target_compile_options(bid
    PRIVATE -DUSE_COMPILER_F128_TYPE=0 -DUSE_COMPILER_F80_TYPE=0
    PRIVATE -DDECIMAL_GLOBAL_ROUNDING=1 -DDECIMAL_GLOBAL_EXCEPTION_FLAGS=1
)
if (WIN32)
else (WIN32)
    target_compile_options(bid PRIVATE -Defi2)
endif (WIN32)
target_include_directories(bid
    PUBLIC IntelRDFPMathLib20U2/LIBRARY/src
)
target_link_libraries(bid
    dpml
)
