#!/bin/bash

for i in `ipcs -m | tail -n +4 | awk {'print $2'}` # 共享内存
do
	ipcrm -m $i;
	echo 删除shm = $i
done

for i in `ipcs -s | tail -n +4 | awk {'print $2'}` # 信号量
do
	echo 删除sem = $i
	ipcrm -s $i;
done

for i in `ipcs -q | tail -n +4 | awk {'print $2'}` # 消息队列
do
	echo 删除mq = $i
	ipcrm -q $i;
done