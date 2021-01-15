#!/bin/bash

echo "now begin to build the program"
echo "      0. for all"
echo "      1. for amtp_monitor"
echo "      2. for amtp_router"
echo "      3. for amtp_proxy"
echo "      4. for libjwumq x86_64"
echo "      5. for libjwumq i386"
echo "      6. for libamtpca x86_64"
echo "      7. for libamtpca i386"
echo "      8. for libamtpsa"
echo "      9. for libamtpa tester x86_64"
echo "      10. for libamtpa tester i386"
echo "      11. for jwumq_monitor"
echo "      12. for jwumq_factory"
echo "      13. for jwumq_pool"
echo "      14. for libjwumqa"
echo "      15. for libjwumqa_tester"
echo "      16. for libamtpa_license"
echo "      17. for amtpa_lic_generator"
echo "      18. "
echo "      19. "
echo "please select[0 - 98]:"
read SELECTED_NUM

if [ "$SELECTED_NUM" = "" ];then
SELECTED_NUM="0"
fi

case "$SELECTED_NUM" in
0)
# make all
make -f ../make/amtp_monitor.mk - clean || exit 1
make -f ../make/amtp_monitor.mk - all || exit 1
make -f ../make/amtp_router.mk - clean || exit 1
make -f ../make/amtp_router.mk - all || exit 1
make -f ../make/amtp_proxy.mk - clean || exit 1
make -f ../make/amtp_proxy.mk - all || exit 1
make -f ../make/libjwumq.mk - clean || exit 1
make -f ../make/libjwumq.mk - all || exit 1
make -f ../make/libamtpsa.mk - clean || exit 1
make -f ../make/libamtpsa.mk - all || exit 1
make -f ../make/libamtpca.mk - clean || exit 1
make -f ../make/libamtpca.mk - all || exit 1
make -f ../make/jwumq_monitor.mk - clean || exit 1
make -f ../make/jwumq_monitor.mk - all || exit 1
make -f ../make/jwumq_factory.mk - clean || exit 1
make -f ../make/jwumq_factory.mk - all || exit 1
make -f ../make/jwumq_pool.mk - clean || exit 1
make -f ../make/jwumq_pool.mk - all || exit 1
make -f ../make/libamtpaccess_tester.mk - clean || exit 1
make -f ../make/libamtpaccess_tester.mk - all || exit 1
make -f ../make/libjwumqa.mk - clean || exit 1
make -f ../make/libjwumqa.mk - all || exit 1
make -f ../make/libjwumqa_tester.mk - clean || exit 1
make -f ../make/libjwumqa_tester.mk - all || exit 1

;;
1)
# make amtp_monitor
make -f ../make/amtp_monitor.mk - clean || exit 1
make -f ../make/amtp_monitor.mk - all || exit 1
;;
2)
# make amtp_router
make -f ../make/amtp_router.mk - clean || exit 1
make -f ../make/amtp_router.mk - all || exit 1
;;
3)
# make amtp_proxy
make -f ../make/amtp_proxy.mk - clean || exit 1
make -f ../make/amtp_proxy.mk - all || exit 1
;;
4)
# make libjwumq x86_64
make -f ../make/libjwumq.mk - clean || exit 1
make -f ../make/libjwumq.mk - all || exit 1
;;
5)
# make libjwumq i386
make -f ../make/libjwumq.mk - clean_i386 || exit 1
make -f ../make/libjwumq.mk - all_i386 || exit 1
;;
6)
# make libamtpca x86_64
make -f ../make/libamtpca.mk - clean || exit 1
make -f ../make/libamtpca.mk - all || exit 1
;;
7)
# make libamtpca i386
make -f ../make/libamtpca.mk - clean_i386 || exit 1
make -f ../make/libamtpca.mk - all_i386 || exit 1
;;
8)
# make libamtpsa
make -f ../make/libamtpsa.mk - clean || exit 1
make -f ../make/libamtpsa.mk - all || exit 1
;;
9)
# make libamtpa tester  x86_64
make -f ../make/libamtpaccess_tester.mk - clean || exit 1
make -f ../make/libamtpaccess_tester.mk - all || exit 1
;;
10)
# make libamtpa tester i386
make -f ../make/libamtpaccess_tester.mk - clean_i386 || exit 1
make -f ../make/libamtpaccess_tester.mk - all_i386 || exit 1
;;
11)
# make jwumq_monitor
make -f ../make/jwumq_monitor.mk - clean || exit 1
make -f ../make/jwumq_monitor.mk - all || exit 1
;;
12)
# make jwumq_factory
make -f ../make/jwumq_factory.mk - clean || exit 1
make -f ../make/jwumq_factory.mk - all || exit 1
;;
13)
# make jwumq_pool
make -f ../make/jwumq_pool.mk - clean || exit 1
make -f ../make/jwumq_pool.mk - all || exit 1
;;
14)
# make libjwumqa
make -f ../make/libjwumqa.mk - clean || exit 1
make -f ../make/libjwumqa.mk - all || exit 1
;;
15)
# make libjwumqa_tester
make -f ../make/libjwumqa_tester.mk - clean || exit 1
make -f ../make/libjwumqa_tester.mk - all || exit 1
;;
16)
# make libamtpa_license
make -f ../make/libamtpa_license.mk - clean || exit 1
make -f ../make/libamtpa_license.mk - all || exit 1
;;
17)
# make amtpa_lic_generater
make -f ../make/amtpa_lic_generator.mk - clean || exit 1
make -f ../make/amtpa_lic_generator.mk - all || exit 1
;;


*)
echo "******Wrong item selected, please retry!!!******"
;;
esac
