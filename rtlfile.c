#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <errno.h>
#include <stdio.h>
#include "rtlfile.h"

struct rtlsdr_file {
    FILE* file;
    int timer;

    volatile bool stop;
    volatile bool testmode;

    uint32_t rtl_freq;
    uint32_t tuner_freq;
    uint32_t freq;
    uint32_t rate;

    int ppm;
    int ds;
    int ot;
    int gain;
    
    uint8_t testCtr;
};

uint32_t rtlsdr_get_device_count(void){
    return 1;
}

const char* rtlsdr_get_device_name(uint32_t index){
    if(index) return NULL;

    return "RTLFILE";
}

int rtlsdr_get_device_usb_strings(uint32_t index,
					     char *manufact,
					     char *product,
					     char *serial){
    
    if(index) return -1;

    if(manufact) strcpy(manufact, "RTLFILE");
    if(product) strcpy(product, "0");
    if(serial) strcpy(serial, "000000");

    return 0;
}

int rtlsdr_get_index_by_serial(const char *serial){
    if(!serial) return -1;
    if(!strcmp(serial, "000000")){
        return 0;
    }

    return -3;
}

int rtlsdr_open(rtlsdr_dev_t **dev, uint32_t index){
    if(!dev) return -1;
    if(index) return -1;

    rtlsdr_dev_t* d = (rtlsdr_dev_t*)malloc(sizeof(rtlsdr_dev_t));
    memset(d, 0, sizeof(*d));
    const char* filename = getenv("DATAFILE");

    if(!filename){
        filename = "data.rtl";
    }

    d->file = fopen(filename, "rb");
    if(!d->file) goto error;

    d->timer = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
    if(d->timer < 0) goto error;

    d->rtl_freq = d->tuner_freq = 28800000;
    d->rate = 1600000;

    *dev = d;
    return 0;

error:
    rtlsdr_close(d);
    return -1;
}

int rtlsdr_close(rtlsdr_dev_t *d){
    if(d->file){
        fclose(d->file);
    }
    if(d){
        free(d);
    }

    return 0;
}

int rtlsdr_set_xtal_freq(rtlsdr_dev_t *dev, uint32_t rtl_freq,
				    uint32_t tuner_freq){

    dev->rtl_freq = rtl_freq;
    dev->tuner_freq = tuner_freq;
    return 0;
}

int rtlsdr_get_xtal_freq(rtlsdr_dev_t *dev, uint32_t *rtl_freq,
				    uint32_t *tuner_freq){

    if(rtl_freq) *rtl_freq = dev->rtl_freq;
    if(tuner_freq) *tuner_freq = dev->tuner_freq;

    return 0;
}

int rtlsdr_get_usb_strings(rtlsdr_dev_t *dev, char *manufact,
				      char *product, char *serial){
    return rtlsdr_get_device_usb_strings(0, manufact, product, serial);
}

int rtlsdr_write_eeprom(rtlsdr_dev_t *dev, uint8_t *data,
				  uint8_t offset, uint16_t len){
    return -3;
}

int rtlsdr_read_eeprom(rtlsdr_dev_t *dev, uint8_t *data,
				  uint8_t offset, uint16_t len){
    return -3;
}

int rtlsdr_set_center_freq(rtlsdr_dev_t *dev, uint32_t freq){
    dev->freq = freq;
    return 0;
}

uint32_t rtlsdr_get_center_freq(rtlsdr_dev_t *dev){
    return dev->freq;
}

int rtlsdr_set_freq_correction(rtlsdr_dev_t *dev, int ppm){
    dev->ppm = ppm;

    return 0;
}

int rtlsdr_get_freq_correction(rtlsdr_dev_t *dev){
    return dev->ppm;
}

enum rtlsdr_tuner rtlsdr_get_tuner_type(rtlsdr_dev_t *dev){
    return RTLSDR_TUNER_R820T;
}

int rtlsdr_get_tuner_gains(rtlsdr_dev_t *dev, int *gains){
    printf("Get tuner gains\n");

    const int gainvalues[] = {0, 9, 14, 27, 37, 77, 87, 125, 144, 157, 166, 197, 207, 229, 254, 280, 297, 328,
                              338, 364, 372, 386, 402, 421, 434, 439, 445, 480, 496};
    
    if(gains){
        memcpy(gains, gainvalues, sizeof(gainvalues));
    }

    return sizeof(gainvalues)/sizeof(int);
}

