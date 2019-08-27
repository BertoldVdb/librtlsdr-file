
# librtlsdr-file

**Summary**

This library replaces your librtlsdr.so.0 and instead of receiving data from the antenna it will read a file written by the rtl\_sdr program. This allows you to use any rtl-sdr receiver program with recordings, even if it cannot read from a file.

Note that it will pass the data without modification, for example the gain and sample rate can be adjusted, but the file will not be amplified or resampled. The file will be played back at the configured sample rate.

**Example usage**

```
rtl_sdr -f 100000000 -s 1600000 -g 30 -n 160000000 fmband.rtl #Record 100s of FM band signals

make                                                          #Compile library
LD_LIBRARY_PATH=`pwd` DATAFILE=fmband.rtl LOOP=1 gqrx         #Analyze the signal in GQRX
```

