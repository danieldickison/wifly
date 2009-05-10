/*
 *  Server.c
 *  iX-Yoke-plugin
 *
 *  Created by Daniel Dickison on 5/9/09.
 *  Copyright 2009 Daniel_Dickison. All rights reserved.
 *
 */

#include "iX-Yoke-plugin.h"


/****** UDP Server ******/

void *server_loop(void *arg)
{
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    struct sockaddr_in addr; 
    UInt8 buffer[kPacketSizeLimit];
    size_t recv_size;
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(kServerPort);
    
    if (-1 == bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr)))
    {
        server_msg = "Unable to bind server socket.";
        goto stop_server;
    }
    
    server_msg = "Starting server loop.";
    
    for (;;) 
    {
        recv_size = recvfrom(sock, (void *)buffer, kPacketSizeLimit, 0, NULL, NULL);
        if (recv_size < 0)
        {
            server_msg = strerror(errno);
            goto stop_server;
        }
        /*
         char debugstr[64];
         sprintf(debugstr, "Recv throttle %f", throt);
         server_msg = debugstr;
         */
        
        int i = 0;
        while (i < recv_size)
        {
            UInt8 tag = ix_get_tag(buffer, &i);
            if (tag == kServerKillTag)
            {
                server_msg = "Server kill received";
                goto stop_server;
            }
            else if (tag == kProtocolVersion1Tag)
            {
                for (int axis = 0; axis < kNumAxes; axis++)
                {
                    get_axis(axis)->value = ix_get_ratio(buffer, &i);
                }
            }
            // else ignore packet
        }
    }
    
stop_server:
    close(sock);
    return NULL;
}

