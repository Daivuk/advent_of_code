#include "main.h"


struct card_t
{
    int win_count = 0;
};


static vector<int> parse_numbers(const string& str)
{
    auto splits = onut::splitString(str, ' ');
    vector<int> ret;
    for (const auto& split : splits)
    {
        ret.push_back(stoi(split));
    }
    return ret;
}


static vector<card_t> parse_cards(const vector<string>& data)
{
    vector<card_t> cards;

    for (const auto& line : data)
    {
        card_t card;

        auto splits = onut::splitString(onut::splitString(line, ':')[1], '|');
        vector<int> winning_numbers = parse_numbers(splits[0]);
        vector<int> my_numbers = parse_numbers(splits[1]);

        for (auto number : my_numbers)
        {
            if (contains(winning_numbers, number))
                card.win_count++;
        }

        cards.push_back(card);
    }

    return cards;
}


int calculate_part1(const vector<string>& data)
{
    int result = 0;

    vector<card_t> cards = parse_cards(data);
    for (const auto& card : cards)
    {
        auto points = card.win_count ? (1 << (card.win_count - 1)) : 0;
        result += points;
    }

    return result;
}


static int get_instance_count(int index, card_t& card, vector<card_t>& cards)
{
    int ret = 1; // Include self

    for (int i = index + 1; i < index + 1 + card.win_count && i < (int)cards.size(); ++i)
    {
        ret += get_instance_count(i, cards[i], cards);
    }

    return ret;
}


int calculate_part2(const vector<string>& data)
{
    int result = 0;

    vector<card_t> cards = parse_cards(data);
    for (int i = 0; i < (int)cards.size(); ++i)
    {
        result += get_instance_count(i, cards[i], cards);
    }

    return result;
}
