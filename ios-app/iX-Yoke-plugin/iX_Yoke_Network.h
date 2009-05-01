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


UInt8 ix_get_tag(UInt8 *buffer, int *i) {
    return (UInt8)buffer[(*i)++];
}
void ix_put_tag(UInt8 *buffer, int *i, int tag) {
    buffer[(*i)++] = tag;
}

// Ratios are represented as floats and transmitted as signed big-endian 16-bit integers
float ix_get_ratio(UInt8 *buffer, int *i) {
    SInt16 n = buffer[(*i)++] << 8;
    n |= buffer[(*i)++];
    return (float)n / 0x7fff;
}
void ix_put_ratio(UInt8 *buffer, int *i, float ratio) {
    SInt16 n = (SInt16)(ratio * 0x7fff);
    buffer[(*i)++] = (n >> 8);
    buffer[(*i)++] = (n & 0xff);
}


#endif