#include <fmt/core.h>
#include <vector>
#include <fstream>
#include <ostream>
#include <numeric>
#include <optional>

#include "crawler_network.h"
#include "crawler_options.h"


int main(int argc, char** argv)
{
    // TODO proper arg flags
    const std::optional<crawler_options> maybe_options = parse_crawler_options_from_argv(argc, argv);
    if (!maybe_options.has_value()) // "help" or invalid command was passed in
        return 1;

    const crawler_options& options = maybe_options.value();

    // use options to set up scanner
    // create the neighbor offsets to check
    // range from 0 to `range`
    const auto ip_offsets = [&](size_t range)
    {
        std::vector<int> result(range);
        std::iota(result.begin(), result.end(), 0);
        return result;
    }(options.neighbor_distance);

    // split our IP string into the 4 octet int sections
    const auto ip_addr = ip_str_to_parts(options.origin_ip);

    // generate the ips to check with our offsets
    const auto ips = ip_neighbor_set(ip_addr, ip_offsets);

    // print every port we're going to check on every ip
    // Crawl.
    const auto valid_servers = ip_crawl_horizontal(ips, options.ports, options.threads, options.timeout_connect_ms, options.timeout_read_ms);

    fmt::print("\n-------------------------------------\n");
    fmt::print("Unique IPs scanned: {}\n", ips.size() * options.ports.size());
    fmt::print("Valid servers: {}\n", valid_servers.size());
    fmt::print("-------------------------------------\n");

    if (valid_servers.empty())
        return 0; // dont bother saving if theres no ips

    // Save
    const std::string ip_formatted_as_file_name_appropriate = boost::regex_replace(std::string(options.origin_ip), boost::regex("[^0-9]"), "_");
    const std::string file_name = fmt::format("server_list_{}.txt", ip_formatted_as_file_name_appropriate);
    std::ofstream out_file{ file_name };

    // display valid ips
    if (options.verbose_results)
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

    return 0;
}


