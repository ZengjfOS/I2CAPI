#include <stdio.h>
#include <string.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "i2c-dev.h"
#include "i2c.h"
#include "i2c_data.h"

#include "android/log.h"
static const char *TAG="EEPROM_aplex";
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)
/**
 * A demo for test
 */
/*
int main (int argc, char **argv) {
    int fd,ret;

    fd = open("/dev/i2c-3",O_RDWR);
    if(fd < 0)
    {
        perror("open error\n");
    }
    
    unsigned char buf[] = {'x', 'x', 'x', '0', 'y', 'm', '\0'};
    i2c_data_read_str (fd, 0x50, 0, buf, array_size(buf)-1);
    printf("i2c function test: %s\n", buf);
    //i2c_data_write_byte (fd, 0x50, 3, buf[1]);

    //unsigned char ch = 0;
    //i2c_data_read_byte(fd, 0x50, 1, &ch);
    close(fd);
    return 0;
}
*/

/*****************************************************************************
 * 函数说明：
 *     往i2c设备写数据，写一串字节
 * 参数说明：
 *     1. fd：     文件描述符
 *     2. addr：   i2c设备地址
 *     3. offset： i2c设备内寄存器偏移地址
 *     4. buf：    字节数组
 *     5. count：  字节数组长度
 * 返回值：
 *     正常返回写入的个数，如果出错，返回错误码
 ****************************************************************************/
int i2c_data_write_str (int fd, int addr, int offset, unsigned char *buf, int count) {
    int ret = 0;
    struct i2c_rdwr_ioctl_data i2c_data;

    i2c_data.nmsgs = 1;     /* 每次只写一个字节 */
    i2c_data.msgs  = (struct i2c_msg*)malloc(i2c_data.nmsgs*sizeof(struct i2c_msg));
    if(!i2c_data.msgs)
    {
        perror("i2c_data_write_str function malloc error.\n");
        return -1;
    }

    ioctl(fd, I2C_TIMEOUT, 4); /*超时时间*/
    ioctl(fd, I2C_RETRIES, 8); /*重复次数*/

    (i2c_data.msgs[0]).buf = (unsigned char*)malloc(2);

    LOGE("fd: %d, addr: 0x%x, count = %d.", fd, addr, count);
    int i = 0;
    for (i = 0; i < count; i++) {

        (i2c_data.msgs[0]).len      = 2;
        (i2c_data.msgs[0]).addr     = addr;     //i2c 设备地址
        (i2c_data.msgs[0]).flags    = 0;        //write
        (i2c_data.msgs[0]).buf[0]   = (unsigned char)offset+i;   // i2c 写入目标的地址
        (i2c_data.msgs[0]).buf[1]   = (unsigned char)buf[i];     //the data to write

        ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
        if(ret < 0) {
            //perror("i2c_data_write_str ioctl error");
            LOGE("i2c_data_write_str ioctl error i = %d.", i);
            return ret;
        }   
    }   

    free((i2c_data.msgs[0]).buf);
    free(i2c_data.msgs);

    return i;
}

/************************************************************************
 * 函数说明：
 *     往i2c设备写数据，写一个字节
 * 参数说明：
 *     1. fd：     文件描述符
 *     2. addr：   i2c设备地址
 *     3. offset： i2c设备内寄存器偏移地址
 *     4. buf：    要写入的字节
 * 返回值：
 *     正常返回0，出错，返回错误码；
 ************************************************************************/
int i2c_data_write_byte (int fd, int addr, int offset, unsigned char buf) {

    int ret = 0;
    struct i2c_rdwr_ioctl_data i2c_data;

    i2c_data.nmsgs = 1; /* 每次只写一个字节 */
    i2c_data.msgs  = (struct i2c_msg*)malloc(i2c_data.nmsgs*sizeof(struct i2c_msg));
    if(!i2c_data.msgs)
    {
        perror("i2c_data_write_byte function malloc error.\n");
        return -1;
    }

    ioctl(fd,I2C_TIMEOUT,4); /*超时时间*/
    ioctl(fd,I2C_RETRIES,8); /*重复次数*/

    (i2c_data.msgs[0]).buf      = (unsigned char*)malloc(2);

    (i2c_data.msgs[0]).len      = 2;
    (i2c_data.msgs[0]).addr     = addr;     //i2c 设备地址
    (i2c_data.msgs[0]).flags    = 0;        //write
    (i2c_data.msgs[0]).buf[0]   = (unsigned char)offset;    // i2c 写入目标的地址
    (i2c_data.msgs[0]).buf[1]   = (unsigned char)buf;       //the data to write

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if(ret < 0) {
        perror("i2c_data_write_byte ioctl error");
        return ret;
    }   

    free((i2c_data.msgs[0]).buf);
    free(i2c_data.msgs);
    return ret;
}

