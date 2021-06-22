#!/bin/bash
killall -9 jwumq_monitor
killall -9 jwumq_factory
killall -9 jwumq_pool
#!/bin/bash

echo "now begin to build the program"
echo "      1. for start jwumq process"
echo "      2. for kill all jwumq process"
echo "      3. for restart all jwumq process"
echo "please select[1 - 3]:"
read SELECTED_NUM

if [ "$SELECTED_NUM" = "" ];then
SELECTED_NUM="0"
fi

case "$SELECTED_NUM" in
1)
# start jwumq process
./jwumq_monitor 
;;
2)
# kill all jwumq process
killall -9 jwumq_monitor
killall -9 jwumq_factory
killall -9 jwumq_pool
;;
3)
# restart all jwumq process
killall -9 jwumq_factory
killall -9 jwumq_pool
;;
*)
echo "******Wrong item selected, please retry!!!******"
;;
esac



