/*
 *  Server.c
 *  iX-Yoke-plugin
 *
 *  Created by Daniel Dickison on 5/9/09.
 *  Copyright 2009 Daniel_Dickison. All rights reserved.
 *
 */

#include "iX-Yoke-plugin.h"

#if APL || LIN
#include <arpa/inet.h>
#include <errno.h>
#endif


long current_update_time = -1;
char *server_ips = "";


/****** UDP Server ******/

#if IBM
void server_loop(LPVOID pParameter)
#else
void *server_loop(void *arg)
#endif
{
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    struct sockaddr_in addr; 
    uint8_t buffer[kPacketSizeLimit];
    size_t recv_size;
    socklen_t addr_len = 0;
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(kServerPort);
    
    if (-1 == bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
    {
        server_msg = "Unable to bind server socket.";
        goto stop_server;
    }
    
    // Retrieve this machine's IP addresses.
    addr_len = sizeof(addr);
    struct ifaddrs *addresses;
    if (getifaddrs(&addresses) != 0)
    {
        server_ips = "Could not determine host IP";
    }
    else
    {
        int str_len = 512;
        char addr_str[INET_ADDRSTRLEN];
        server_ips = calloc(str_len, sizeof(char));
        str_len--; //Leave room for null terminator.
        while (addresses)
        {
            struct sockaddr *address = addresses->ifa_addr;
            if (address->sa_family == AF_INET &&
                strcmp(addresses->ifa_name, "lo0"))
            {
                struct sockaddr_in *addr_in = (struct sockaddr_in *)address;
                if (inet_ntop(address->sa_family, &addr_in->sin_addr, addr_str, address->sa_len))
                {
                    if (server_ips[0])
                    {
                        strncat(server_ips, ", ", str_len);
                        str_len -= 2;
                    }
                    strncat(server_ips, addr_str, str_len);
                    str_len -= strlen(addr_str);
                    strncat(server_ips, " (", str_len);
                    str_len -= 2;
                    strncat(server_ips, addresses->ifa_name, str_len);
                    str_len -= strlen(addresses->ifa_name);
                    strncat(server_ips, ")", str_len);
                    str_len -= 2;
                }
            }
            addresses = addresses->ifa_next;
        }
        freeifaddrs(addresses);
    }
    
    server_msg = "Starting server loop.";
    
    for (;;) 
    {
        recv_size = recvfrom(sock, (char *)buffer, kPacketSizeLimit, 0, NULL, NULL);
        if (recv_size < 0)
        {
            server_msg = strerror(errno);
            goto stop_server;
        }
        
        int i = 0;
        while (i < recv_size)
        {
            uint8_t tag = ix_get_tag(buffer, &i);
            if (tag == kServerKillTag)
            {
                server_msg = "Server kill received";
                goto stop_server;
            }
            else if (tag == kProtocolVersion1Tag)
            {
                for (int axis = 0; axis < kNumAxes; axis++)
                {
                    get_axis((iXControlAxisID)axis)->value = ix_get_ratio(buffer, &i);
                }
                current_update_time = get_ms_time();
            }
            // else ignore packet
        }
    }
    
stop_server:
    closesocket(sock);
#if APL || LIN
    return NULL;
#endif
}

