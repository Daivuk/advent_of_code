#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>
#include <onut/Texture.h>

void initSettings()
{
    oSettings->setGameName("Advent of Code 2023 - 2");
    oSettings->setIsResizableWindow(true);
    oSettings->setResolution({1600, 900});
    oSettings->setShowFPS(true);
    oSettings->setShowOnScreenLog(true);
}

void shutdown()
{
}

void update()
{
}

void render()
{
    oRenderer->clear(Color::Black);
}

void postRender()
{
}


#include <chrono>
#include <future>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <onut/Async.h>
#include <onut/Files.h>
#include <onut/Dispatcher.h>
#include <onut/Point.h>
#include <onut/Crypto.h>


using namespace std;
using namespace chrono_literals;


vector<string> test_data1;
vector<string> test_data2;
vector<string> puzzle_data;

int test_result1 = -1;
int test_result2 = -1;
int result1 = -1;
int result2 = -1;


vector<string> load_data(const char* in_filename)
{
    // Load content
    auto filename = oContentManager->findResourceFile(in_filename);
    auto raw_data = onut::getFileData(filename);
    string data_string;
    data_string.resize(raw_data.size());
    memcpy(data_string.data(), raw_data.data(), raw_data.size());

    onut::replace(data_string, "\r", "");

    // Split lines
    return move(onut::splitString(data_string, '\n', false));
}


void parse_data()
{
    test_data1 = load_data("test_data1.txt");
    test_data2 = load_data("test_data2.txt");
    puzzle_data = load_data("puzzle_data.txt");
}


static const map<string, int> color_to_idx = { {"red", 0}, {"green", 1}, {"blue", 2} };


struct subset_t
{
    int cubes[3] = { 0, 0, 0 };
};


struct game_t
{
    vector<subset_t> subsets;
};


vector<game_t> parse_games(const vector<string>& data)
{
    vector<game_t> games;

    for (const string& line : data)
    {
        game_t game;
        auto subset_splits = onut::splitString(onut::splitString(line, ':')[1], ';');
        for (const auto& subset_split : subset_splits)
        {
            subset_t subset;
            auto cube_splits = onut::splitString(subset_split, ',');
            for (const auto& cube_split : cube_splits)
            {
                auto splits = onut::splitString(cube_split, ' ');
                subset.cubes[color_to_idx.at(splits[1])] = stoi(splits[0]);
            }
            game.subsets.push_back(subset);
        }
        games.push_back(game);
    }

    return games;
}


int calculate1(const vector<string>& data)
{
    int result = 0;
    int counts[3] = { 12, 13, 14 };
    auto games = parse_games(data);

    for (int i = 0; i < (int)games.size(); ++i)
    {
        const auto& game = games[i];
        bool is_possible = true;
        for (const auto& subset : game.subsets)
        {
            for (int j = 0; j < 3; ++j)
            {
                if (subset.cubes[j] > counts[j])
                {
                    is_possible = false;
                    break;
                }
            }
            if (!is_possible) break;
        }
        if (is_possible)
        {
            result += i + 1;
        }
    }

    return result;
}


int get_power(const game_t& game)
{
    int mins[3] = {0, 0, 0};

    for (const auto& subset : game.subsets)
        for (int j = 0; j < 3; ++j)
            mins[j] = onut::max(mins[j], subset.cubes[j]);

    return mins[0] * mins[1] * mins[2];
}


int calculate2(const vector<string>& data)
{
    int result = 0;
    auto games = parse_games(data);

    for (int i = 0; i < (int)games.size(); ++i)
    {
        const auto& game = games[i];
        int power = get_power(game);
        result += power;
    }

    return result;
}


void init()
{
    parse_data();

    test_result1 = calculate1(test_data1);
    result1 = calculate1(puzzle_data);

    test_result2 = calculate2(test_data2);
    result2 = calculate2(puzzle_data);
}


void renderUI()
{
    ImGui::Begin("Result");

    if (ImGui::CollapsingHeader("Problem 1", ImGuiTreeNodeFlags_DefaultOpen))
    {
        {
            auto text = to_string(test_result1);
            ImGui::InputText("Test result##test1", (char*)text.c_str(), text.size() + 1);
        }
        {
            auto text = to_string(result1);
            ImGui::InputText("Result##ret1", (char*)text.c_str(), text.size() + 1);
        }
    }

    if (ImGui::CollapsingHeader("Problem 2", ImGuiTreeNodeFlags_DefaultOpen))
    {
        {
            auto text = to_string(test_result2);
            ImGui::InputText("Test result##test2", (char*)text.c_str(), text.size() + 1);
        }
        {
            auto text = to_string(result2);
            ImGui::InputText("Result##ret2", (char*)text.c_str(), text.size() + 1);
        }
    }

    ImGui::End();
}
