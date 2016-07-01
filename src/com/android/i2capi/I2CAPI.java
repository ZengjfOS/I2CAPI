package com.android.i2capi;

public class I2CAPI {
	int fd = -1;
	native public int open(String path, int flags);
	native public String readStr(int fd, int addr, int offset, int count);
	native public int writeStr(int fd, int addr, int offset, String buf, int count);
	native public void close(int fd);

	static {
		System.loadLibrary("I2CAPI");
	}
}
