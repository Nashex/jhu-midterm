#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "wave.h"

int main(int argc, char **argv) {

  float freq, gain;
  int waveType;
  int numSamples; 

  if (argc != 6) {
    fprintf(stderr, "Usage: %s waveform frequency amplitude numsamples wavfileout\n", argv[0]);
    exit(1);
  }

  if (sscanf(argv[1], "%d", &waveType) != 1) {
    fatal_error("Invalid wave type.");
  }

  if (sscanf(argv[2], "%f", &freq) != 1) {
    fatal_error("Invalid frequency.");
  }

  if (sscanf(argv[3], "%f", &gain) != 1) {
    fatal_error("Invalid gain.");
  }

  if (sscanf(argv[4], "%d", &numSamples) != 1) {
    fatal_error("Invalid samples.");
  }

  FILE *out = fopen(argv[5], "wb");
  if (out == NULL) {
    fatal_error("Couldn't open wave file");
  }

  write_wave_header(out, numSamples);

  int16_t *mono_buf = malloc(sizeof(int16_t) * numSamples);
  int16_t *stereo_buf = malloc(sizeof(int16_t) * numSamples * 2);

  for (int i = 0; i < numSamples * 2; i++) stereo_buf[i] = 0;

  switch(waveType) {
    case 0:
      generate_sine_wave(mono_buf, numSamples, freq);
      break;
    case 1:
      generate_square_wave(mono_buf, numSamples, freq);
      break;
    case 2:
      generate_saw_wave(mono_buf, numSamples, freq); 
      break; 
    default:
      fatal_error("Invalid wave type.");
      break;  
  }


  apply_gain(mono_buf, numSamples, gain);
  mix_in(stereo_buf, 0, mono_buf, numSamples);
  mix_in(stereo_buf, 1, mono_buf, numSamples);

  write_s16_buf(out, stereo_buf, numSamples * 2);

  fclose(out);

  return 0;
}