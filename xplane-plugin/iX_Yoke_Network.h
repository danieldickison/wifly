/*
 *  iX-Yoke-Network.h
 *  Wi-Fly-plugin
 *
 *  Created by Daniel Dickison on 5/11/09.
 *  Copyright 2009 Daniel_Dickison. All rights reserved.
 *
 * 
 *  This file is part of Wi-Fly-plugin.
 *  
 *  Wi-Fly-plugin is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  Wi-Fly-plugin is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with Wi-Fly-plugin.  If not, see <http://www.gnu.org/licenses/>.
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
