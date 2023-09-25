//
// Created by shufy on 9/24/23.
//

#ifndef SERVER_CRAWL_CRAWLER_NETWORK_H
#define SERVER_CRAWL_CRAWLER_NETWORK_H

// Example of usage:
// IP string: 209.182.102.5
// ip_parts: first=209 second=182 third=102 fourth=5
struct ip_parts
{
    int first;
    int second;
    int third;
    int fourth;
};

bool ip_tcp_is_open(const char* ip, unsigned short port, int timeout_ms = 5'000);
ip_parts ip_str_to_parts(const char* ip);
ip_parts ip_neighbor(ip_parts addr, int neighbor_offset, int node);


#endif //SERVER_CRAWL_CRAWLER_NETWORK_H
