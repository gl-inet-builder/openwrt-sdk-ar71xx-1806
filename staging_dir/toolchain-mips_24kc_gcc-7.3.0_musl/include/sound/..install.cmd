cmd_/home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/sound/.install := bash scripts/headers_install.sh /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/sound ./include/uapi/sound asequencer.h asoc.h asound.h asound_fm.h compress_offload.h compress_params.h emu10k1.h firewire.h hdsp.h hdspm.h sb16_csp.h sfnt_info.h snd_sst_tokens.h tlv.h usb_stream.h; bash scripts/headers_install.sh /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/sound ./include/sound ; bash scripts/headers_install.sh /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/sound ./include/generated/uapi/sound ; for F in ; do echo "$(pound)include <asm-generic/$$F>" > /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/sound/$$F; done; touch /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/sound/.install
