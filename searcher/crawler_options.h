//
// Created by shufy on 9/29/23.
//

#ifndef METHOD9_CRAWLER_OPTIONS_H
#define METHOD9_CRAWLER_OPTIONS_H
#include <string>
#include <vector>
#include <optional>
#include <ostream>
#include <boost/regex.hpp>

static const boost::regex ip_regex(R"(^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)\.?\b){4}$)");

struct crawler_options
{
    bool verbose_results;
    bool show_options;
    size_t threads;
    size_t timeout_read_ms;
    size_t timeout_connect_ms;
    size_t neighbor_distance;
    size_t neighbor_offset;
    std::string origin_ip;
    std::string crawl_direction;
    std::vector<unsigned short> ports;

    friend std::ostream& operator<<(std::ostream& stream, const crawler_options& options)
    {
        stream << "---------------------------\n";
        stream << "Crawler Options\n";
        stream << "---------------------------\n";
        stream << "\tIP: " << options.origin_ip << '\n';
        stream << "\tIP Offset: " << options.neighbor_offset << '\n';
        stream << "\tNeighbor Blocks: " << options.neighbor_distance << '\n';
        stream << "\tCrawl Mode: " << options.crawl_direction << '\n';
        stream << "\tTimeout: " << options.timeout_connect_ms << " ms (connect), " << options.timeout_read_ms << " ms (read)\n";
        stream << "\tPorts to Scan: ";
        for (size_t i = 0; i < options.ports.size(); ++i)
        {
            stream << options.ports[i];
            if (i < options.ports.size() - 1)
                stream << ',';
        }
        stream << '\n';
        stream << "\tActive Threads: " << options.threads << '\n';
        stream << "---------------------------\n\n";
        return stream;
    }
};

std::optional<crawler_options>  parse_crawler_options_from_argv(int argc, char* argv[]);

#endif //METHOD9_CRAWLER_OPTIONS_H
