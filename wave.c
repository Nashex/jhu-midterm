#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "io.h"
#include "wave.h"

void write_wave_header(FILE *out, unsigned num_samples) {
  //
  // See: http://soundfile.sapp.org/doc/WaveFormat/
  //

  uint32_t ChunkSize, Subchunk1Size, Subchunk2Size;
  uint16_t NumChannels = NUM_CHANNELS;
  uint32_t ByteRate = SAMPLES_PER_SECOND * NumChannels * (BITS_PER_SAMPLE/8u);
  uint16_t BlockAlign = NumChannels * (BITS_PER_SAMPLE/8u);

  // Subchunk2Size is the total amount of sample data
  Subchunk2Size = num_samples * NumChannels * (BITS_PER_SAMPLE/8u);
  Subchunk1Size = 16u;
  ChunkSize = 4u + (8u + Subchunk1Size) + (8u + Subchunk2Size);

  // Write the RIFF chunk descriptor
  write_bytes(out, "RIFF", 4u);
  write_u32(out, ChunkSize);
  write_bytes(out, "WAVE", 4u);

  // Write the "fmt " sub-chunk
  write_bytes(out, "fmt ", 4u);       // Subchunk1ID
  write_u32(out, Subchunk1Size);
  write_u16(out, 1u);                 // PCM format
  write_u16(out, NumChannels);
  write_u32(out, SAMPLES_PER_SECOND); // SampleRate
  write_u32(out, ByteRate);
  write_u16(out, BlockAlign);
  write_u16(out, BITS_PER_SAMPLE);

  // Write the beginning of the "data" sub-chunk, but not the actual data
  write_bytes(out, "data", 4);        // Subchunk2ID
  write_u32(out, Subchunk2Size);
}

void read_wave_header(FILE *in, unsigned *num_samples) {
  char label_buf[4];
  uint32_t ChunkSize, Subchunk1Size, SampleRate, ByteRate, Subchunk2Size;
  uint16_t AudioFormat, NumChannels, BlockAlign, BitsPerSample;

  read_bytes(in, label_buf, 4u);
  if (memcmp(label_buf, "RIFF", 4u) != 0) {
    fatal_error("Bad wave header (no RIFF label)");
  }

  read_u32(in, &ChunkSize); // ignore

  read_bytes(in, label_buf, 4u);
  if (memcmp(label_buf, "WAVE", 4u) != 0) {
    fatal_error("Bad wave header (no WAVE label)");
  }

  read_bytes(in, label_buf, 4u);
  if (memcmp(label_buf, "fmt ", 4u) != 0) {
    fatal_error("Bad wave header (no 'fmt ' subchunk ID)");
  }

  read_u32(in, &Subchunk1Size);
  //printf("Subchunk1Size=%u\n", Subchunk1Size);
  if (Subchunk1Size != 16u) {
    fatal_error("Bad wave header (Subchunk1Size was not 16)");
  }

  read_u16(in, &AudioFormat);
  if (AudioFormat != 1u) {
    fatal_error("Bad wave header (AudioFormat is not PCM)");
  }

  read_u16(in, &NumChannels);
  if (NumChannels != NUM_CHANNELS) {
    fatal_error("Bad wave header (NumChannels is not 2)");
  }

  read_u32(in, &SampleRate);
  if (SampleRate != SAMPLES_PER_SECOND) {
    fatal_error("Bad wave header (Unexpected sample rate)");
  }

  read_u32(in, &ByteRate); // ignore

  read_u16(in, &BlockAlign); // ignore

  read_u16(in, &BitsPerSample);
  if (BitsPerSample != BITS_PER_SAMPLE) {
    fatal_error("Bad wave header (Unexpected bits per sample)");
  }

  read_bytes(in, label_buf, 4u);
  if (memcmp(label_buf, "data", 4u) != 0) {
    fatal_error("Bad wave header (no 'data' subchunk ID)");
  }

  // finally we're at the Subchunk2Size field, from which we can
  // determine the number of samples
  read_u32(in, &Subchunk2Size);
  *num_samples = Subchunk2Size / NUM_CHANNELS / (BITS_PER_SAMPLE/8u);
}

