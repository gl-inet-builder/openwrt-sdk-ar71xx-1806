cmd_/home/lancer/workspace/gl-image/openwrt-18.06.git/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/scsi/.install := bash scripts/headers_install.sh /home/lancer/workspace/gl-image/openwrt-18.06.git/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/scsi ./include/uapi/scsi cxlflash_ioctl.h scsi_bsg_fc.h scsi_netlink.h scsi_netlink_fc.h; bash scripts/headers_install.sh /home/lancer/workspace/gl-image/openwrt-18.06.git/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/scsi ./include/scsi ; bash scripts/headers_install.sh /home/lancer/workspace/gl-image/openwrt-18.06.git/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/scsi ./include/generated/uapi/scsi ; for F in ; do echo "$(pound)include <asm-generic/$$F>" > /home/lancer/workspace/gl-image/openwrt-18.06.git/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/scsi/$$F; done; touch /home/lancer/workspace/gl-image/openwrt-18.06.git/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/scsi/.install
