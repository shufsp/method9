//
// Created by shufy on 9/29/23.
//

#ifndef METHOD9_CRAWLER_OPTIONS_H
#define METHOD9_CRAWLER_OPTIONS_H
#include <string>
#include <vector>
#include <optional>
#include <boost/regex.hpp>

static const boost::regex ip_regex(R"(^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)\.?\b){4}$)");

struct crawler_options
{
    bool verbose_results;
    size_t threads;
    size_t timeout_read_ms;
    size_t timeout_connect_ms;
    size_t neighbor_distance;
    std::string origin_ip;
    std::string crawl_direction;
    std::vector<unsigned short> ports;
};

std::optional<crawler_options>  parse_crawler_options_from_argv(int argc, char* argv[]);

#endif //METHOD9_CRAWLER_OPTIONS_H
