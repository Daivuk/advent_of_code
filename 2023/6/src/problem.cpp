#include "main.h"


struct race_t
{
    int64_t duration = 0;
    int64_t record_distance = 0;
};


vector<race_t> parse_races(const vector<string>& data)
{
    auto durations = parse_numbers(onut::splitString(data[0], ':')[1]);
    auto distances = parse_numbers(onut::splitString(data[1], ':')[1]);

    vector<race_t> ret;
    ret.resize(durations.size());

    for (int i = 0; i < (int)durations.size(); ++i)
    {
        ret[i].duration = durations[i];
        ret[i].record_distance = distances[i];
    }

    return ret;
}


vector<race_t> parse_races2(const vector<string>& data)
{
    auto durations = parse_numbers(onut::removeChars(onut::splitString(data[0], ':')[1], " "));
    auto distances = parse_numbers(onut::removeChars(onut::splitString(data[1], ':')[1], " "));

    vector<race_t> ret;
    ret.resize(durations.size());

    for (int i = 0; i < (int)durations.size(); ++i)
    {
        ret[i].duration = durations[i];
        ret[i].record_distance = distances[i];
    }

    return ret;
}


int64_t calculate_part1(const vector<string>& data)
{
    int64_t result = 1;

    auto races = parse_races(data);

    for (const auto& race : races)
    {
        int64_t speed = 0;
        int64_t record_count = 0;
        for (int64_t wait_time = 0; wait_time <= race.duration; ++wait_time)
        {
            int64_t time_remaining = race.duration - wait_time;
            int64_t distance = speed * time_remaining;
            if (distance > race.record_distance) ++record_count;
            speed++;
        }
        result *= record_count;
    }

    return result;
}


int64_t calculate_part2(const vector<string>& data)
{
    int64_t result = 1;

    auto races = parse_races2(data);

    for (const auto& race : races)
    {
        int64_t speed = 0;
        int64_t record_count = 0;
        for (int64_t wait_time = 0; wait_time <= race.duration; ++wait_time)
        {
            int64_t time_remaining = race.duration - wait_time;
            int64_t distance = speed * time_remaining;
            if (distance > race.record_distance)
            {
                ++record_count;
            }
            speed++;
        }
        result *= record_count;
    }

    return result;
}
