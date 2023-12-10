#include "main.h"


struct card_t
{
    int win_count = 0;
};


static vector<card_t> parse_cards(const vector<string>& data)
{
    vector<card_t> cards;

    for (const auto& line : data)
    {
        card_t card;

        auto splits = onut::splitString(onut::splitString(line, ':')[1], '|');
        vector<int64_t> winning_numbers = parse_numbers(splits[0]);
        vector<int64_t> my_numbers = parse_numbers(splits[1]);

        for (auto number : my_numbers)
        {
            if (contains(winning_numbers, number))
                card.win_count++;
        }

        cards.push_back(card);
    }

    return cards;
}


int64_t calculate_part1(puzzle_t* puzzle)
{
    const auto& data = puzzle->data;
    int64_t result = 0;

    vector<card_t> cards = parse_cards(data);
    for (const auto& card : cards)
    {
        auto points = card.win_count ? (1 << (card.win_count - 1)) : 0;
        result += points;
    }

    return result;
}


static int64_t get_instance_count(int index, card_t& card, vector<card_t>& cards)
{
    int64_t ret = 1; // Include self

    for (int i = index + 1; i < index + 1 + card.win_count && i < (int)cards.size(); ++i)
    {
        ret += get_instance_count(i, cards[i], cards);
    }

    return ret;
}


int64_t calculate_part2(puzzle_t* puzzle)
{
    const auto& data = puzzle->data;
    int64_t result = 0;

    vector<card_t> cards = parse_cards(data);
    for (int i = 0; i < (int)cards.size(); ++i)
    {
        result += get_instance_count(i, cards[i], cards);
    }

    return result;
}
