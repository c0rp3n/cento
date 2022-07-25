//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//

#ifndef centoZTile_hpp
#define centoZTile_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoRect.hpp"

#include <compare>
#include <typeinfo>

CENTO_BEGIN_NAMESPACE

enum struct MemberIndex : u32 { Space = -1 };
enum struct TileIndex   : u32 { None  = -1 };

#pragma push pack(1)

enum struct CompressedSize : u16
{
    InvInfinity = -1,
    Infinity    = -2,
    MaxSize     = -3
};

/*
 * A Compressed Rectangle is designed to reduce the amount of space required to
 * represent tiles in a tiling by compressing the upper right coordinate into
 * just a single word.
 *
 * It is also able to store the boundary tiles (minx,miny),(maxy,maxy) in a
 * single tile by having a concept of inverted infinity.
 *
 * The maximum width or height of a none standard tile is MaxSize (65533) after
 * this a tile must be inserted as two or more adjacent solid tiles.
 *
 * When the compressed size is Infinity the upper right coord is maxx or maxy.
 *
 * When the compressed size is a Inverted Infinity the point stored in the rect
 * is actually the upper right coord rather than the lower left.
 *
 * Given the below tiling:
 *
 * +------------------------------------------------------------------------+
 * |                                                            (maxx,maxy) |
 * |                                                                        |
 * | (minx,1024)                                                            |
 * 5---------------+----------------------------------------+---------------+
 * |  (-1024,1024) |                            (1024,1024) |   (maxx,1024) |
 * |               |                                        |               |
 * |               |                                        |               |
 * |               |                                        |               |
 * |               |                                        |               |
 * |               |                                        |               |
 * | (minx,-1024)  | (-1024,-1024)                          | (1024,-1024)  |
 * 2---------------3----------------------------------------4---------------+
 * |                                                           (maxx,-1024) |
 * |                                                                        |
 * | (minx,miny)                                                            |
 * 1------------------------------------------------------------------------+
 *
 * The rects would look like:
 *
 * 1. {.ll = {.x =  minx, .y =  miny}, .ur = {.x =  maxx, .y = -1024}}
 * 2. {.ll = {.x =  minx, .y = -1024}, .ur = {.x = -1024, .y =  1024}}
 * 3. {.ll = {.x = -1024, .y = -1024}, .ur = {.x =  1024, .y =  1024}}
 * 4. {.ll = {.x =  1024, .y = -1024}, .ur = {.x =  maxx, .y =  1024}}
 * 5. {.ll = {.x =  minx, .y =  1024}, .ur = {.x =  maxx, .y =  maxy}}
 *
 * The compressed rects would look like:
 *
 * 1. {.point = {.x =  minx, .y = -1024}, .w =    Infinity, .h = InvInfinity}
 * 2. {.point = {.x = -1024, .y = -1024}, .w = InvInfinity, .h =        2048}
 * 3. {.point = {.x = -1024, .y = -1024}, .w =        2048, .h =        2048}
 * 4. {.point = {.x =  1024, .y = -1024}, .w =    Infinity, .h =        2048}
 * 5. {.point = {.x =  minx, .y =  1024}, .w =    Infinity, .h =    Infinity}
 *
 * This demonstrates how the compressed rectangle can still keep the tile count
 * to a minimum, there are unlikely to be large gaps in the middle of the
 * tiling, so this optimisation comes in useful around the edges.
 */
struct CompressedRect
{
    Point          point;
    CompressedSize w;
    CompressedSize h;

    CENTO_FORCEINLINE friend Point lowerLeft(const CompressedRect& r)
    {
        return { .x = r.w == CompressedSize::InvInfinity ? r.point.x : minx,
                 .y = r.h == CompressedSize::InvInfinity ? r.point.y : miny };
    }

    CENTO_FORCEINLINE friend Point upperRight(const CompressedRect& r)
    {
        const auto f = [](i32 coord, CompressedSize size, i32 min, i32 max)
        {
            if (size == CompressedSize::InvInfinity) { return coord; }

            const u16 s = static_cast<u16>(size);
            return (size == CompressedSize::Infinity) ? max : (coord + s);
        };

        return { .x = f(r.point.x, r.w, minx, maxx),
                 .y = f(r.point.y, r.h, miny, maxy) };
    }
};
static_assert(std::is_trivial_v<CompressedRect>);
static_assert(sizeof(CompressedRect) == 12);

/*
 * This is a design for a highly compressed tile which stores both the lower
 * left and upper right of the tile in the struct for better lookup, but also
 * fits into 4 words, the standard tile currently consumes 7 words.
 *
 * A tile is the basic unit used for representing both space and
 * solid area in a plane.  It has the following structure:
 *
 *                                    above
 *                                      ^
 *                                      |
 *              +-----------------------+ ---> right
 *              |         (upper right) |
 *              |                       |
 *              |                       |
 *              | (lower left)          |
 *   right <--- +-----------------------+
 *              |
 *              v
 *            below
 *
 * The (x, y) coordinates of the lower left corner and the width and height of
 * the tile are stored, along with four "corner stitches": RT, TR, BL, LB.
 *
 * To reduce the size of the corner stiches from 1 word each convert them to
 * indexs and store all 4 stiches in 2 words.  This limits us to 4,294,967,295
 * tiles but this is likely plenty for most use cases.
 *
 * The membership is also represnted as a index again this allows for plenty of
 * objects to be inserted, there is also going to be more tiles in the structure
 * than members.
 *
 * Space tiles are distinguished by a MemberIndex equal to Space.
 *
 * When a rectangle is inserted that has a width or height > max(u16) - 1 the
 * rect shall be split into two sections and then inserted. To prevent space
 * tiles which touch the bounds being split this way a width or height of
 * max(u16) will represent infinity.
 */
struct ZTile
{
    CompressedRect rect;
    MemberIndex    id;
    TileIndex      below;
    TileIndex      left;
    TileIndex      above;
    TileIndex      right;

    friend std::strong_ordering operator<=>(const ZTile& lhs, const ZTile& rhs) = default;
};
static_assert(std::is_trivial_v<ZTile>);
static_assert(sizeof(ZTile) == 32);

#pragma pop pack

CENTO_END_NAMESPACE

#endif // centoZTile_hpp