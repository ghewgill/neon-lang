if (OPENSSL_FOUND)
    set(OPENSSL_TARGET "OpenSSL::SSL;OpenSSL::Crypto")
else ()
    if (NOT EXISTS external/libressl-3.2.2)
        execute_process(
            COMMAND python3 ../scripts/extract.py libressl-3.2.2.tar.gz .
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE retcode
        )
        if (NOT "${retcode}" STREQUAL "0")
            message(FATAL_ERROR "Fatal error extracting archive")
        endif ()
    endif ()

    if (WIN32)
        add_library(RESSL
            libressl-3.2.2/crypto/aes/aes_cbc.c
            libressl-3.2.2/crypto/aes/aes_core.c
            libressl-3.2.2/crypto/asn1/a_bitstr.c
            libressl-3.2.2/crypto/asn1/a_bool.c
            libressl-3.2.2/crypto/asn1/a_d2i_fp.c
            libressl-3.2.2/crypto/asn1/a_digest.c
            libressl-3.2.2/crypto/asn1/a_dup.c
            libressl-3.2.2/crypto/asn1/a_enum.c
            libressl-3.2.2/crypto/asn1/a_i2d_fp.c
            libressl-3.2.2/crypto/asn1/a_int.c
            libressl-3.2.2/crypto/asn1/a_mbstr.c
            libressl-3.2.2/crypto/asn1/a_object.c
            libressl-3.2.2/crypto/asn1/a_octet.c
            libressl-3.2.2/crypto/asn1/a_print.c
            libressl-3.2.2/crypto/asn1/a_sign.c
            libressl-3.2.2/crypto/asn1/a_strex.c
            libressl-3.2.2/crypto/asn1/a_strnid.c
            libressl-3.2.2/crypto/asn1/a_time.c
            libressl-3.2.2/crypto/asn1/a_time_tm.c
            libressl-3.2.2/crypto/asn1/a_type.c
            libressl-3.2.2/crypto/asn1/a_utf8.c
            libressl-3.2.2/crypto/asn1/a_verify.c
            libressl-3.2.2/crypto/asn1/ameth_lib.c
            libressl-3.2.2/crypto/asn1/asn_pack.c
            libressl-3.2.2/crypto/asn1/asn1_gen.c
            libressl-3.2.2/crypto/asn1/asn1_lib.c
            libressl-3.2.2/crypto/asn1/asn1_par.c
            libressl-3.2.2/crypto/asn1/d2i_pr.c
            libressl-3.2.2/crypto/asn1/evp_asn1.c
            libressl-3.2.2/crypto/asn1/f_int.c
            libressl-3.2.2/crypto/asn1/f_string.c
            libressl-3.2.2/crypto/asn1/i2d_pr.c
            libressl-3.2.2/crypto/asn1/p5_pbe.c
            libressl-3.2.2/crypto/asn1/p5_pbev2.c
            libressl-3.2.2/crypto/asn1/p8_pkey.c
            libressl-3.2.2/crypto/asn1/t_pkey.c
            libressl-3.2.2/crypto/asn1/t_x509.c
            libressl-3.2.2/crypto/asn1/t_x509a.c
            libressl-3.2.2/crypto/asn1/tasn_dec.c
            libressl-3.2.2/crypto/asn1/tasn_enc.c
            libressl-3.2.2/crypto/asn1/tasn_fre.c
            libressl-3.2.2/crypto/asn1/tasn_new.c
            libressl-3.2.2/crypto/asn1/tasn_prn.c
            libressl-3.2.2/crypto/asn1/tasn_typ.c
            libressl-3.2.2/crypto/asn1/tasn_utl.c
            libressl-3.2.2/crypto/asn1/x_algor.c
            libressl-3.2.2/crypto/asn1/x_attrib.c
            libressl-3.2.2/crypto/asn1/x_bignum.c
            libressl-3.2.2/crypto/asn1/x_crl.c
            libressl-3.2.2/crypto/asn1/x_exten.c
            libressl-3.2.2/crypto/asn1/x_long.c
            libressl-3.2.2/crypto/asn1/x_name.c
            libressl-3.2.2/crypto/asn1/x_pubkey.c
            libressl-3.2.2/crypto/asn1/x_req.c
            libressl-3.2.2/crypto/asn1/x_sig.c
            libressl-3.2.2/crypto/asn1/x_spki.c
            libressl-3.2.2/crypto/asn1/x_val.c
            libressl-3.2.2/crypto/asn1/x_x509.c
            libressl-3.2.2/crypto/asn1/x_x509a.c
            libressl-3.2.2/crypto/bio/b_dump.c
            libressl-3.2.2/crypto/bio/b_print.c
            libressl-3.2.2/crypto/bio/bio_lib.c
            libressl-3.2.2/crypto/bio/bss_file.c
            libressl-3.2.2/crypto/bio/bss_mem.c
            libressl-3.2.2/crypto/bio/bss_null.c
            libressl-3.2.2/crypto/bn/bn_add.c
            libressl-3.2.2/crypto/bn/bn_asm.c
            libressl-3.2.2/crypto/bn/bn_blind.c
            libressl-3.2.2/crypto/bn/bn_ctx.c
            libressl-3.2.2/crypto/bn/bn_div.c
            libressl-3.2.2/crypto/bn/bn_exp.c
            libressl-3.2.2/crypto/bn/bn_exp2.c
            libressl-3.2.2/crypto/bn/bn_gcd.c
            libressl-3.2.2/crypto/bn/bn_gf2m.c
            libressl-3.2.2/crypto/bn/bn_kron.c
            libressl-3.2.2/crypto/bn/bn_lib.c
            libressl-3.2.2/crypto/bn/bn_mod.c
            libressl-3.2.2/crypto/bn/bn_mont.c
            libressl-3.2.2/crypto/bn/bn_mul.c
            libressl-3.2.2/crypto/bn/bn_nist.c
            libressl-3.2.2/crypto/bn/bn_prime.c
            libressl-3.2.2/crypto/bn/bn_print.c
            libressl-3.2.2/crypto/bn/bn_rand.c
            libressl-3.2.2/crypto/bn/bn_recp.c
            libressl-3.2.2/crypto/bn/bn_shift.c
            libressl-3.2.2/crypto/bn/bn_sqr.c
            libressl-3.2.2/crypto/bn/bn_sqrt.c
            libressl-3.2.2/crypto/bn/bn_word.c
            libressl-3.2.2/crypto/buffer/buffer.c
            libressl-3.2.2/crypto/cmac/cm_ameth.c
            libressl-3.2.2/crypto/cmac/cm_pmeth.c
            libressl-3.2.2/crypto/cmac/cmac.c
            libressl-3.2.2/crypto/cms/cms_asn1.c
            libressl-3.2.2/crypto/cms/cms_att.c
            libressl-3.2.2/crypto/cms/cms_dd.c
            libressl-3.2.2/crypto/cms/cms_env.c
            libressl-3.2.2/crypto/cms/cms_kari.c
            libressl-3.2.2/crypto/cms/cms_lib.c
            libressl-3.2.2/crypto/cms/cms_sd.c
            libressl-3.2.2/crypto/compat/arc4random.c
            libressl-3.2.2/crypto/compat/bsd-asprintf.c
            libressl-3.2.2/crypto/compat/explicit_bzero_win.c
            libressl-3.2.2/crypto/compat/getentropy_win.c
            libressl-3.2.2/crypto/compat/posix_win.c
            libressl-3.2.2/crypto/compat/reallocarray.c
            libressl-3.2.2/crypto/compat/strcasecmp.c
            libressl-3.2.2/crypto/compat/strlcat.c
            libressl-3.2.2/crypto/compat/strlcpy.c
            libressl-3.2.2/crypto/compat/timingsafe_bcmp.c
            libressl-3.2.2/crypto/compat/timingsafe_memcmp.c
            libressl-3.2.2/crypto/conf/conf_api.c
            libressl-3.2.2/crypto/conf/conf_def.c
            libressl-3.2.2/crypto/conf/conf_lib.c
            libressl-3.2.2/crypto/conf/conf_mod.c
            libressl-3.2.2/crypto/cryptlib.c
            libressl-3.2.2/crypto/dh/dh_ameth.c
            libressl-3.2.2/crypto/dh/dh_asn1.c
            libressl-3.2.2/crypto/dh/dh_check.c
            libressl-3.2.2/crypto/dh/dh_gen.c
            libressl-3.2.2/crypto/dh/dh_key.c
            libressl-3.2.2/crypto/dh/dh_lib.c
            libressl-3.2.2/crypto/dh/dh_pmeth.c
            libressl-3.2.2/crypto/dsa/dsa_ameth.c
            libressl-3.2.2/crypto/dsa/dsa_asn1.c
            libressl-3.2.2/crypto/dsa/dsa_gen.c
            libressl-3.2.2/crypto/dsa/dsa_key.c
            libressl-3.2.2/crypto/dsa/dsa_lib.c
            libressl-3.2.2/crypto/dsa/dsa_ossl.c
            libressl-3.2.2/crypto/dsa/dsa_pmeth.c
            libressl-3.2.2/crypto/dsa/dsa_sign.c
            libressl-3.2.2/crypto/dsa/dsa_vrf.c
            libressl-3.2.2/crypto/dso/dso_lib.c
            libressl-3.2.2/crypto/dso/dso_null.c
            libressl-3.2.2/crypto/dso/dso_openssl.c
            libressl-3.2.2/crypto/ec/ec_ameth.c
            libressl-3.2.2/crypto/ec/ec_asn1.c
            libressl-3.2.2/crypto/ec/ec_curve.c
            libressl-3.2.2/crypto/ec/ec_cvt.c
            libressl-3.2.2/crypto/ec/ec_key.c
            libressl-3.2.2/crypto/ec/ec_lib.c
            libressl-3.2.2/crypto/ec/ec_mult.c
            libressl-3.2.2/crypto/ec/ec_oct.c
            libressl-3.2.2/crypto/ec/ec_pmeth.c
            libressl-3.2.2/crypto/ec/ec_print.c
            libressl-3.2.2/crypto/ec/ec2_mult.c
            libressl-3.2.2/crypto/ec/ec2_oct.c
            libressl-3.2.2/crypto/ec/ec2_smpl.c
            libressl-3.2.2/crypto/ec/eck_prn.c
            libressl-3.2.2/crypto/ec/ecp_mont.c
            libressl-3.2.2/crypto/ec/ecp_nist.c
            libressl-3.2.2/crypto/ec/ecp_oct.c
            libressl-3.2.2/crypto/ec/ecp_smpl.c
            libressl-3.2.2/crypto/ecdh/ech_key.c
            libressl-3.2.2/crypto/ecdh/ech_lib.c
            libressl-3.2.2/crypto/ecdsa/ecs_asn1.c
            libressl-3.2.2/crypto/ecdsa/ecs_lib.c
            libressl-3.2.2/crypto/ecdsa/ecs_ossl.c
            libressl-3.2.2/crypto/ecdsa/ecs_sign.c
            libressl-3.2.2/crypto/ecdsa/ecs_vrf.c
            libressl-3.2.2/crypto/engine/eng_init.c
            libressl-3.2.2/crypto/engine/eng_lib.c
            libressl-3.2.2/crypto/engine/eng_list.c
            libressl-3.2.2/crypto/engine/eng_table.c
            libressl-3.2.2/crypto/engine/tb_asnmth.c
            libressl-3.2.2/crypto/engine/tb_cipher.c
            libressl-3.2.2/crypto/engine/tb_dh.c
            libressl-3.2.2/crypto/engine/tb_digest.c
            libressl-3.2.2/crypto/engine/tb_dsa.c
            libressl-3.2.2/crypto/engine/tb_ecdh.c
            libressl-3.2.2/crypto/engine/tb_ecdsa.c
            libressl-3.2.2/crypto/engine/tb_pkmeth.c
            libressl-3.2.2/crypto/engine/tb_rsa.c
            libressl-3.2.2/crypto/err/err.c
            libressl-3.2.2/crypto/err/err_prn.c
            libressl-3.2.2/crypto/evp/bio_enc.c
            libressl-3.2.2/crypto/evp/bio_md.c
            libressl-3.2.2/crypto/evp/digest.c
            libressl-3.2.2/crypto/evp/e_aes.c
            libressl-3.2.2/crypto/evp/evp_enc.c
            libressl-3.2.2/crypto/evp/evp_lib.c
            libressl-3.2.2/crypto/evp/evp_pbe.c
            libressl-3.2.2/crypto/evp/evp_pkey.c
            libressl-3.2.2/crypto/evp/m_md5.c
            libressl-3.2.2/crypto/evp/m_sha1.c
            libressl-3.2.2/crypto/evp/m_sigver.c
            libressl-3.2.2/crypto/evp/names.c
            libressl-3.2.2/crypto/evp/p_lib.c
            libressl-3.2.2/crypto/evp/p_sign.c
            libressl-3.2.2/crypto/evp/p_verify.c
            libressl-3.2.2/crypto/evp/p5_crpt.c
            libressl-3.2.2/crypto/evp/p5_crpt2.c
            libressl-3.2.2/crypto/evp/pmeth_fn.c
            libressl-3.2.2/crypto/evp/pmeth_gn.c
            libressl-3.2.2/crypto/evp/pmeth_lib.c
            libressl-3.2.2/crypto/ex_data.c
            libressl-3.2.2/crypto/gost/gost_asn1.c
            libressl-3.2.2/crypto/gost/gost2814789.c
            libressl-3.2.2/crypto/gost/gost89_keywrap.c
            libressl-3.2.2/crypto/gost/gost89_params.c
            libressl-3.2.2/crypto/gost/gost89imit_ameth.c
            libressl-3.2.2/crypto/gost/gost89imit_pmeth.c
            libressl-3.2.2/crypto/gost/gostr341001.c
            libressl-3.2.2/crypto/gost/gostr341001_ameth.c
            libressl-3.2.2/crypto/gost/gostr341001_key.c
            libressl-3.2.2/crypto/gost/gostr341001_params.c
            libressl-3.2.2/crypto/gost/gostr341001_pmeth.c
            libressl-3.2.2/crypto/gost/gostr341194.c
            libressl-3.2.2/crypto/gost/streebog.c
            libressl-3.2.2/crypto/hmac/hm_ameth.c
            libressl-3.2.2/crypto/hmac/hm_pmeth.c
            libressl-3.2.2/crypto/hmac/hmac.c
            libressl-3.2.2/crypto/lhash/lhash.c
            libressl-3.2.2/crypto/malloc-wrapper.c
            libressl-3.2.2/crypto/md5/md5_dgst.c
            libressl-3.2.2/crypto/mem_clr.c
            libressl-3.2.2/crypto/mem_dbg.c
            libressl-3.2.2/crypto/modes/cbc128.c
            libressl-3.2.2/crypto/modes/ccm128.c
            libressl-3.2.2/crypto/modes/cfb128.c
            libressl-3.2.2/crypto/modes/ctr128.c
            libressl-3.2.2/crypto/modes/gcm128.c
            libressl-3.2.2/crypto/modes/ofb128.c
            libressl-3.2.2/crypto/modes/xts128.c
            libressl-3.2.2/crypto/o_init.c
            libressl-3.2.2/crypto/o_time.c
            libressl-3.2.2/crypto/objects/o_names.c
            libressl-3.2.2/crypto/objects/obj_dat.c
            libressl-3.2.2/crypto/objects/obj_lib.c
            libressl-3.2.2/crypto/objects/obj_xref.c
            libressl-3.2.2/crypto/ocsp/ocsp_asn.c
            libressl-3.2.2/crypto/pkcs12/p12_add.c
            libressl-3.2.2/crypto/pkcs12/p12_asn.c
            libressl-3.2.2/crypto/pkcs12/p12_crpt.c
            libressl-3.2.2/crypto/pkcs12/p12_decr.c
            libressl-3.2.2/crypto/pkcs12/p12_key.c
            libressl-3.2.2/crypto/pkcs12/p12_p8d.c
            libressl-3.2.2/crypto/pkcs12/p12_p8e.c
            libressl-3.2.2/crypto/pkcs12/p12_utl.c
            libressl-3.2.2/crypto/pkcs7/pk7_asn1.c
            libressl-3.2.2/crypto/pkcs7/pk7_attr.c
            libressl-3.2.2/crypto/pkcs7/pk7_doit.c
            libressl-3.2.2/crypto/pkcs7/pk7_lib.c
            libressl-3.2.2/crypto/rand/rand_lib.c
            libressl-3.2.2/crypto/rsa/rsa_ameth.c
            libressl-3.2.2/crypto/rsa/rsa_asn1.c
            libressl-3.2.2/crypto/rsa/rsa_crpt.c
            libressl-3.2.2/crypto/rsa/rsa_eay.c
            libressl-3.2.2/crypto/rsa/rsa_gen.c
            libressl-3.2.2/crypto/rsa/rsa_lib.c
            libressl-3.2.2/crypto/rsa/rsa_none.c
            libressl-3.2.2/crypto/rsa/rsa_oaep.c
            libressl-3.2.2/crypto/rsa/rsa_pk1.c
            libressl-3.2.2/crypto/rsa/rsa_pmeth.c
            libressl-3.2.2/crypto/rsa/rsa_pss.c
            libressl-3.2.2/crypto/rsa/rsa_sign.c
            libressl-3.2.2/crypto/rsa/rsa_x931.c
            libressl-3.2.2/crypto/sha/sha1_one.c
            libressl-3.2.2/crypto/sha/sha1dgst.c
            libressl-3.2.2/crypto/sha/sha256.c
            libressl-3.2.2/crypto/sha/sha512.c
            libressl-3.2.2/crypto/stack/stack.c
            libressl-3.2.2/crypto/x509/x_all.c
            libressl-3.2.2/crypto/x509/x509_akey.c
            libressl-3.2.2/crypto/x509/x509_alt.c
            libressl-3.2.2/crypto/x509/x509_att.c
            libressl-3.2.2/crypto/x509/x509_bcons.c
            libressl-3.2.2/crypto/x509/x509_bitst.c
            libressl-3.2.2/crypto/x509/x509_cmp.c
            libressl-3.2.2/crypto/x509/x509_conf.c
            libressl-3.2.2/crypto/x509/x509_cpols.c
            libressl-3.2.2/crypto/x509/x509_crld.c
            libressl-3.2.2/crypto/x509/x509_def.c
            libressl-3.2.2/crypto/x509/x509_enum.c
            libressl-3.2.2/crypto/x509/x509_ext.c
            libressl-3.2.2/crypto/x509/x509_extku.c
            libressl-3.2.2/crypto/x509/x509_genn.c
            libressl-3.2.2/crypto/x509/x509_ia5.c
            libressl-3.2.2/crypto/x509/x509_info.c
            libressl-3.2.2/crypto/x509/x509_int.c
            libressl-3.2.2/crypto/x509/x509_lib.c
            libressl-3.2.2/crypto/x509/x509_lu.c
            libressl-3.2.2/crypto/x509/x509_ncons.c
            libressl-3.2.2/crypto/x509/x509_obj.c
            libressl-3.2.2/crypto/x509/x509_ocsp.c
            libressl-3.2.2/crypto/x509/x509_pci.c
            libressl-3.2.2/crypto/x509/x509_pcia.c
            libressl-3.2.2/crypto/x509/x509_pcons.c
            libressl-3.2.2/crypto/x509/x509_pku.c
            libressl-3.2.2/crypto/x509/x509_pmaps.c
            libressl-3.2.2/crypto/x509/x509_prn.c
            libressl-3.2.2/crypto/x509/x509_req.c
            libressl-3.2.2/crypto/x509/x509_skey.c
            libressl-3.2.2/crypto/x509/x509_sxnet.c
            libressl-3.2.2/crypto/x509/x509_trs.c
            libressl-3.2.2/crypto/x509/x509_v3.c
            libressl-3.2.2/crypto/x509/x509_vfy.c
            libressl-3.2.2/crypto/x509/x509_vpm.c
            libressl-3.2.2/crypto/x509/x509_utl.c
            libressl-3.2.2/crypto/x509/x509name.c
            libressl-3.2.2/crypto/x509/x509rset.c
        )
        target_compile_definitions(RESSL PRIVATE -DLIBRESSL_INTERNAL)
        target_compile_definitions(RESSL PRIVATE -DOPENSSL_NO_HW_PADLOCK)
        target_compile_definitions(RESSL PRIVATE -D__BEGIN_HIDDEN_DECLS=)
        target_compile_definitions(RESSL PRIVATE -D__END_HIDDEN_DECLS=)
        target_include_directories(RESSL
            PUBLIC libressl-3.2.2/include
            PRIVATE libressl-3.2.2/include/compat
            PRIVATE libressl-3.2.2/crypto
            PRIVATE libressl-3.2.2/crypto/asn1
            PRIVATE libressl-3.2.2/crypto/bn
            PRIVATE libressl-3.2.2/crypto/ec
            PRIVATE libressl-3.2.2/crypto/ecdh
            PRIVATE libressl-3.2.2/crypto/evp
            PRIVATE libressl-3.2.2/crypto/modes
        )
        set(OPENSSL_TARGET "RESSL")
    else (WIN32)
        add_custom_command(
            OUTPUT lib/libssl.a lib/libcrypto.a
            COMMAND cd libressl-3.2.2 && ./configure --prefix=`pwd`/.. --with-pic && make && make install
        )
        add_custom_target(libssl DEPENDS lib/libssl.a)
        add_library(RESSL::SSL STATIC IMPORTED GLOBAL)
        add_dependencies(RESSL::SSL libssl)
        set_target_properties(RESSL::SSL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
        set_target_properties(RESSL::SSL PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/libssl.a")
        add_custom_target(libcrypto DEPENDS lib/libcrypto.a)
        add_library(RESSL::Crypto STATIC IMPORTED GLOBAL)
        add_dependencies(RESSL::Crypto libcrypto)
        set_target_properties(RESSL::Crypto PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
        set_target_properties(RESSL::Crypto PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/libcrypto.a")
        set(OPENSSL_TARGET "RESSL::SSL;RESSL::Crypto")
    endif (WIN32)
endif ()
set(OPENSSL_TARGET "${OPENSSL_TARGET}" PARENT_SCOPE)
