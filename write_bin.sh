#!/usr/bin/zsh


disk=/home/sunqi/Desktop/operate_system_practice/hd60M.img

cd `dirname $0`
echo 当前文件位置 $(pwd)
#将mbr写入第0个分区
dd if=bin/mbr.bin of=${disk} count=1 bs=512  conv=notrunc
#将loader写入第二分区 count×bs >实际的输入大小的时候会按照实际的大小写入
dd if=bin/loader.bin  of=${disk} bs=512 count=4 seek=2 conv=notrunc
#将内核写入第九分区
dd if=bin/kernel.bin of=${disk} bs=512 count=200 seek=9 conv=notrunc

if [ -f "${disk}.lock" ];
then
    rm -f "${disk}.lock"
else
    echo "${disk}.lock 不存在"
fi

bochs -f ./bochsrc.txt