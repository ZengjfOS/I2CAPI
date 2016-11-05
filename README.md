# I2CAPI

在Android层通过I2C总线通信来访问一些设备，这里主要是访问EEPROM而写的库。

## [JNI](jni/i2c_data.h)提供I2C Bus和[EEPROM AT24C02](http://www.atmel.com/Images/doc0180.pdf)通信函数

  * 读一串字节：`int i2c_data_read_str (int fd, int addr, int offset, unsigned char *buf, int count);`
  * 读一个字节：`int i2c_data_read_byte (int fd, int addr, int offset, unsigned char *buf);`
  * 写一串字节：`int i2c_data_write_str (int fd, int addr, int offset, unsigned char *buf, int count);`
  * 写一个字节：`int i2c_data_write_byte (int fd, int addr, int offset, unsigned char buf);`
  
## [Java API](src/com/android/i2capi/I2CAPI.java)提供如下函数

  * 打开设备节点，返回文件描述符：`native public int open(String path, int flags);`
  * 读取字符串：`native public String readStr(int fd, int addr, int offset, int count);`
  * 写入字符串：`native public int writeStr(int fd, int addr, int offset, String buf, int count);`
  * 关闭文件描述符：`native public void close(int fd);`
  
## 缺陷

该库目前仅仅能用于AT24C02芯片，如果需要用于其他方面，请注意对JNI和Java API进行修改。另外强烈建议，对这种底层硬件协议分析，最好是有逻辑分析来对硬件传输数据进行分析，可以提高效率。

## Author

[曾剑锋](http://www.cnblogs.com/zengjfgit/)
