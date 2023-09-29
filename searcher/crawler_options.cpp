//
// Created by shufy on 9/29/23.
//

#include "crawler_options.h"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fmt/core.h>

namespace po = boost::program_options;

std::optional<crawler_options> parse_crawler_options_from_argv(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("s",                   po::value<std::string>(),  "the IP address that we start scanning from. required.")
            ("threads",             po::value<int>(),               "number of threads. optional. default = 50")
            ("timeout-read",        po::value<int>(),          "read timeout in milliseconds. optional. default = 1000ms (1s)")
            ("timeout-connection",  po::value<int>(),          "connection timeout in milliseconds. optional. default = 1000ms (1s)")
            ("ports",               po::value<std::string>(),  "comma-separated list of ports. optional. default is 25565")
            ("distance",            po::value<int>(),          "the amount of adjacent neighbors to visit. higher = further ips scanned. optional. default is 3")
            ("crawl-direction",     po::value<std::string>(),  "the crawl mode. options for this are: forward, backward, both. optional. default is forward.")
            ("verbose-results",  "display all the valid servers with their MOTD and player count after scanning.");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const boost::program_options::error& e) {
        std::cerr << "Error parsing args: " << e.what() << std::endl;
        return std::nullopt; // Handle the error as needed
    }

    // set some sane defaults
    crawler_options options = {
        .verbose_results = false,
        .threads = 50,
        .timeout_read_ms = 1000,
        .timeout_connect_ms = 1000,
        .neighbor_distance = 3,
        .origin_ip = "",
        .crawl_direction = "forward",
        .ports = { 25565 }
    };

    if (vm.count("help") || vm.empty())
    {
        std::cout << desc << '\n'; // cant use fmt print because of << overload.
        return std::nullopt;
    }

    if (vm.count("s"))
    {
        const std::string target_ip = vm["s"].as<std::string>();
        if (!boost::regex_match(target_ip, ip_regex))
        {
            fmt::print("Invalid format for starting IP address {:?}. Must be a real ip in the format of {:?}\n", target_ip, "1.2.3.4");
            return std::nullopt;
        }
        options.origin_ip = target_ip;
    }

    if (vm.count("threads"))
    {
        const int thread_count = vm["threads"].as<int>();
        if (thread_count > 0)
            options.threads = static_cast<size_t>(thread_count);
        else fmt::print("[Warning][{}] Invalid value for option {:?} ({:?}), ignoring.\n", __FUNCTION__, "threads", std::to_string(thread_count));
    }

    if (vm.count("timeout_read"))
    {
        const int read_timeout = vm["timeout_read"].as<int>();
        if (read_timeout > 0)
            options.timeout_read_ms = static_cast<size_t>(read_timeout);
        else fmt::print("[Warning][{}] Invalid value for option {:?} ({:?}), ignoring.\n", __FUNCTION__, "timeout_read", std::to_string(read_timeout));
    }

    if (vm.count("timeout_connection")) {
        const int connection_timeout    = vm["timeout_connection"].as<int>();
        if (connection_timeout > 0)
            options.timeout_connect_ms  = static_cast<size_t>(connection_timeout);
        else fmt::print("[Warning][{}] Invalid value for option {:?} ({:?}), ignoring.\n", __FUNCTION__, "timeout_connection", std::to_string(connection_timeout));
    }

    if (vm.count("ports")) {
        const std::string ports_str = vm["ports"].as<std::string>();

        std::vector<std::string> ports;
        boost::split(ports, ports_str, boost::is_any_of(","));

        std::string current_port;
        for (const auto& port : ports)
        {
            try
            {
                current_port = port;
                const unsigned short port_int = std::stoi(port);
                options.ports.push_back(port_int);
            }
            catch (const std::invalid_argument& e)
            {
                fmt::print("[Warning][{}] Invalid port {:?} provided in ports list argument. It will not be added to the scanner.\n", __FUNCTION__, current_port);
            }
        }
    }

    if (vm.count("distance"))
    {
        const int distance = vm["distance"].as<int>();
        if (distance > 0)
            options.neighbor_distance = static_cast<size_t>(distance);
        else fmt::print("[Warning][{}] Invalid value for option {:?} ({:?}), ignoring.\n", __FUNCTION__, "distance", std::to_string(distance));
    }

    if (vm.count("crawl_direction"))
    {
        const std::string crawl_direction = vm["crawl_direction"].as<std::string>();
        if (!crawl_direction.empty())
        {
            if (crawl_direction == "forward" || crawl_direction == "backward" || crawl_direction == "both")
                options.crawl_direction = crawl_direction;
            else
                fmt::print("[Warning][{}] Invalid value for option {:?} ({:?}), ignoring.\n", __FUNCTION__, "distance", crawl_direction);
        }
        else
            fmt::print("[Warning][{}] Option {:?} supplied without a value. Ignoring. Using default\n", __FUNCTION__, "distance");
    }

    if (vm.count("verbose-results"))
    {
        options.verbose_results = true;
    }

    return std::make_optional<crawler_options>(options);
}