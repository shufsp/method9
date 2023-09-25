//
// Created by shufy on 9/24/23.
//

#ifndef SERVER_CRAWL_NBT_ADAPT_H
#define SERVER_CRAWL_NBT_ADAPT_H
#include <unordered_map>
#include <functional>
#include <vector>
#include "NBTWriter.h"
#include "nbt_create.h"

inline void adapt_nbt_to_v1_20(NBT::NBTWriter& writer, const server& srv)
{
    // Minecraft 1.20
    writer.writeCompound("");
    writer.writeString("name", srv.name.data());
    writer.writeString("icon", srv.base64_icon.data());
    writer.writeString("ip", srv.ip.data());
    writer.writeByte("hidden", srv.hide_address);
    writer.endCompound();
}

void adapt_nbt_to_version(const minecraft_version version, NBT::NBTWriter& writer, const std::vector<server>& servers);

#endif //SERVER_CRAWL_NBT_ADAPT_H
