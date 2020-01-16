if (GMP_FOUND)
    set(GMP_TARGET "GMP::GMP")
else ()
    if (NOT EXISTS external/gmp-6.1.2)
        execute_process(
            COMMAND python3 ../scripts/extract.py gmp-6.1.2.tar.bz2 .
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE retcode
        )
        if (NOT "${retcode}" STREQUAL "0")
            message(FATAL_ERROR "Fatal error extracting archive")
        endif ()
    endif ()

    if (WIN32)
        set(HAVE_HOST_CPU_FAMILY_power 0)
        set(HAVE_HOST_CPU_FAMILY_powerpc 0)
        set(GMP_LIMB_BITS 32)
        set(GMP_NAIL_BITS 0)
        set(DEFN_LONG_LONG_LIMB)
        set(LIBGMP_DLL 0)
        configure_file(gmp-6.1.2/gmp-h.in gmp-6.1.2/gmp.h)
        configure_file(gmp-6.1.2/mpn/x86/gmp-mparam.h gmp-6.1.2/gmp-mparam.h COPYONLY)

        add_custom_command(
            OUTPUT gmp-6.1.2/config.h
            COMMAND python ${CMAKE_SOURCE_DIR}/scripts/gmp-fix-config-h.py <gmp-6.1.2/config.in >gmp-6.1.2/config.h
            DEPENDS gmp-6.1.2/config.in
        )
        add_custom_target(config_h DEPENDS gmp-6.1.2/config.h)

        add_executable(gen-fib gmp-6.1.2/gen-fib.c)
        add_custom_command(
            OUTPUT gmp-6.1.2/fib_table.h
            COMMAND gen-fib header ${GMP_LIMB_BITS} ${GMP_NAIL_BITS} >gmp-6.1.2/fib_table.h
            DEPENDS gen-fib
        )
        add_custom_target(fib_table DEPENDS gmp-6.1.2/fib_table.h)

        add_executable(gen-fac gmp-6.1.2/gen-fac.c)
        add_custom_command(
            OUTPUT gmp-6.1.2/fac_table.h
            COMMAND gen-fac ${GMP_LIMB_BITS} ${GMP_NAIL_BITS} >gmp-6.1.2/fac_table.h
            DEPENDS gen-fac
        )
        add_custom_target(fac_table DEPENDS gmp-6.1.2/fac_table.h)

        add_executable(gen-bases gmp-6.1.2/gen-bases.c)
        add_custom_command(
            OUTPUT gmp-6.1.2/mp_bases.h
            COMMAND gen-bases header ${GMP_LIMB_BITS} ${GMP_NAIL_BITS} >gmp-6.1.2/mp_bases.h
            DEPENDS gen-bases
        )
        add_custom_command(
            OUTPUT gmp-6.1.2/mp_bases.c
            COMMAND gen-bases table ${GMP_LIMB_BITS} ${GMP_NAIL_BITS} >gmp-6.1.2/mp_bases.c
            DEPENDS gen-bases
        )
        add_custom_target(mp_bases DEPENDS gmp-6.1.2/mp_bases.h gmp-6.1.2/mp_bases.c)

        add_library(GMP
            gmp-6.1.2/assert.c
            gmp-6.1.2/errno.c
            gmp-6.1.2/memory.c
            gmp-6.1.2/mp_bases.c
            gmp-6.1.2/mp_bpl.c
            gmp-6.1.2/mp_clz_tab.c
            gmp-6.1.2/mp_dv_tab.c
            gmp-6.1.2/mp_get_fns.c
            gmp-6.1.2/mp_minv_tab.c
            gmp-6.1.2/tal-reent.c
            gmp-6.1.2/mpn/generic/add.c
            gmp-6.1.2/mpn/generic/add_1.c
            gmp-6.1.2/mpn/generic/add_n.c
            gmp-6.1.2/mpn/generic/addmul_1.c
            gmp-6.1.2/mpn/generic/bdiv_dbm1c.c
            gmp-6.1.2/mpn/generic/bdiv_q.c
            gmp-6.1.2/mpn/generic/binvert.c
            gmp-6.1.2/mpn/generic/cmp.c
            gmp-6.1.2/mpn/generic/com.c
            gmp-6.1.2/mpn/generic/dcpi1_bdiv_q.c
            gmp-6.1.2/mpn/generic/dcpi1_bdiv_qr.c
            gmp-6.1.2/mpn/generic/dcpi1_div_q.c
            gmp-6.1.2/mpn/generic/dcpi1_div_qr.c
            gmp-6.1.2/mpn/generic/dcpi1_divappr_q.c
            gmp-6.1.2/mpn/generic/divis.c
            gmp-6.1.2/mpn/generic/div_q.c
            gmp-6.1.2/mpn/generic/dive_1.c
            gmp-6.1.2/mpn/generic/divexact.c
            gmp-6.1.2/mpn/generic/divrem_1.c
            gmp-6.1.2/mpn/generic/divrem_2.c
            gmp-6.1.2/mpn/generic/gcdext.c
            gmp-6.1.2/mpn/generic/gcdext_1.c
            gmp-6.1.2/mpn/generic/gcdext_lehmer.c
            gmp-6.1.2/mpn/generic/gcd_subdiv_step.c
            gmp-6.1.2/mpn/generic/get_d.c
            gmp-6.1.2/mpn/generic/get_str.c
            gmp-6.1.2/mpn/generic/hgcd.c
            gmp-6.1.2/mpn/generic/hgcd_appr.c
            gmp-6.1.2/mpn/generic/hgcd_matrix.c
            gmp-6.1.2/mpn/generic/hgcd_reduce.c
            gmp-6.1.2/mpn/generic/hgcd_step.c
            gmp-6.1.2/mpn/generic/hgcd2.c
            gmp-6.1.2/mpn/generic/invertappr.c
            gmp-6.1.2/mpn/generic/lshift.c
            gmp-6.1.2/mpn/generic/lshiftc.c
            gmp-6.1.2/mpn/generic/matrix22_mul.c
            gmp-6.1.2/mpn/generic/matrix22_mul1_inverse_vector.c
            gmp-6.1.2/mpn/generic/mod_1.c
            gmp-6.1.2/mpn/generic/mod_1_1.c
            gmp-6.1.2/mpn/generic/mod_1_2.c
            gmp-6.1.2/mpn/generic/mod_1_4.c
            gmp-6.1.2/mpn/generic/mode1o.c
            gmp-6.1.2/mpn/generic/mul.c
            gmp-6.1.2/mpn/generic/mul_1.c
            gmp-6.1.2/mpn/generic/mul_basecase.c
            gmp-6.1.2/mpn/generic/mul_fft.c
            gmp-6.1.2/mpn/generic/mul_n.c
            gmp-6.1.2/mpn/generic/mullo_basecase.c
            gmp-6.1.2/mpn/generic/mullo_n.c
            gmp-6.1.2/mpn/generic/mulmod_bnm1.c
            gmp-6.1.2/mpn/generic/mu_bdiv_q.c
            gmp-6.1.2/mpn/generic/mu_bdiv_qr.c
            gmp-6.1.2/mpn/generic/mu_div_q.c
            gmp-6.1.2/mpn/generic/mu_div_qr.c
            gmp-6.1.2/mpn/generic/mu_divappr_q.c
            gmp-6.1.2/mpn/generic/neg.c
            gmp-6.1.2/mpn/generic/nussbaumer_mul.c
            gmp-6.1.2/mpn/generic/powlo.c
            gmp-6.1.2/mpn/generic/powm.c
            gmp-6.1.2/mpn/generic/pre_divrem_1.c
            gmp-6.1.2/mpn/generic/redc_1.c
            gmp-6.1.2/mpn/generic/redc_n.c
            gmp-6.1.2/mpn/generic/rshift.c
            gmp-6.1.2/mpn/generic/sbpi1_bdiv_q.c
            gmp-6.1.2/mpn/generic/sbpi1_bdiv_qr.c
            gmp-6.1.2/mpn/generic/sbpi1_div_q.c
            gmp-6.1.2/mpn/generic/sbpi1_div_qr.c
            gmp-6.1.2/mpn/generic/sbpi1_divappr_q.c
            gmp-6.1.2/mpn/generic/set_str.c
            gmp-6.1.2/mpn/generic/sqr.c
            gmp-6.1.2/mpn/generic/sqrlo.c
            gmp-6.1.2/mpn/generic/sqrlo_basecase.c
            gmp-6.1.2/mpn/generic/sqr_basecase.c
            gmp-6.1.2/mpn/generic/sqrmod_bnm1.c
            gmp-6.1.2/mpn/generic/sub.c
            gmp-6.1.2/mpn/generic/sub_1.c
            gmp-6.1.2/mpn/generic/sub_n.c
            gmp-6.1.2/mpn/generic/submul_1.c
            gmp-6.1.2/mpn/generic/tdiv_qr.c
            gmp-6.1.2/mpn/generic/toom2_sqr.c
            gmp-6.1.2/mpn/generic/toom22_mul.c
            gmp-6.1.2/mpn/generic/toom3_sqr.c
            gmp-6.1.2/mpn/generic/toom32_mul.c
            gmp-6.1.2/mpn/generic/toom33_mul.c
            gmp-6.1.2/mpn/generic/toom4_sqr.c
            gmp-6.1.2/mpn/generic/toom42_mul.c
            gmp-6.1.2/mpn/generic/toom43_mul.c
            gmp-6.1.2/mpn/generic/toom44_mul.c
            gmp-6.1.2/mpn/generic/toom53_mul.c
            gmp-6.1.2/mpn/generic/toom6_sqr.c
            gmp-6.1.2/mpn/generic/toom63_mul.c
            gmp-6.1.2/mpn/generic/toom6h_mul.c
            gmp-6.1.2/mpn/generic/toom8_sqr.c
            gmp-6.1.2/mpn/generic/toom8h_mul.c
            gmp-6.1.2/mpn/generic/toom_couple_handling.c
            gmp-6.1.2/mpn/generic/toom_eval_dgr3_pm1.c
            gmp-6.1.2/mpn/generic/toom_eval_dgr3_pm2.c
            gmp-6.1.2/mpn/generic/toom_eval_pm1.c
            gmp-6.1.2/mpn/generic/toom_eval_pm2.c
            gmp-6.1.2/mpn/generic/toom_eval_pm2exp.c
            gmp-6.1.2/mpn/generic/toom_eval_pm2rexp.c
            gmp-6.1.2/mpn/generic/toom_interpolate_12pts.c
            gmp-6.1.2/mpn/generic/toom_interpolate_16pts.c
            gmp-6.1.2/mpn/generic/toom_interpolate_5pts.c
            gmp-6.1.2/mpn/generic/toom_interpolate_6pts.c
            gmp-6.1.2/mpn/generic/toom_interpolate_7pts.c
            gmp-6.1.2/mpn/generic/toom_interpolate_8pts.c
            gmp-6.1.2/mpn/generic/zero_p.c
            gmp-6.1.2/mpz/abs.c
            gmp-6.1.2/mpz/add.c
            gmp-6.1.2/mpz/add_ui.c
            gmp-6.1.2/mpz/cfdiv_q_2exp.c
            gmp-6.1.2/mpz/clear.c
            gmp-6.1.2/mpz/cmp.c
            gmp-6.1.2/mpz/cmp_si.c
            gmp-6.1.2/mpz/divexact.c
            gmp-6.1.2/mpz/divis.c
            gmp-6.1.2/mpz/fdiv_r.c
            gmp-6.1.2/mpz/gcdext.c
            gmp-6.1.2/mpz/get_d.c
            gmp-6.1.2/mpz/get_si.c
            gmp-6.1.2/mpz/get_str.c
            gmp-6.1.2/mpz/get_ui.c
            gmp-6.1.2/mpz/init.c
            gmp-6.1.2/mpz/invert.c
            gmp-6.1.2/mpz/iset.c
            gmp-6.1.2/mpz/iset_si.c
            gmp-6.1.2/mpz/iset_str.c
            gmp-6.1.2/mpz/iset_ui.c
            gmp-6.1.2/mpz/mul.c
            gmp-6.1.2/mpz/mul_2exp.c
            gmp-6.1.2/mpz/n_pow_ui.c
            gmp-6.1.2/mpz/neg.c
            gmp-6.1.2/mpz/powm.c
            gmp-6.1.2/mpz/pow_ui.c
            gmp-6.1.2/mpz/realloc.c
            gmp-6.1.2/mpz/set.c
            gmp-6.1.2/mpz/set_str.c
            gmp-6.1.2/mpz/set_ui.c
            gmp-6.1.2/mpz/sub.c
            gmp-6.1.2/mpz/tdiv_q.c
            gmp-6.1.2/mpz/tdiv_r.c
            gmp-6.1.2/mpz/tdiv_r_ui.c
        )
        add_dependencies(GMP config_h fib_table fac_table mp_bases)
        target_include_directories(GMP
            PUBLIC gmp-6.1.2
        )
        set(GMP_TARGET "GMP")
    else (WIN32)
        add_custom_command(
            OUTPUT lib/libgmp.a
            COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/gmp-6.1.2 && ./configure --prefix=${CMAKE_BINARY_DIR}/external --with-pic --enable-cxx && make && make install
        )
        add_custom_target(libgmp DEPENDS lib/libgmp.a)
        add_library(GMP STATIC IMPORTED GLOBAL)
        add_dependencies(GMP libgmp)
        set_target_properties(GMP PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
        set_target_properties(GMP PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/libgmp.a")
        set(GMP_TARGET "GMP")
    endif (WIN32)
endif ()
set(GMP_TARGET "${GMP_TARGET}" PARENT_SCOPE)
