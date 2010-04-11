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
    generate(out, 20, 101, 101);
    generate(out, 20, 50, 50);
    generate(out, 20, 49, 49);
    generate(out, 20, 10000, 10000);
    generate(out, 20, 49, 49);
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
                if (random() % 2) {
                    buf[i] *= -1;
                }
            }
  
        }
    } catch (...) {
        delete [] buf;
    }
    sox_write(file, buf, samples);
//    sox_close(file);
    delete [] buf;
}