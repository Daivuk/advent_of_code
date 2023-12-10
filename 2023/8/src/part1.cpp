#include "main.h"


int64_t calculate_part1(puzzle_t* puzzle)
{
    const auto& data = puzzle->data;
    int64_t result = 0;

    // Parse
    string lrs = data[0];
    map<string, pair<string, string>> maps;
    for (int i = 2; i < (int)data.size(); ++i)
    {
        const auto& line = data[i];
        auto splits = onut::splitString(line, "= ,()");
        maps[splits[0]] = { splits[1], splits[2] };
    }

    // Execute
    size_t n = 0;
    string cur_map = "AAA";
    while (cur_map != "ZZZ")
    {
        char c = lrs[n++];
        if (n == lrs.size()) n = 0;

        if (c == 'L')
            cur_map = maps[cur_map].first;
        else
            cur_map = maps[cur_map].second;

        ++result;
    }

    return result;
}
