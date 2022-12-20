#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cento/centoRect.hpp"

#include <array>
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

    return 0;
}