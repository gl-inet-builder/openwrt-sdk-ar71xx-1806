cmd_/home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/tc_ematch/.install := bash scripts/headers_install.sh /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/tc_ematch ./include/uapi/linux/tc_ematch tc_em_cmp.h tc_em_meta.h tc_em_nbyte.h tc_em_text.h; bash scripts/headers_install.sh /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/tc_ematch ./include/linux/tc_ematch ; bash scripts/headers_install.sh /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/tc_ematch ./include/generated/uapi/linux/tc_ematch ; for F in ; do echo "$(pound)include <asm-generic/$$F>" > /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/tc_ematch/$$F; done; touch /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/tc_ematch/.install