void compute_pan(float angle, float channel_gain[]) {
  channel_gain[0] = (sqrt(2.0) / 2.0) * (cos(angle) + sin(angle));
  channel_gain[1] = (sqrt(2.0) / 2.0) * (cos(angle) - sin(angle));
}

// Basic full-amplitude wave generation into a mono sample buffer
void generate_sine_wave(int16_t mono_buf[], unsigned num_samples, float freq_hz) {
  float phaseInc = 2*PI*freq_hz / num_samples;
  float currentPhase = 0;
  for (unsigned int i = 0; i < num_samples; i++) {
    mono_buf[i] = 32767 * sin(currentPhase);
    currentPhase += phaseInc;
  }
}

void generate_square_wave(int16_t mono_buf[], unsigned num_samples, float freq_hz) {
  float phaseInc = 2*PI*freq_hz / num_samples;
  float currentPhase = 0;
  for (unsigned int i = 0; i < num_samples; i++) {
    mono_buf[i] = 32767 * (sin(currentPhase) > 0 ? 1 : -1);
    currentPhase += phaseInc;
  }
}

void generate_saw_wave(int16_t mono_buf[], unsigned num_samples, float freq_hz) {
  for (unsigned int i = 0; i < num_samples; i++) {
    mono_buf[i] = (fmod(freq_hz*i/num_samples,1.0) * 2 - 1) * 32767;
  }
}

// Attenuate each sample in a mono sample buffer by specified factor
void apply_gain(int16_t mono_buf[], unsigned num_samples, float gain) {
  for (unsigned int i = 0; i < num_samples; i++) {
    if (gain * mono_buf[i] > 32767) mono_buf[i] = 32767;
    else if (gain * mono_buf[i] < -32767) mono_buf[i] = -32767;
    else mono_buf[i] = gain * mono_buf[i];
  }
}

void apply_adsr_envelope(int16_t mono_buf[], unsigned num_samples) {
  for (int i = 0; i < ATTACK_NUM_SAMPLES; i++) {
    if (mono_buf[i] * 1.2 * ((float) i / ATTACK_NUM_SAMPLES) > 32767) mono_buf[i] = 32767;
    else if (mono_buf[i] * 1.2 * ((float) i / ATTACK_NUM_SAMPLES) < -32767) mono_buf[i] = -32767;
    else mono_buf[i] = mono_buf[i] * 1.2 * ((float) i / ATTACK_NUM_SAMPLES);
  }
  for (int i = ATTACK_NUM_SAMPLES; i < ATTACK_NUM_SAMPLES + DECAY_NUM_SAMPLES; i++) {
    if (mono_buf[i] * 1.2 * (1 - (((float) i / DECAY_NUM_SAMPLES) * 5)) > 32767) mono_buf[i] = 32767;
    else if (mono_buf[i] * 1.2 * (1 - (((float) i / DECAY_NUM_SAMPLES) * 5)) < -32767) mono_buf[i] = -32767;
    else mono_buf[i] = mono_buf[i] * 1.2 * (1 - (((float) i / DECAY_NUM_SAMPLES) * 5));
  }
  for (int i = num_samples - RELEASE_NUM_SAMPLES; i < num_samples; i++) {
    if (mono_buf[i]* (1 - (((float) i / DECAY_NUM_SAMPLES))) > 32767) mono_buf[i] = 32767;
    else if (mono_buf[i] * (1 - (((float) i / DECAY_NUM_SAMPLES))) < -32767) mono_buf[i] = -32767;
    else mono_buf[i] = mono_buf[i] * (1 - (((float) i / DECAY_NUM_SAMPLES)));
  }
}

void mix_in(int16_t stereo_buf[], unsigned channel, const int16_t mono_buf[], unsigned num_samples) {
  for (unsigned int i = channel; i < num_samples * 2; i+=2) {
    if (stereo_buf[i] + mono_buf[i / 2] > 32767) stereo_buf[i] = 32767;
    else if (stereo_buf[i] + mono_buf[i / 2] < -32767)  stereo_buf[i] = -32767;
    else stereo_buf[i] += mono_buf[i / 2];
  }
}

