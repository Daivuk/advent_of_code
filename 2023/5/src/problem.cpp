#include "main.h"


struct range_t
{
    int64_t src_start = 0;
    int64_t count = 0;
    int64_t dst_start = 0;
};


struct map_t
{
    shared_ptr<map_t> dst;
    vector<range_t> ranges;
};


static int64_t get_dst_id(int64_t id, const shared_ptr<map_t>& map)
{
    for (const auto& range : map->ranges)
    {
        if (id >= range.src_start && id < range.src_start + range.count)
        {
            auto i = id - range.src_start;
            return range.dst_start + i;
        }
    }

    return id;
}


static int64_t get_location(int64_t id, const shared_ptr<map_t>& map)
{
    auto dst_id = get_dst_id(id, map);
    if (map->dst)
    {
        return get_location(dst_id, map->dst);
    }
    return dst_id;
}


static shared_ptr<map_t> parse_maps(const vector<string>& data)
{
    shared_ptr<map_t> root;
    shared_ptr<map_t> parent;

    for (int i = 2; i < (int)data.size(); ++i)
    {
        const auto& line = data[i];

        auto splits = onut::splitString(line, ' ');
        splits = onut::splitString(splits[0], '-');

        if (parent)
        {
            auto new_map = make_shared<map_t>();
            parent->dst = new_map;
            parent = new_map;
        }
        else
        {
            root = make_shared<map_t>();
            parent = root;
        }

        ++i;
        for (; i < (int)data.size(); ++i)
        {
            const auto& line = data[i];
            if (line.empty()) break;

            range_t range;

            auto range_vals = parse_numbers(line);
            range.dst_start = range_vals[0];
            range.src_start = range_vals[1];
            range.count = range_vals[2];

            parent->ranges.push_back(range);
        }
    }

    return root;
}


int64_t calculate_part1(const vector<string>& data)
{
    int64_t result = 0;

    vector<int64_t> seeds = parse_numbers(onut::splitString(data[0], ':')[1]);
    auto root = parse_maps(data);

    int64_t lowest = numeric_limits<int64_t>::max();
    for (auto seed : seeds)
    {
        auto loc_id = get_location(seed, root);
        lowest = min(loc_id, lowest);
    }
    result = lowest;

    return result;
}


#if 0 // Lets brute force it for now. takes about a minute and half
int64_t calculate_part2(const vector<string>& data)
{
    int64_t result = 0;

    vector<int64_t> seed_ranges = parse_numbers(onut::splitString(data[0], ':')[1]);
    auto root = parse_maps(data);

    int64_t seed_count = 0;
    for (int i = 0; i < (int)seed_ranges.size(); i += 2)
    {
        seed_count += seed_ranges[i + 1];
    }

    vector<future<int64_t>> workers;
    atomic<int64_t> computed = 0;
    for (int i = 0; i < (int)seed_ranges.size(); i += 2)
    {
        workers.push_back(async(launch::async, [root, &computed, seed_count](int64_t start, int64_t count) -> int64_t
        {
            int64_t lowest = numeric_limits<int64_t>::max();
            for (int64_t seed = start; seed < start + count; ++seed)
            {
                auto loc_id = get_location(seed, root);
                lowest = min(loc_id, lowest);
                sub_progress2 = (float)((double)(++computed) / (double)seed_count);
            }
            return lowest;
        }, seed_ranges[i], seed_ranges[i + 1]));
    }

    int64_t lowest = numeric_limits<int64_t>::max();
    for (auto& worker : workers)
    {
        lowest = min(worker.get(), lowest);
    }
    result = lowest;

    return result;
}
#endif


#if 1
struct block_t
{
    int64_t start;
    int64_t size;
    int64_t dst_start;
};


void add_range(vector<block_t>& blocks, const range_t& range)
{
    for (int i = 0; i < (int)blocks.size(); ++i)
    {
        auto& block = blocks[i];
        if (range.src_start >= block.start &&
            range.src_start + range.count <= block.start + block.size)
        {
            block_t new_block;
            new_block.start = range.src_start;
            new_block.size = range.count;
            new_block.dst_start = range.dst_start;

            if (range.src_start == block.start)
            {
                // Insert before
                block.size -= range.count;
                block.start += range.count;
                block.dst_start += range.count;
                blocks.insert(blocks.begin() + i, new_block);
            }
            else
            {
                // Check if we need to insert a new block after our new one (Punching a hole in existing block)
                if (range.src_start + range.count < block.start + block.size)
                {
                    block_t split_block;
                    split_block.start = range.src_start + range.count;
                    split_block.size = (block.start + block.size) - (range.src_start + range.count);
                    split_block.dst_start = block.dst_start + (range.src_start - block.start) + range.count;
                    blocks.insert(blocks.begin() + i + 1, split_block);
                }

                // Insert after
                blocks[i].size = range.src_start - blocks[i].start;
                blocks.insert(blocks.begin() + i + 1, new_block);
            }

            return;
        }
    }
}


int64_t calculate_part2(const vector<string>& data)
{
    int64_t result = 0;

    vector<int64_t> seed_ranges = parse_numbers(onut::splitString(data[0], ':')[1]);
    auto root = parse_maps(data);

    // Flatten maps
    vector<shared_ptr<map_t>> maps;
    while (root)
    {
        maps.push_back(root);
        root = root->dst;
    }

    // Consider this like a memory mapping problem. We have memory blocks that maps to a destination
    vector<block_t> blocks;

    // Start with a first block that maps the whole range
    block_t first_block;
    first_block.start = 0;
    first_block.dst_start = 0;
    first_block.size = numeric_limits<int64_t>::max();
    blocks.push_back(first_block);

    // Split the "memory" with ranges
    for (int i = 0; i < (int)seed_ranges.size(); i += 2)
    {
        auto start_id = seed_ranges[i];
        auto count = seed_ranges[i + 1];
        add_range(blocks, {start_id, count, start_id});
    }

    return result;
}
#endif
