#pragma once

struct Vertex {
  float x;
  float y;
  float z;
} __attribute__((packed));

struct Color {
  float r;
  float g;
  float b;
} __attribute__((packed));

struct UV {
  float u;
  float v;
} __attribute__((packed));