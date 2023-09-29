//
// Created by shufy on 9/24/23.
//

#ifndef SERVER_CRAWL_CRAWLER_NETWORK_H
#define SERVER_CRAWL_CRAWLER_NETWORK_H
#include <unordered_set>
#include <vector>
#include <boost/regex.hpp>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

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

struct minecraft_server_tcp_info
{
    bool online;
    size_t player_count;
    size_t max_players;
    std::string motd;
};

struct minecraft_server
{
    std::string host;
    unsigned short port;
    minecraft_server_tcp_info server_list_info;
};

bool ip_tcp_is_open(const std::string_view ip, const unsigned short port, const size_t timeout_ms = 5'000);
ip_parts ip_str_to_parts(const std::string_view ip);
ip_parts ip_neighbor(ip_parts addr, const int neighbor_offset, const int node);
std::vector<minecraft_server> ip_crawl_horizontal(const std::unordered_set<std::string>& ips, const std::vector<unsigned short>& ports, const size_t threads = 1'000,const size_t connection_timeout_ms = 1'000, const size_t read_timeout_ms = 1'000);
void ip_crawl_vertical(const std::string_view ip);
std::unordered_set<std::string> ip_neighbor_set(const ip_parts& origin_ip, const std::vector<int>& offsets);
minecraft_server_tcp_info ip_tcp_minecraft_server_ping(const std::string_view ip, const unsigned short port, const size_t connection_timeout_ms = 1'000, const size_t read_timeout_ms = 1'000);


#endif //SERVER_CRAWL_CRAWLER_NETWORK_H
