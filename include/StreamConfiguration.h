#ifndef STREAM_CONFIGURATION_H
#define STREAM_CONFIGURATION_H

#include <stdint.h>

#define VIDEO_FORMAT_H264 0
#define VIDEO_FORMAT_HEVC 1

typedef struct {
    int width;
    int height;
} VideoResolution;

typedef struct {
    uint8_t videoFormat;
    VideoResolution resolution;
    uint32_t bitrateKbps;
    uint32_t frameRate;
} StreamConfiguration;

#endif // STREAM_CONFIGURATION_H
