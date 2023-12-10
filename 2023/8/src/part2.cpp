#include "main.h"


struct ghost_t
{
    string cur_map;
    int64_t reach_count = 0;
    int64_t count = 0;
    bool reached = false;
};


int64_t calculate_part2(puzzle_t* puzzle)
{
    const auto& data = puzzle->data;
    int64_t result = 0;

    // Parse
    string lrs = data[0];
    vector<ghost_t> ghosts;
    int ghost_count = 0;
    map<string, pair<string, string>> maps;
    for (int i = 2; i < (int)data.size(); ++i)
    {
        const auto& line = data[i];
        auto splits = onut::splitString(line, "= ,()");
        maps[splits[0]] = { splits[1], splits[2] };
        if (splits[0][2] == 'A') ghosts.push_back({ splits[0], 0, 0, false });
    }

    // Execute
    size_t n = 0;
    bool all_reached = true;
    do
    {
        char c = lrs[n++];
        if (n == lrs.size()) n = 0;

        all_reached = true;
        for (auto& ghost : ghosts)
        {
            if (ghost.reached) continue;
            if (c == 'L')
                ghost.cur_map = maps[ghost.cur_map].first;
            else
                ghost.cur_map = maps[ghost.cur_map].second;
            ghost.reach_count++;
            if (ghost.cur_map[2] == 'Z')
            {
                ghost.reached = true;
                ghost.count = ghost.reach_count;
                continue;
            }
            all_reached = false;
        }
    } while (!all_reached);

    // Find the biggest increment
    int64_t biggest_count = 0;
    for (int64_t i = 0; i < (int64_t)ghosts.size(); ++i)
    {
        if (ghosts[i].reach_count > biggest_count)
        {
            biggest_count = ghosts[i].reach_count;
        }
    }

    // Increments until they are all equal
    while (true)
    {
        bool all_same = true;
        for (int i = 1; i < (int)ghosts.size(); ++i)
        {
            if (ghosts[i].count != ghosts[i - 1].count)
            {
                all_same = false;
                break;
            }
        }

        if (all_same)
        {
            // We're done
            break;
        }

        // Increment
        result += biggest_count;

        for (auto& ghost : ghosts)
        {
            while (ghost.count < result)
            {
                ghost.count += ghost.reach_count;
            }
        }
    }

    return result;
}
