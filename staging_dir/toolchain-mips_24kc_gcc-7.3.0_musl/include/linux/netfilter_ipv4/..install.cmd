cmd_/home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/netfilter_ipv4/.install := bash scripts/headers_install.sh /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/netfilter_ipv4 ./include/uapi/linux/netfilter_ipv4 ip_tables.h ipt_CLUSTERIP.h ipt_ECN.h ipt_LOG.h ipt_REJECT.h ipt_TTL.h ipt_ah.h ipt_ecn.h ipt_ttl.h; bash scripts/headers_install.sh /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/netfilter_ipv4 ./include/linux/netfilter_ipv4 ; bash scripts/headers_install.sh /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/netfilter_ipv4 ./include/generated/uapi/linux/netfilter_ipv4 ; for F in ; do echo "$(pound)include <asm-generic/$$F>" > /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/netfilter_ipv4/$$F; done; touch /home/lancer/workspace/openwrt/build_dir/toolchain-mips_24kc_gcc-7.3.0_musl/linux-dev//include/linux/netfilter_ipv4/.install
