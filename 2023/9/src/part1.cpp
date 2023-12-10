#include "main.h"


static bool check_if_all_zeros(const vector<int64_t>& numbers)
{
    for (auto number : numbers)
        if (number)
            return false;
    return true;
}


int64_t calculate_part1(puzzle_t* puzzle)
{
    const auto& data = puzzle->data;
    int64_t result = 0;

    for (const auto& line : data)
    {
        // Build pyramid
        vector<vector<int64_t>> pyramid;
        pyramid.push_back(parse_numbers(line));
        int64_t cur_row = 0;
        while (!check_if_all_zeros(pyramid[cur_row]))
        {
            const auto& prev_row = pyramid[cur_row++];
            vector<int64_t> numbers;
            for (int i = 1; i < (int)prev_row.size(); ++i)
            {
                numbers.push_back(prev_row[i] - prev_row[i - 1]);
            }
            pyramid.push_back(numbers);
        }

        // Extrapolate
        pyramid.back().push_back(0);
        for (int j = (int)pyramid.size() - 2; j >= 0; --j)
        {
            auto& row = pyramid[j];
            auto& next_row = pyramid[j + 1];
            row.push_back(row.back() + next_row.back());
        }

        result += pyramid[0].back();
    }

    return result;
}
