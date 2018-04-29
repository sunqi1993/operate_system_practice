#!/bin/zsh
rm -rf ../bochs_sunqi/*.lock
#将所有的*.bin *.o文件都放到bin文件夹下
nasm -I include/ -l a.txt -o bin/loader.bin loader.S
nasm -I include/ -o bin/mbr_page110.bin mbr_page110.S
nasm -f elf -o bin/print.o lib/kernel/print.S

#生成32位的程序
gcc -I lib/kernel/ -I lib/usr/ -c -g -m32 -o bin/main.o kernel/main.c  
#链接生成可重定位的文件
ld bin/main.o bin/print.o -Ttext 0xc0001500 -melf_i386 \
-e main -o bin/kernel.bin
#将mbr写入第一个分区
dd if=bin/mbr_page110.bin of=../bochs_sunqi/hd60M.img count=1 bs=512  conv=notrunc
#将loader写入第二分区 count×bs >实际的输入大小的时候会按照实际的大小写入
dd if=bin/loader.bin  of=../bochs_sunqi/hd60M.img bs=512 count=4 seek=2 conv=notrunc
#将内核写入第九分区
dd if=bin/kernel.bin of=../bochs_sunqi/hd60M.img bs=512 count=200 seek=9 conv=notrunc
#运行程序
bochs -f ../bochs_sunqi/bochsrc.txt
