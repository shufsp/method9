//
// Created by shufy on 9/24/23.
//
#include "crawler_network.h"
#include <boost/asio.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <fmt/core.h>

// Most important method for the crawler. Checks if the given IP's port is open to TCP connections
bool ip_tcp_is_open(const char* ip, unsigned short port, int timeout_ms)
{
    // set up our socket
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket(io_service);
    socket.open(boost::asio::ip::tcp::v4());
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);

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
ip_parts ip_str_to_parts(const char* ip)
{
    ip_parts parts{}; // 0.0.0.0

    // get ip address bytes
    boost::asio::ip::address ip_address = boost::asio::ip::make_address(ip);
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