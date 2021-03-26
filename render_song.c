#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "io.h"
#include "wave.h"

typedef struct Instrument {
  int wave;
  float pan;
  bool asdr;
  float gain;
} Instrument;

int main(int argc, char **argv) {

  float freq, gain;
  int waveType;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s example.txt example.wav\n", argv[0]);
    exit(1);
  }

  FILE *in = fopen(argv[1], "rb");
  if (in == NULL) {
    fatal_error("Couldn't open text file");
  }

  //Instrument instruments[16];
  //for (int i = 0; i < 16; i++) instruments[i] = (Instrument) {.wave = 0, .pan = 0.0, .asdr = false, .gain = .2};

  uint16_t numSamples;
  read_u16(in, &numSamples);

  printf("%d", numSamples);

  char c = 'a';
  while (fread(&c, sizeof(char), 1, in) == 1) {
    printf("%s", &c);
  }

  //write_wave_header(in, *numSamples);

  //int16_t *mono_buf = malloc(sizeof(int16_t) * numSamples);
  //int16_t *stereo_buf = malloc(sizeof(int16_t) * numSamples * 2);

  // for (int i = 0; i < numSamples * 2; i++) stereo_buf[i] = 0;

  // switch(waveType) {
  //   case 0:
  //     generate_sine_wave(mono_buf, numSamples, freq);
  //     break;
  //   case 1:
  //     generate_square_wave(mono_buf, numSamples, freq);
  //     break;
  //   case 2:
  //     generate_saw_wave(mono_buf, numSamples, freq); 
  //     break; 
  //   default:
  //     fatal_error("Invalid wave type.");
  //     break;  
  // }


  // apply_gain(mono_buf, numSamples, gain);
  // mix_in(stereo_buf, 0, mono_buf, numSamples);
  // mix_in(stereo_buf, 1, mono_buf, numSamples);

  // write_s16_buf(out, stereo_buf, numSamples * 2);

  fclose(in);

  return 0;
}