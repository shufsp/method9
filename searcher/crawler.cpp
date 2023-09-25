
#include <boost/regex.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <fmt/core.h>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <mutex>
#include <ostream>
#include <thread>

#include "crawler_network.h"

namespace fs = std::filesystem;

static const boost::regex ip_regex(R"(^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)\.?\b){4}$)");

// Given a starting ip `origin_ip` and a list of offsets, generates a set of unique IP addresses where each IP address
// neighbors the `origin_ip` at some offset within the given list of offsets.
std::unordered_set<std::string> jehovas_generate(const ip_parts& origin_ip, const std::vector<int>& offsets)
{
    fmt::print("Generating neighborhood... ");

    // make our ips list a set so that we dont accidentally overlap during scan and have duplicates
    std::unordered_set<std::string> ips;
    const size_t neighborhood_size = offsets.size() * 255;
    ips.reserve(neighborhood_size);

    for (const int offset : offsets)
    {
        // at each offset given, we visit all possible "nodes" at that octet offset
        // this is quite fast because all we're doing to get the neighbor is bitwise AND
        for (int i = 0; i < 255; ++i)
        {
            const auto neighbor_ip = ip_neighbor(origin_ip, offset, i);
            const auto neighbor_ip_str = fmt::format("{}.{}.{}.{}", neighbor_ip.first, neighbor_ip.second, neighbor_ip.third, neighbor_ip.fourth);
            if (!boost::regex_match(neighbor_ip_str, ip_regex))
            {
                // this should never happen
                fmt::print("[WARNING] {} is not a valid ip\n", neighbor_ip_str); 
                continue; // neighbor isn't a valid ip.. ?
            }

            fmt::print("\rGenerating neighborhood... ({} / {})", ips.size(), neighborhood_size);
            ips.insert(neighbor_ip_str);
        }
    }
    fmt::print("\n");

    return ips;
}

// Given a unique set of ip address strings, we summon some threads to check which ips in the set are ONLINE
// The default port checked is minecraft's port 25565, TODO plans to have port scanning in the future
void jehovas_witness(const std::unordered_set<std::string>& ips)
{
    // mutexes, since we are going to use a thread pool
    std::mutex print_mut;
    std::mutex vector_mut;

    // prepare our jehovas witness force (thread pool)
    // you can adjust the threads higher than 100 if you want. just dont blow up your computer.,..,
    const auto IP_COUNT = ips.size();
    boost::asio::thread_pool pool(100);

    // set up our crawler
    int crawlers_done = 0;
    std::vector<std::string> valid_servers;
    std::atomic<int> tasks_completed(0);

    // lambda function that each thread will run
    const auto crawler = [&](const std::string& ip)
    {
        // Port we're going to check
        constexpr unsigned short MC_PORT = 25565;

        // Knock on the port's door (check if the port is open to the public)
        bool they_answered = ip_tcp_is_open(ip.data(), MC_PORT, 1000);
        if (they_answered)
        {
            // the port is open! It's MOST LIKELY a running minecraft server
            std::lock_guard<std::mutex> vec_lock(vector_mut);  // so we can print from multiple threads
            valid_servers.push_back(ip);
        }
        {
            // server did not respond within the given timeout, or port is closed on this ip address.
            // TODO options to adjust timeout
            std::lock_guard<std::mutex> print_lock(print_mut);  // so we can print from multiple threads
            fmt::print("Scanning {} ... ({} / {})              \r", ip, ++crawlers_done, IP_COUNT);
        }
        tasks_completed.fetch_add(1, std::memory_order_relaxed);
    };

    // start scanning
    fmt::print("Mounting crawlers ... \n");
    for (const auto& ip : ips)
    {
        // Throw our crawler lambda into the thread pool and execute
        boost::asio::post(pool, [Func = crawler, ip] { return Func(ip); });
    }

    // wait for all of our threads to finish checking ips
    pool.join();

    // export crawler results
    // TODO option to change txt path
    constexpr std::string_view valid_servers_path = "pirates_servers_list.txt"; 
    fmt::print("\n--------------------------------\n");
    fmt::print("{} servers scanned", ips.size());
    fmt::print("\n{} valid servers\n", valid_servers.size());
    fmt::print("Wrote valid servers to \"{}\"\n", valid_servers_path);
    fmt::print("--------------------------------\n");
    std::ofstream valid_servers_file{ valid_servers_path.data() };
    for (const auto& ip : valid_servers)
    {
        valid_servers_file << ip << '\n';
    }
    valid_servers_file.close();
    fmt::print("\n");
}

// If center is 0 and range is 3, this will return { -3, -2, -1, 0, 1, 2, 3 }
std::vector<int> create_offset_range(int center, int range) 
{
    std::vector<int> result;
    for (int i = -range; i <= range; ++i) 
        result.push_back(center + i);
    return result;
}

int main(int argc, char** argv)
{
    // no args given
   if (argc == 1)
   {
       fmt::print("Usage: server-crawler <ip> [range]\n");
       return -1;
   }

   // at least one arg given
   // Make sure starting IP is valid
   const char* ip = argv[1]; 
   if (!boost::regex_match(ip, ip_regex))
   {
       fmt::print("Invalid input \"{}\"\n", ip);
       return -1;
   }

   try // believe me, I tried to not use exceptions
   {
       // get custom offset
       // TODO add more options, or even command flags system
       const auto offset = argc < 3 ? 3 : std::stoi(argv[2]);

       // split our IP string into the 4 octet int sections
       const auto ip_addr = ip_str_to_parts(ip);

       // create the neighbor offsets to check
       const auto ip_offsets = create_offset_range(0, offset);

       // generate the ips to check with our offsets
       const auto ips = jehovas_generate(ip_addr, ip_offsets); 

       // Crawl.
       jehovas_witness(ips);
   }
   catch (std::exception& ex)
   {
       fmt::print("Invalid input\n");
   }

   return 0;
}


