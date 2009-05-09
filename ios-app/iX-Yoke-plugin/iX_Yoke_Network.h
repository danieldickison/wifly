/*
 *  iX-Yoke-Network.h
 *  iX-Yoke-plugin
 *
 *  Created by Daniel Dickison on 4/29/09.
 *  Copyright 2009 Daniel_Dickison. All rights reserved.
 *
 */


#ifndef _IX_YOKE_NETWORK_H
#define _IX_YOKE_NETWORK_H


#define kServerPort 56244
#define kPacketSizeLimit 1024

enum {
    kServerKillTag,
    kProtocolVersion1Tag
};


static inline uint8_t ix_get_tag(uint8_t *buffer, int *i) {
    return (uint8_t)buffer[(*i)++];
}
static inline void ix_put_tag(uint8_t *buffer, int *i, int tag) {
    buffer[(*i)++] = tag;
}

// Ratios are represented as floats in the range [0, 1] and transmitted as unsigned big-endian 16-bit integers
static inline float ix_get_ratio(uint8_t *buffer, int *i) {
    uint16_t n = (buffer[(*i)++] << 8);
    n |= buffer[(*i)++];
    return (float)n / 0xffff;
}
static inline void ix_put_ratio(uint8_t *buffer, int *i, float ratio) {
    uint16_t n = (uint16_t)(ratio * 0xffff);
    buffer[(*i)++] = (n >> 8);
    buffer[(*i)++] = (n & 0xff);
}


#endif