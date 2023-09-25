//
// Created by shufy on 9/24/23.
//

#include "nbt_adapt.h"

// writes NBT server.dat using the corresponding version implementation
void adapt_nbt_to_version(const minecraft_version version, NBT::NBTWriter& writer, const std::vector<server>& servers)
{
    // TODO support more versions, if earlier version's server.dat files vary.
    const static std::unordered_map<minecraft_version, std::function<void(NBT::NBTWriter&, const server&)>> version_map = {
            { v1_20, adapt_nbt_to_v1_20 }
    };

    /*
     * server.dat structure
     * root
     * |_   servers container
     *      |_  server
     *          |_ name
     *          |_ ip
     *            ...
     */

    // the root node already exists. TODO at least I'm hoping it does in all versions of the server.dat....
    writer.writeListHead("servers", NBT::idCompound, servers.size()); // begin servers list

    for (const auto& srv : servers)
        version_map.at(version)(writer, srv); // use the implementation for the given minecraft version

    writer.endCompound(); // end the `servers` tag
}