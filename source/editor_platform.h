#ifndef EDITOR_PLATFORM_H
#define EDITOR_PLATFORM_H


#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include <float.h>


typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} vec3;

typedef struct {
    f32 x;
    f32 y;
    f32 width;
    f32 height;
} Rect;

#endif
