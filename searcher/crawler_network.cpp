//
// Created by shufy on 9/24/23.
//
#include "crawler_network.h"
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <fmt/core.h>
#include <tuple>
#include <codecvt>
#include <fstream>

// Checks if the given IP's port is open to TCP connections
bool ip_tcp_is_open(const std::string_view ip, const unsigned short port, const int timeout_ms)
{
    // set up our socket
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket(io_service);
    socket.open(boost::asio::ip::tcp::v4());
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip.data()), port);

    // Set the socket to non-blocking mode
    socket.non_blocking(true);

    // attempt to connect to the given ip asynchronously
    boost::system::error_code ec;
    socket.async_connect(endpoint, [&ec](const boost::system::error_code& result_ec) {
        ec = result_ec;
    });

    // we don't want to attempt to connect forever doe
    // timeout after `timeout_ms` ms
    boost::asio::deadline_timer timer(io_service);
    timer.expires_from_now(boost::posix_time::milliseconds(timeout_ms));
    timer.async_wait([&socket](const boost::system::error_code& timer_ec) {
        if (timer_ec != boost::asio::error::operation_aborted) {
            socket.close();
        }
    });

    // execute everything we just set up
    io_service.run();

    if (!ec) {
        return true; // no error, we are connected successfully
    } else {
        return false; // something, anything, went wrong.
    }
}

// turns ip string into 4 integer octets
ip_parts ip_str_to_parts(const std::string_view ip)
{
    ip_parts parts{}; // 0.0.0.0

    // get ip address bytes
    boost::asio::ip::address ip_address = boost::asio::ip::make_address(ip.data());
    boost::asio::ip::address_v4::bytes_type bytes = ip_address.to_v4().to_bytes();

    // emplace each byte DIRECTLY into the corresponding memory space inside the `parts` struct
    for (int i = 0; i < 4; i++)
        *(((int*)&parts) + i) = bytes[i] & 0xFF;

    return parts;
}

// returns the neighbor `neighbor_offset` steps away from the given ip `addr` at node `node`
ip_parts ip_neighbor(ip_parts addr, const int neighbor_offset, const int node)
{
    addr.third = addr.third + neighbor_offset & 0xff;
    addr.fourth = node;
    return addr;
}

// Given a unique set of ip address strings without their ports, we summon some threads to check which ips in the set are ONLINE
// The default port checked is minecraft's port 25565.
std::vector<minecraft_server> ip_crawl_horizontal(const std::unordered_set<std::string>& ips, const std::vector<unsigned short>& ports)
{
    // prepare our jehovas witness force (thread pool)
    // you can adjust the threads higher than 100 if you want. just dont blow up your computer.,..,
    boost::asio::thread_pool pool(200);

    std::mutex print_mutex;

    std::mutex valid_servers_mutex;
    std::vector<minecraft_server> valid_servers;

    std::mutex scanned_servers_count_mutex;
    size_t scanned_servers_count = 0;

    // start scanning
    fmt::print("Mounting crawlers ... \n");

    // For every port in the given ports list
    // Throw our crawler lambda into the thread pool and execute
    for (const auto& ip : ips)
    {
        const auto crawler_func =
                [&, Func = ip_tcp_minecraft_server_ping]
                        (unsigned short port)
                {
                    const auto response = Func(ip, port);

                    // add to valid servers list if server is online and not in ips_visited cache
                    if (response.online)
                    {

                        std::lock_guard<std::mutex> lock(valid_servers_mutex);
                        valid_servers.emplace_back(ip, port, response);
                    }

                    // atomically increment scnaned server count
                    {
                        std::lock_guard<std::mutex> lock(scanned_servers_count_mutex);
                        ++scanned_servers_count;
                    }

                    // atomically print
                    {
                        std::lock_guard<std::mutex> lock(print_mutex);
                        fmt::print("Scanning ({} / {})                       \r", scanned_servers_count, ips.size() * ports.size());
                    }

                };

        //crawler_func(); // uncomment to debug on single thread
        for (const unsigned short& port : ports)
            boost::asio::post(pool, [crawler_func, port] { return crawler_func(port); });
    }

    // wait for all of our threads to finish checking ips
    pool.join();

    return valid_servers;
}

// simulate a minecraft client server list ping.
// retrieves information about the server and returns the info struct
minecraft_server_tcp_info ip_tcp_minecraft_server_ping(const std::string_view ip, const unsigned short port)
{
    using boost::asio::ip::tcp;
    boost::asio::io_context io_context;
    tcp::socket socket(io_context);
    static constexpr minecraft_server_tcp_info OFFLINE =
    {
            .online = false,
            .player_count = 0,
            .max_players = 0,
            .motd = "",
    };

    try
    {
        // resolve TCP endpoint
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(ip.data(), std::to_string(port));

        // check if server can even accept connection
        if (!ip_tcp_is_open(ip, port, 500))
        {
            return OFFLINE;
        }

        // connect to server and request/simulate minecraft client server list ping
        boost::asio::connect(socket, endpoints);
        boost::asio::write(socket, boost::asio::buffer(std::string("\xFE", 1)));

        // set up buffer read timeout deadline timer
        boost::asio::deadline_timer timer(io_context);
        timer.expires_from_now(boost::posix_time::milliseconds(500));
        timer.async_wait([&](const boost::system::error_code& error) {
            if (!error) {
                // Timeout occurred, cancel the read operation
                socket.cancel();
            }
        });


        // read response
        const auto buffer = [&]()
        {
            std::vector<uint8_t> b(256);
            boost::asio::async_read(socket, boost::asio::buffer(b),
                                    [&](const boost::system::error_code& error, size_t s) {
                                        timer.cancel(); // Cancel the timer since the read completed
                                        if (!error) {
                                            // Read completed successfully
                                        }
                                    });

            io_context.run();
            return b;
        }();


        // we no longer need our connection
        socket.close();


        if (buffer[0] != 0xFF)
        {
            return OFFLINE;
        }

        // server is online
        const std::string_view full_response(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        const std::string_view response_we_care_about = full_response.substr(3);

        // parse decoded string
        const auto response_parts = [&]() -> std::vector<std::string>
        {
            std::vector<std::string> parts;
            return boost::split(parts, response_we_care_about, boost::is_any_of("\u00A7"));
        }();


        const bool online = true;
        const std::string motd = boost::trim_copy(boost::regex_replace(response_parts[0], boost::regex("[^a-zA-Z0-9 \\.]"), ""));
        const size_t player_count = static_cast<size_t>(std::stoi(boost::regex_replace(response_parts[1], boost::regex("[^.0-9]"), "")));
        const size_t max_players = static_cast<size_t>(std::stoi(boost::regex_replace(response_parts[2], boost::regex("[^.0-9]"), "")));

        return { online, player_count, max_players, motd };
    }
    catch (const std::runtime_error& re)
    {
        socket.close();
        fmt::print("[{}] Severe Error: {}\n", __FUNCTION__, re.what());
        return OFFLINE;
    }
    catch (const std::exception& e)
    {
        socket.close();
        //fmt::print("[{}] Error: {}\n", __FUNCTION__, e.what());
        return OFFLINE;
    }
}

// Given a starting ip `origin_ip` and a list of offsets, generates a set of unique IP addresses where each IP address
// neighbors the `origin_ip` at some offset within the given list of offsets.
std::unordered_set<std::string> ip_neighbor_set(const ip_parts& origin_ip, const std::vector<int>& offsets)
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

