#!/bin/bash

echo "now begin to build the program"
echo "      1. for start amtp process"
echo "      2. for kill all amtp process"
echo "      3. for restart all amtp process"
echo "please select[1 - 3]:"
read SELECTED_NUM

if [ "$SELECTED_NUM" = "" ];then
SELECTED_NUM="0"
fi

case "$SELECTED_NUM" in
1)
# start amtp process
./amtp_monitor -c conf/config.conf
;;
2)
# kill all amtp process
killall -9 amtp_monitor
killall -9 amtp_router
killall -9 amtp_proxy
;;
3)
# restart all amtp process
killall -9 amtp_router
killall -9 amtp_proxy
;;
*)
echo "******Wrong item selected, please retry!!!******"
;;
esac


