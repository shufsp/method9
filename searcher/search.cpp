#include <fmt/core.h>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <vector>
#include <fstream>
#include <ostream>
#include <numeric>
#include "crawler_network.h"

int main(int argc, char** argv)
{
    // TODO proper arg flags

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
       fmt::print("Invalid input {:?}\n", ip);
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
       // range from 0 to `range`
       const auto ip_offsets = [&](size_t range)
       {
           std::vector<int> result(range);
           std::iota(result.begin(), result.end(), 0);
           return result;
       }(offset);

       // generate the ips to check with our offsets
       const auto ips = ip_neighbor_set(ip_addr, ip_offsets);

       // debug ips that are valid
       // const std::unordered_set<std::string> ips{ "209.192.194.167","209.192.194.92" };

       // generate a port list in range [begin, end)
       const auto ip_ports_to_check = [&](size_t begin, size_t end)
       {
           std::vector<unsigned short> result(end - begin);
           // iota is a method that creates a number sequence
           std::iota(result.begin(), result.end(), begin);
           return result;
       }(25562, 25570);

       // print every port we're going to check on every ip
       fmt::print("Ports list: ");
       for (const auto& port : ip_ports_to_check)
           fmt::print("{},", port);
       fmt::print("\n");

       // Crawl.
       const auto valid_servers = ip_crawl_horizontal(ips, ip_ports_to_check);

       // Save
       const std::string ip_formatted_as_file_name_appropriate = boost::regex_replace(std::string(ip), boost::regex("[^0-9]"), "_");
       const std::string file_name = fmt::format("server_list_{}.txt", ip_formatted_as_file_name_appropriate);
       std::ofstream out_file{ file_name };

       fmt::print("\n-------------------------------------\n");
       fmt::print("Unique IPs scanned: {}\n", ips.size() * ip_ports_to_check.size());
       fmt::print("Valid servers: {}\n", valid_servers.size());
       fmt::print("-------------------------------------\n");

       // display valid ips
       for (const auto& server : valid_servers)
       {
           out_file << server.host << ':' << server.port << '\n';
           fmt::print("{}:{}\t\tPlayers:\t({}/{})\t\tMOTD:\t{:?}\n",
                      server.host,
                      server.port,
                      server.server_list_info.player_count,
                      server.server_list_info.max_players,
                      server.server_list_info.motd);
       }
       fmt::print("\n");
       out_file.close();
       fmt::print("Valid IPs list saved to {:?}\n\n", file_name);
   }
   catch (std::exception& ex)
   {
       fmt::print("Invalid input\n");
   }

   return 0;
}


