#include <iostream>
#include <cstdlib>

extern "C" {
#include <sox.h>
}

void generate(sox_format_t* file, size_t samples, sox_sample_t min, sox_sample_t max);

int main (int argc, char * const argv[]) {
    sox_signalinfo_t signal;
    signal.rate = 44100;
    signal.channels = 2;
    signal.precision = 16;
    signal.length = SOX_IGNORE_LENGTH;
    signal.mult = NULL;

    sox_format_t *out = sox_open_write(argv[1], &signal, NULL, NULL, NULL, NULL);
    srandomdev();
    generate(out, 20, 0x1111, 0x1111);
    generate(out, 22050, 3276, 0x7fff);
    generate(out, 20, 0x1111, 0x1111);
    generate(out, 2000, 0, 0x0100);
    generate(out, 20, 0x2222, 0x2222);
    generate(out, 44060, 3276, 0x7fff);
    generate(out, 20, 0x2222, 0x2222);
    generate(out, 20, 0x3333, 0x3333);
    generate(out, 44060, 3276, 0x7fff);
    generate(out, 20, 0x3333, 0x3333);
    generate(out, 20, 0x4444, 0x4444);
    generate(out, 44060, 3276, 0x7fff);
    generate(out, 20, 0x4444, 0x4444);
    generate(out, 20, 0x5555, 0x5555);
    generate(out, 44060, 3276, 0x7fff);
    generate(out, 20, 0x5555, 0x5555);
    sox_close(out);
    sox_format_t *in = sox_open_read(argv[1], &signal, NULL, NULL);
    sox_sample_t t;
    sox_read(in, &t, 1);
    return 0;
}

void generate(sox_format_t* file, size_t samples, sox_sample_t min, sox_sample_t max) {
//    samples = 1;
    sox_sample_t *buf = NULL;
    try {
        buf = new sox_sample_t[samples];
        for (size_t i = 0; i < samples; ++i) {
            if (max == min) {
                buf[i] = max << 16;
            } else {
                sox_sample_t s = (random() % (max-min)) + min;
                if (s >= SOX_INT16_MAX) {
                    s = SOX_INT16_MAX;
                }
                s <<= 16;
                buf[i] = s;
            }
  
  //          buf[i] = 0x01020304;
            if (random() % 2) {
                buf[i] *= -1;
            }
        }
    } catch (...) {
        delete [] buf;
    }
    sox_write(file, buf, samples);
//    sox_close(file);
    delete [] buf;
}