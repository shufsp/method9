//
// Created by shufy on 9/24/23.
//

#ifndef SERVER_CRAWL_NBT_CREATE_H
#define SERVER_CRAWL_NBT_CREATE_H
#include <string>
#include <vector>

enum minecraft_version
{
    v1_20,
};

struct server
{
    std::string name;
    std::string base64_icon;
    std::string ip;
    int8_t hide_address; // 1 or 0
    int8_t accept_textures; // 1 or 0
};


#endif //SERVER_CRAWL_NBT_CREATE_H
