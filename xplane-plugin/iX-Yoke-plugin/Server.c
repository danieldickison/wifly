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

#define PACKET_RATE_SENSITIVITY 0.2f

volatile long current_update_time = -1;
volatile int avg_packet_latency = 50;
char *server_ips = "";
char *server_hostname = "";
char *server_error_string = NULL;


/****** UDP Server ******/

long get_last_packet_time()
{
    return current_update_time;
}

int get_packet_rate()
{
    // Avoid divide-by-zero for strangely rapid packets.
    if (avg_packet_latency == 0) return 20;
    
    return 1000 / avg_packet_latency;
}

char *get_server_error_string()
{
    return server_error_string;
}


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
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(kServerPort);
    
    if (-1 == bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
    {
        server_msg = "Unable to bind server socket.";
        server_error_string = "Socket error. Disable then re-enable plugin.";
        goto stop_server;
    }
    
    // Retrieve this machine's IP addresses.
    // Using getifaddrs() would handle every network adapter, but is not Win32-compatible.
    // This method (using gethostname() and gethostbyname()) is probably good enough for most cases.
    server_hostname = calloc(255, sizeof(char));
    if (gethostname(server_hostname, 255) != 0)
    {
        server_hostname = "Error retrieving host name.";
    }
    else
    {
        struct hostent *host = gethostbyname(server_hostname);
        if (host == NULL)
        {
            server_ips = "Unable to determine IP addresses.";
        }
        else
        {
            const int str_len = 512;
            server_ips = calloc(str_len, sizeof(char));
            int first_time = 1;
            for (int i = 0; host->h_addr_list[i]; i++)
            {
                if (!first_time)
                {
                    strlcat(server_ips, ", ", str_len);
                }
                struct in_addr host_addr = *(struct in_addr *)host->h_addr_list[i];
                char *addr = inet_ntoa(host_addr);
                strlcat(server_ips, addr, str_len);
                first_time = 0;
            }
        }
    }
    
    server_msg = "Starting server loop.";
    server_error_string = NULL;
    
    for (;;) 
    {
        recv_size = recvfrom(sock, (char *)buffer, kPacketSizeLimit, 0, NULL, NULL);
        if (recv_size < 0)
        {
            server_msg = strerror(errno);
            server_error_string = "Network error. Disable then re-enable plugin.";
            goto stop_server;
        }
        
        int i = 0;
        while (i < recv_size)
        {
            uint8_t tag = ix_get_tag(buffer, &i);
            if (tag == kServerKillTag)
            {
                server_msg = "Server kill received.";
                server_error_string = "Server killed. Disable then re-enable plugin.";
                goto stop_server;
            }
            else if (tag == kProtocolVersion1Tag)
            {
                server_error_string = NULL;
                for (int axis = 0; axis < kNumAxes; axis++)
                {
                    iXControlAxisRef axisRef = get_axis((iXControlAxisID)axis);
                    axisRef->prev_value = axisRef->value;
                    axisRef->value = ix_get_ratio(buffer, &i);
                }
                int prev_update_time = current_update_time;
                current_update_time = get_ms_time();
                if (prev_update_time != -1)
                {
                    int delta = current_update_time - prev_update_time;
                    avg_packet_latency = (PACKET_RATE_SENSITIVITY * delta +
                                          (1 - PACKET_RATE_SENSITIVITY) * avg_packet_latency);
                }
            }
            else
            {
                server_error_string = "New plugin required! http://danieldickison.com/wifly";
            }
        }
    }
    
stop_server:
    closesocket(sock);
#if APL || LIN
    return NULL;
#endif
}

