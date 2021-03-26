#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "io.h"

// TODO: implement the functions declared in io.h
void fatal_error(const char *message) {
  fprintf(stderr, "Error: %s", message);
  exit(1);
}

void write_byte(FILE *out, char val) {
  if (fwrite(&val, sizeof(char), 1, out) != 1) fatal_error("Failed to write.");
}
void write_bytes(FILE *out, const char data[], unsigned n) {
  for (int i = 0; i < n; i++) {
    if (fwrite(&data[i], sizeof(char), 1, out) != 1) fatal_error("Failed to write.");
  }
}

void write_u16(FILE *out, uint16_t value) {
  if (fwrite(&value, sizeof(uint16_t), 1, out) != 1) fatal_error("Failed to write.");
}

void write_u32(FILE *out, uint32_t value) {
  if (fwrite(&value, sizeof(uint32_t), 1, out) != 1) fatal_error("Failed to write."); 
}

void write_s16(FILE *out, int16_t value) {
  if (fwrite(&value, sizeof(int16_t), 1, out) != 1) fatal_error("Failed to write."); 
}

void write_s16_buf(FILE *out, const int16_t buf[], unsigned n) {
  for (int i = 0; i < n; i++) {
    write_s16(out, buf[i]);
  }
}

void read_byte(FILE *in, char *val) {
  if (fread(&val, sizeof(char), 1, in) != 1) fatal_error("Failed to read."); 
}

void read_bytes(FILE *in, char data[], unsigned n) {
  int count = fread(data, sizeof(char), n, in);
  if (count != n) fatal_error("Failed to read to buffer."); 
}

void read_u16(FILE *in, uint16_t *val) {
  int count = fread(val, sizeof(uint16_t), 1, in);
  if (count != 1) fatal_error("Failed to read uint16_t."); 
}

void read_u32(FILE *in, uint32_t *val) {
  if (fread(val, sizeof(uint32_t), 1, in) != 1) fatal_error("Failed to read uint32_t."); 
}

void read_s16(FILE *in, int16_t *val) {
  if (fread(val, sizeof(int16_t), 1, in) != 1) fatal_error("Failed to read int16_t."); 
}

void read_s16_buf(FILE *in, int16_t buf[], unsigned n) {
  for (int i = 0; i < n; i++) {
    read_s16(in, &buf[i]); 
  }
}