/*************************************************************************
 * 函数说明：
 *     从i2c设备读数据，读一个字节
 * 参数说明：
 *     1. fd：     文件描述符
 *     2. addr：   i2c设备地址
 *     3. offset： i2c设备内寄存器偏移地址
 *     4. buf：    保存读出的字节指针
 * 返回值：
 *     正常返回0，出错，返回错误码；
 ************************************************************************/
int i2c_data_read_byte (int fd, int addr, int offset, unsigned char *buf) {
    int ret;
    struct i2c_rdwr_ioctl_data i2c_data;

    i2c_data.nmsgs = 2; /*读时序为2个开始信号*/
    i2c_data.msgs  = (struct i2c_msg*)malloc(i2c_data.nmsgs*sizeof(struct i2c_msg));
    if(!i2c_data.msgs) {
        perror("i2c_data_read_byte functionmalloc error");
        exit(1);
    }

    ioctl(fd,I2C_TIMEOUT,4); /*超时时间*/
    ioctl(fd,I2C_RETRIES,8); /*重复次数*/

    (i2c_data.msgs[0]).len      = 1;        //i2c 目标数据的地址
    (i2c_data.msgs[0]).addr     = addr;     // i2c 设备地址
    (i2c_data.msgs[0]).flags    = 0;        //write
    (i2c_data.msgs[0]).buf      = (unsigned char*)malloc(1);
    (i2c_data.msgs[0]).buf[0]   = offset;   //i2c 数据地址

    (i2c_data.msgs[1]).len      = 1;        //读出的数据
    (i2c_data.msgs[1]).addr     = addr;     // i2c 设备地址
    (i2c_data.msgs[1]).flags    = I2C_M_RD; //read
    (i2c_data.msgs[1]).buf      = (unsigned char*)malloc(1);//存放返回值的地址。
    (i2c_data.msgs[1]).buf[0]   = 0;        //初始化读缓冲

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if(ret<0) {
        perror("i2c_data_read_byte ioctl error.\n");
        return ret;
    }

    //printf("buff[0] = %x\n",i2c_data.msgs[1].buf[0]);
    *buf = i2c_data.msgs[1].buf[0];

    free((i2c_data.msgs[0]).buf);
    free((i2c_data.msgs[1]).buf);
    free(i2c_data.msgs);

    return ret;
}

/**********************************************************************************
 * 函数说明：
 *     从i2c设备读数据，读一串字节
 * 参数说明：
 *     1. fd：     文件描述符
 *     2. addr：   i2c设备地址
 *     3. offset： i2c设备内寄存器偏移地址
 *     4. buf：    保存从i2c设备读出数据的字节数组指针
 *     5. count：  要读的字节个数
 * 返回值：
 *     正常返回0，如果出错，返回错误码
 **********************************************************************************/
int i2c_data_read_str (int fd, int addr, int offset, unsigned char *buf, int count) {
    int ret;
    struct i2c_rdwr_ioctl_data i2c_data;

    i2c_data.nmsgs = 2; 
    i2c_data.msgs  = (struct i2c_msg*)malloc(i2c_data.nmsgs*sizeof(struct i2c_msg));
    if(!i2c_data.msgs) {
        perror("i2c_data_read_byte functionmalloc error");
        exit(1);
    }

    ioctl(fd, I2C_TIMEOUT, 4); /*超时时间*/
    ioctl(fd, I2C_RETRIES, 8); /*重复次数*/

    (i2c_data.msgs[0]).len      = 1;        //i2c 目标数据的地址
    (i2c_data.msgs[0]).addr     = addr;     // i2c 设备地址
    (i2c_data.msgs[0]).flags    = 0;        //write
    (i2c_data.msgs[0]).buf      = (unsigned char*)malloc(1);
    (i2c_data.msgs[0]).buf[0]   = offset;   //i2c 数据地址

    (i2c_data.msgs[1]).len      = count;    //读出的数据
    (i2c_data.msgs[1]).addr     = addr;     // i2c 设备地址
    (i2c_data.msgs[1]).flags    = I2C_M_RD; //read
    (i2c_data.msgs[1]).buf      = buf;      //存放返回值的地址。

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if(ret<0) {
        perror("i2c_data_read_byte ioctl error.\n");
        return ret;
    }
    //buf[count] = 0;
    //printf("buf = %s\n", buf);

    free((i2c_data.msgs[0]).buf);
    free(i2c_data.msgs);

    return 0;
}
