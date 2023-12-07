#include "main.h"


const char FACE_VALUES[] = { 'J', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'Q', 'K', 'A' };


enum class type_t : int
{
    unknown = 0,
    high_card = 1,
    one_pair = 2,
    two_pair = 3,
    three_of_a_kind = 4,
    full_house = 5,
    four_of_a_kind = 6,
    five_of_a_kind = 7
};


struct hand_t
{
    int cards[5] = { -1, -1, -1, -1, -1 };
    int64_t bid = 0;
    type_t type = type_t::unknown;
};


struct frequency_t
{
    int value = 0;
    int count = 0;
};


static bool operator<(const hand_t& a, const hand_t& b)
{
    if (a.type < b.type) return true;
    if (b.type < a.type) return false;

    for (int i = 0; i < 5; ++i)
    {
        if (a.cards[i] < b.cards[i]) return true;
        if (b.cards[i] < a.cards[i]) return false;
    }

    assert(false); // They are equal, error!
    return true;
}


static int parse_face(char face)
{
    for (int i = 0; i < (int)sizeof(FACE_VALUES); ++i)
    {
        if (FACE_VALUES[i] == face) return i;
    }
    assert(false); // Error
    return -1;
}


static vector<frequency_t> get_frequencies(const hand_t& hand)
{
    vector<frequency_t> freqs;

    int wild_count = 0;

    for (int i = 0; i < 5; ++i)
    {
        auto value = hand.cards[i];
        bool new_freq = true;

        if (value == 0) // 'J'
        {
            ++wild_count;
            continue;
        }

        for (auto& freq : freqs)
        {
            if (freq.value == value)
            {
                new_freq = false;
                freq.count++;
                break;
            }
        }

        if (new_freq)
        {
            freqs.push_back({ value, 1 });
        }
    }

    // Order large to small groups
    std::sort(freqs.begin(), freqs.end(), [](const frequency_t& a, const frequency_t& b) -> bool
    {
        return a.count > b.count;
    });

    // Add wild card count
    if (wild_count == 5)
    {
        freqs.push_back({ 0, 5 });
    }
    else
    {
        freqs[0].count += wild_count;
    }

    return freqs;
}


static type_t get_hand_type(const hand_t& hand)
{
    auto freqs = get_frequencies(hand);

    // Five of a kind
    if (freqs[0].count == 5) return type_t::five_of_a_kind;

    // Four of a kind
    if (freqs[0].count == 4) return type_t::four_of_a_kind;

    // Full house
    if (freqs[0].count == 3 && freqs[1].count == 2) return type_t::full_house;

    // Three of a kind
    if (freqs[0].count == 3) return type_t::three_of_a_kind;

    // Two pair
    if (freqs[0].count == 2 && freqs[1].count == 2) return type_t::two_pair;

    // One pair
    if (freqs[0].count == 2) return type_t::one_pair;

    // High card
    return type_t::high_card;
}


static vector<hand_t> parse_hands(const vector<string>& data)
{
    vector<hand_t> hands;

    for (const auto& line : data)
    {
        auto splits = onut::splitString(line, ' ');

        hand_t hand;

        for (int i = 0; i < 5; ++i)
        {
            hand.cards[i] = parse_face(splits[0][i]);
        }
        hand.bid = stoll(splits[1]);
        hand.type = get_hand_type(hand);

        hands.push_back(hand);
    }

    return hands;
}


int64_t calculate_part2(const vector<string>& data)
{
    int64_t result = 0;

    auto hands = parse_hands(data);
    std::sort(hands.begin(), hands.end());

    for (int i = 0; i < (int)hands.size(); ++i)
    {
        result += (i + 1) * hands[i].bid;
    }

    return result;
}
