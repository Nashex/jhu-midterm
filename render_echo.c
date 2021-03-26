#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "wave.h"

int main(int argc, char **argv) {

  int delay;
  float gain;

  if (argc != 5) {
    fprintf(stderr, "Usage: %s wavfilein wavfileout delay amplitude\n", argv[0]);
    exit(1);
  }

  if (sscanf(argv[3], "%d", &delay) != 1) {
    fatal_error("Invalid samples.");
  }

  if (sscanf(argv[4], "%f", &gain) != 1) {
    fatal_error("Invalid gain.");
  }

  FILE *out = fopen(argv[2], "wb");
  if (out == NULL) {
    fatal_error("Couldn't open wave file");
  }

  FILE *in = fopen(argv[1], "rb");
  if (in == NULL) {
    fatal_error("Couldn't open wave file");
  }

  unsigned num_samples;
  read_wave_header(in, &num_samples);

  write_wave_header(out, num_samples);

  int16_t *mono_buf_l = malloc(sizeof(int16_t) * num_samples);
  int16_t *mono_buf_r = malloc(sizeof(int16_t) * num_samples);
  int16_t *stereo_buf = malloc(sizeof(int16_t) * num_samples * 2);

  read_s16_buf(in, stereo_buf, num_samples * 2);

  for (unsigned int i = 0; i < num_samples * 2; i+=2) {
    mono_buf_l[i / 2] += stereo_buf[i];
  }
  for (unsigned int i = 1; i < num_samples * 2; i+=2) {
     mono_buf_r[i / 2] += stereo_buf[i];
  }

  apply_gain(mono_buf_l, num_samples, gain);
  apply_gain(mono_buf_r, num_samples, gain);

  mix_in(stereo_buf + (delay), 0, mono_buf_l, num_samples);
  mix_in(stereo_buf + (delay), 1, mono_buf_r, num_samples);

  write_s16_buf(out, stereo_buf, num_samples * 2);

  free(mono_buf_l);
  free(stereo_buf);

  fclose(in);
  fclose(out);

  return 0;
}