int rtlsdr_set_tuner_gain(rtlsdr_dev_t *dev, int gain){
    dev->gain = gain;
    return 0;
}

int rtlsdr_set_tuner_bandwidth(rtlsdr_dev_t *dev, uint32_t bw){
    return 0;
}

int rtlsdr_get_tuner_gain(rtlsdr_dev_t *dev){
    return dev->gain;
}

int rtlsdr_set_tuner_if_gain(rtlsdr_dev_t *dev, int stage, int gain){
    return 0;
}

int rtlsdr_set_tuner_gain_mode(rtlsdr_dev_t *dev, int manual){
    return 0;
}

int rtlsdr_set_sample_rate(rtlsdr_dev_t *dev, uint32_t rate){
    dev->rate = rate;
    return 0;
}

uint32_t rtlsdr_get_sample_rate(rtlsdr_dev_t *dev){
    return dev->rate;
}

int rtlsdr_set_testmode(rtlsdr_dev_t *dev, int on){
    dev->testmode = on;

    return 0;
}

int rtlsdr_set_agc_mode(rtlsdr_dev_t *dev, int on){
    return 0;
}

int rtlsdr_set_direct_sampling(rtlsdr_dev_t *dev, int on){
    dev->ds = on;
    return 0;
}

int rtlsdr_get_direct_sampling(rtlsdr_dev_t *dev){
    return dev->ds;
}

int rtlsdr_set_offset_tuning(rtlsdr_dev_t *dev, int on){
    dev->ot = on;
    return 0;
}

int rtlsdr_get_offset_tuning(rtlsdr_dev_t *dev){
    return dev->ot;
}

int rtlsdr_reset_buffer(rtlsdr_dev_t *dev){
    return 0;
}

int rtlsdr_read_sync(rtlsdr_dev_t *dev, void *buf, int len, int *n_read){
    if(!dev->testmode){
        size_t n = fread(buf, (size_t)len, 1, dev->file);
        if(n == 0){
            return -1;
        }
    }else{
        for(int j=0; j<len; j++){
            ((uint8_t*)buf)[j] = dev->testCtr++;
        }
    }
    
    if(n_read) *n_read = len;

    return 0;
}

int rtlsdr_wait_async(rtlsdr_dev_t *dev, rtlsdr_read_async_cb_t cb, void *ctx){
    return rtlsdr_read_async(dev, cb, ctx, 0, 0);
}

int rtlsdr_read_async(rtlsdr_dev_t *dev,
				 rtlsdr_read_async_cb_t cb,
				 void *ctx,
				 uint32_t buf_num,
                 uint32_t buf_len){
    int retVal = 0;

    if(buf_len == 0){
        buf_len = 16 * 32 * 512;
    }
    if(buf_len % 2){
        return -1;
    }

    unsigned char* buf = (unsigned char*)malloc(buf_len);

    float interval = 1e9f/(float)dev->rate;

    struct itimerspec sample;
    sample.it_value.tv_sec = 0; 
    sample.it_value.tv_nsec = (long)((float)buf_len * interval / 2.0f);
    sample.it_interval = sample.it_value;

    timerfd_settime(dev->timer, 0, &sample, NULL);

    size_t bytesRead = 0;

    while(!dev->stop){
        uint64_t p = 1;
        ssize_t n = read(dev->timer, &p, sizeof(p));

        if(n <= 0){
            if(errno == EINTR){
                continue;
            }

            retVal = -1;
            break;
        }

        for(uint64_t i=0; i<p; i++){
            if(rtlsdr_read_sync(dev, buf, (int)buf_len, NULL)){
                retVal = -1;
                dev->stop = true;
                break;
            }

            bytesRead += (size_t)buf_len;
            cb(buf, buf_len, ctx);
        }

    }
    dev->stop = false;
    
    sample.it_value.tv_nsec = 0;
    timerfd_settime(dev->timer, 0, &sample, NULL);

    free(buf);

    return retVal;
}

int rtlsdr_cancel_async(rtlsdr_dev_t *dev){
    dev->stop = true;
    return 0;
}

/*!
 * Enable or disable the bias tee on GPIO PIN 0.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param on  1 for Bias T on. 0 for Bias T off.
 * \return -1 if device is not initialized. 0 otherwise.
 */
int rtlsdr_set_bias_tee(rtlsdr_dev_t *dev, int on){
    return 0;
}
