#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cento/centoRect.hpp"
#include "cento/centoCreate.hpp"
#include "cento/centoInsert.hpp"
#include "cento/centoRemove.hpp"

#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace
{

    std::vector<cento::Rect> read_midi(const char* filename)
    {
        FILE* ifile = fopen(filename, "r");
        if (ifile == nullptr) { return {}; }

        std::array<char, 128> buffer;
        const char            del[] = " \t\r\n";

        std::vector<cento::Rect> rects;
        while (fgets(buffer.data(), int(buffer.size()), ifile) != nullptr)
        {
            char*  ptr    = nullptr;
            char*  token  = strtok_r(buffer.data(), del, &ptr);

            // skip when this line is a comment
            if (token[0] == '#') { continue; }

            // read coordinates (clockwise order)
            int coor[8] = {0};
            int i       = 0;
            for (i = 0; token != nullptr; (token = strtok_r(nullptr, del, &ptr)), (++i))
            {
                coor[i] = atoi(token);
            }

            // skip when the tile is not a rectangle
            if ( (coor[0] != coor[2]) || (coor[3] != coor[5]) || (coor[4] != coor[6]) || (coor[7] != coor[1]) )
            {
                fprintf(stderr, "Skipped invalid rectangle: (%d, %d) - (%d, %d) - (%d, %d) - (%d, %d)\n", coor[0], coor[1], coor[2], coor[3], coor[4], coor[5], coor[6], coor[7]);
                continue;
            }

            // collect tile
            rects.push_back({.ll = {coor[0], coor[1]}, .ur = {coor[4], coor[5]}});
        }

        fclose(ifile);

        return rects;
    }

    void print_tile(const char* const message, const cento::TilePlan& plan)
    {
        const cento::Rect& r = plan.rect;
        printf("%s %lu (%d, %d) - (%d, %d)\n",
               message,
               plan.id,
               r.ll.x,
               r.ll.y,
               r.ur.x,
               r.ur.y);
    }

    void print_tile(const char* const message, const cento::Tile* const t)
    {
        print_tile(message, cento::TilePlan{.id = t->id, .rect = getRect(t)});
    }

    bool validate_tiling(cento::Plane& plane)
    {
        i32 count = 0;
        cento::queryAll(plane, [&](cento::Tile* t)
        {
            if (not isSpace(t)) { return; }

            const bool left  = bottomLeft(t) ? isSolid(bottomLeft(t)) : true;
            const bool right = topRight(t) ? isSolid(topRight(t)) : true;
            if (left && right) { return; }

            print_tile("failed to merge", t);
            if (not left) { print_tile("with left", bottomLeft(t)); }
            if (not right) { print_tile("with right", topRight(t)); }

            ++count;
        });

        return count == 0;
    }

    using Snapshot = std::vector<cento::TilePlan>;

    Snapshot snapshot(cento::Plane& plane)
    {
        Snapshot tiles;
        cento::queryAll(plane, [&](const cento::Tile* t)
        {
            tiles.emplace_back(t->id, getRect(t));
        });

        return tiles;
    }

    cento::Rect bounds(const Snapshot& snapshot)
    {

        cento::Rect r{{cento::pInfinity, cento::pInfinity},
                      {cento::nInfinity, cento::nInfinity}};
        for (const cento::TilePlan& plan : snapshot)
        {
            if (plan.id == cento::Space) { continue; }

            r.ll.x = std::min(r.ll.x, plan.rect.ll.x);
            r.ll.y = std::min(r.ll.y, plan.rect.ll.y);
            r.ur.x = std::max(r.ur.x, plan.rect.ur.x);
            r.ur.y = std::max(r.ur.y, plan.rect.ur.y);
        }

        r.ll.x -= 100;
        r.ll.y -= 100;
        r.ur.x += 100;
        r.ur.y += 100;

        return r;
    }

    std::string snapshotToObj(const Snapshot& snapshot)
    {
        std::stringstream ss;

        const cento::Rect bound = bounds(snapshot);

        u64 i = 1;
        for (const cento::TilePlan& plan : snapshot)
        {
            const cento::Rect& r  = plan.rect;
            const cento::Point ll = {std::max(r.ll.x, bound.ll.x),
                                     std::max(r.ll.y, bound.ll.y)};
            const cento::Point ur = {std::min(r.ur.x, bound.ur.x),
                                     std::min(r.ur.y, bound.ur.y)};

            if (plan.id == cento::Space)
            {
                ss << "usemtl space\n";
            }
            else
            {
                ss << "usemtl solid\n";
            }

            ss << "v " << ll.x << ' ' << ll.y << '\n';
            ss << "v " << ll.x << ' ' << ur.y << '\n';
            ss << "v " << ur.x << ' ' << ur.y << '\n';
            ss << "v " << ur.x << ' ' << ll.y << '\n';
            ss << "f " << i << ' ' << i + 1 << ' ' << i + 2 << ' ' << i + 3 << '\n';

            i += 4;
        }

        return ss.str();
    }

}

int main(const int argc, const char* argv[])
{
    if (argc < 2) { return -1; }

    const std::vector<cento::Rect> rects = read_midi(argv[1]);

    printf("Rects (%zu):\n", rects.size());
    for (const cento::Rect& r : rects)
    {
        printf("(%d, %d) - (%d, %d)\n", r.ll.x, r.ll.y, r.ur.x, r.ur.y);
    }

    cento::Plane plane;
    cento::createUniverse(plane);

    u64 id = 0;
    std::vector<cento::Tile*> tiles;
    for (const cento::Rect& r : rects)
    {
        const Snapshot before = snapshot(plane);

        const cento::TilePlan plan{id++, r};
        cento::Tile* const    tile = cento::insertTile(plane, plan);
        if (tile == nullptr)
        {
            print_tile("failed to insert tile", plan);
            cento::query(plane, r, [](cento::Tile* t)
            {
                if (isSpace(t)) { return; }

                print_tile("overlaps with tile", t);
            });
            continue;
        }

        if (not validate_tiling(plane))
        {
            const Snapshot after = snapshot(plane);

            std::ofstream bFile{"before.obj"};
            std::ofstream aFile{"after.obj"};

            bFile << snapshotToObj(before);
            aFile << snapshotToObj(after);

            print_tile("failed to insert tile", plan);
            return -1;
        }

        tiles.push_back(tile);
    }

    for (cento::Tile* const tile : tiles)
    {
        print_tile("removing", tile);

        const Snapshot before = snapshot(plane);

        cento::removeTile(plane, tile);

        if (not validate_tiling(plane))
        {
            const Snapshot after = snapshot(plane);

            std::ofstream bFile{"before.obj"};
            std::ofstream aFile{"after.obj"};

            bFile << snapshotToObj(before);
            aFile << snapshotToObj(after);

            return -1;
        }
    }

    i32 count = 0;
    cento::queryAll(plane, [&](cento::Tile*){ ++count; });

    if (count != 1)
    {
        printf("failed to delete tiles\n");
        cento::queryAll(plane, [](cento::Tile* t)
        {
            print_tile("tile", t);
        });
        return -1;
    }

    return 0;
}