/*
 *  Server.c
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
 *
 */

#include "iX-Yoke-plugin.h"

#if APL || LIN
#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#endif

#define PACKET_RATE_SENSITIVITY 0.2f


#if IBM
void server_loop(void *arg);
HANDLE server_thread;
#else
pthread_t server_thread;
void *server_loop(void *arg);
#endif


volatile long current_update_time = -1;
volatile int avg_packet_latency = 50;
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



void start_server()
{
    current_update_time = -1;
    server_error_string = NULL;
#if IBM
    WSADATA wsaData;
    WSAStartup(MAKEWORD(1, 1), &wsaData);
    _beginthread(server_loop, 0, NULL);
#else
	pthread_create(&server_thread, NULL, server_loop, NULL);
#endif
}


void stop_server()
{    
    // Send a kill tag to local server port.
    int sock;
    struct sockaddr_in sa;
    int bytes_sent;
    int buffer_length = 1;
    int8_t buffer[1];
    buffer[0] = kServerKillTag;
    
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == sock) /* if socket failed to initialize, exit */
    {
        iXDebug("Error creating socket to send server kill message");
        goto server_kill_end;
    }
    
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(0x7F000001);
    sa.sin_port = htons(kServerPort);
    
    bytes_sent = sendto(sock, buffer, buffer_length, 0, (struct sockaddr*)&sa, sizeof (struct sockaddr_in));
    if (bytes_sent < 0)
    {
        iXDebug(strerror(errno));
        goto server_kill_end;
    }
    
server_kill_end:
    closesocket(sock);
	// Should we try to kill the server thread by another means?
#if IBM
    WSACleanup();
#endif
}


void get_server_info(char *hostname, size_t hostname_size,
                     char *ips, size_t ips_size)
{
    iXDebug("Updating server info");
    // Retrieve this machine's IP addresses.
    // Using getifaddrs() would handle every network adapter, but is not Win32-compatible.
    // This method (using gethostname() and gethostbyname()) is probably good enough for most cases.
    ips[0] = '\0';
    if (gethostname(hostname, hostname_size) != 0)
    {
        strncpy(hostname, "Error retrieving host name.", hostname_size);
    }
    else
    {
#if IBM
        struct hostent *host = gethostbyname(hostname);
        if (host == NULL)
        {
            strncpy(ips, "Unable to determine IP addresses.", ips_size);
        }
        else
        {
            int first_time = 1;
            for (int i = 0; host->h_addr_list[i]; i++)
            {
                if (!first_time)
                {
                    strlcat(ips, ", ", ips_size);
                }
                struct in_addr host_addr = *(struct in_addr *)host->h_addr_list[i];
                char *addr = inet_ntoa(host_addr);
                strlcat(ips, addr, ips_size);
                first_time = 0;
            }
        }
#else
        struct ifaddrs *myaddrs, *ifa;
        void *in_addr;
        char buf[64];
        
        if(getifaddrs(&myaddrs) != 0)
        {
            perror("getifaddrs");
            exit(1);
        }
        
        int first_time = 1;
        for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr == NULL)
                continue;
            if (!(ifa->ifa_flags & IFF_UP))
                continue;
            if (strcmp("lo0", ifa->ifa_name) == 0)
                continue;
            
            switch (ifa->ifa_addr->sa_family)
            {
                case AF_INET:
                {
                    struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
                    in_addr = &s4->sin_addr;
                    break;
                }   
                default:
                    continue;
            }
            
            if (!first_time)
            {
                strlcat(ips, ", ", ips_size);
            }
            first_time = 0;
            if (inet_ntop(ifa->ifa_addr->sa_family, in_addr, buf, sizeof(buf)))
            {
                //strlcat(ips, ifa->ifa_name, ips_size);
                //strlcat(ips, "=", ips_size);
                strlcat(ips, buf, ips_size);
            }
            else
            {
                strlcat(ips, "?", ips_size);
            }
        }
        
        freeifaddrs(myaddrs);
#endif
    }
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
    ssize_t recv_size;
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(kServerPort);
    
    if (-1 == bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
    {
        server_error_string = "Socket error. Disable then re-enable plugin.";
        goto stop_server;
    }
    
    server_error_string = NULL;
    
    for (;;) 
    {
        recv_size = recvfrom(sock, (char *)buffer, kPacketSizeLimit, 0, NULL, NULL);
        if (recv_size < 0)
        {
            server_error_string = "Network error. Disable then re-enable plugin.";
            goto stop_server;
        }
        
        int i = 0;
        uint8_t tag = ix_get_tag(buffer, &i, recv_size);
        if (tag == kServerKillTag)
        {
            server_error_string = "Server killed. Disable then re-enable plugin.";
            goto stop_server;
        }
        else if (tag == kProtocolVersion1Tag ||
                 tag == kProtocolVersion2Tag)
        {
            server_error_string = NULL;
            for (int axis = 0; axis < kNumAxes; axis++)
            {
                iXControlAxisRef axisRef = get_axis((iXControlAxisID)axis);
                axisRef->prev_value = axisRef->value;
                axisRef->value = ix_get_ratio(buffer, &i, recv_size);
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
    
stop_server:
    closesocket(sock);
#if APL || LIN
    return NULL;
#endif
}

