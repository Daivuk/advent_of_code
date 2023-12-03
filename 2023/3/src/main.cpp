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


bool is_symbol(char c)
{
    return c != '.' && !(c >= '0' && c <='9');
}


int calculate1(const vector<string>& data)
{
    int result = 0;

    for (int j = 0; j < (int)data.size(); ++j)
    {
        const auto& line = data[j];
        for (int i = 0; i < (int)line.size(); ++i)
        {
            auto c = line[i];
            if (c >= '1' && c <= '9')
            {
                int number = c - '0';
                int k = i;
                while (++k < (int)line.size())
                {
                    auto cc = line[k];
                    if (cc >= '0' && cc <= '9')
                    {
                        number *= 10;
                        number += cc - '0';
                        continue;
                    }
                    break;
                }
                --k;

                bool is_included = false;
                for (int y = j - 1; y <= j + 1; ++y)
                {
                    if (y < 0 || y >= (int)data.size()) continue;
                    for (int x = i - 1; x <= k + 1; ++x)
                    {
                        if (x < 0 || x >= (int)line.size()) continue;
                        if (is_symbol(data[y][x]))
                        {
                            is_included = true;
                            break;
                        }
                    }
                    if (is_included) break;
                }
                
                if (is_included)
                    result += number;

                i = k;
            }
        }
    }

    return result;
}


struct gear_t
{
    vector<int> parts;
};


bool is_gear(char c)
{
    return c == '*';
}


int get_gear_id(int x, int y)
{
    return y * 1000 + x;
}


int calculate2(const vector<string>& data)
{
    int result = 0;

    map<int, gear_t> gears;

    for (int j = 0; j < (int)data.size(); ++j)
    {
        const auto& line = data[j];
        for (int i = 0; i < (int)line.size(); ++i)
        {
            auto c = line[i];
            if (c >= '1' && c <= '9')
            {
                int number = c - '0';
                int k = i;
                while (++k < (int)line.size())
                {
                    auto cc = line[k];
                    if (cc >= '0' && cc <= '9')
                    {
                        number *= 10;
                        number += cc - '0';
                        continue;
                    }
                    break;
                }
                --k;

                for (int y = j - 1; y <= j + 1; ++y)
                {
                    if (y < 0 || y >= (int)data.size()) continue;
                    for (int x = i - 1; x <= k + 1; ++x)
                    {
                        if (x < 0 || x >= (int)line.size()) continue;
                        if (is_gear(data[y][x]))
                        {
                            auto& gear = gears[get_gear_id(x, y)];
                            gear.parts.push_back(number);
                        }
                    }
                }

                i = k;
            }
        }
    }

    for (const auto& kv : gears)
    {
        const auto& gear = kv.second;
        if (gear.parts.size() == 2)
        {
            auto ratio = gear.parts[0] * gear.parts[1];
            result += ratio;
        }
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
