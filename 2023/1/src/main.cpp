#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>
#include <onut/Texture.h>

void initSettings()
{
    oSettings->setGameName("Advent of Code 2023 - 1");
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


int calculate1(const vector<string>& data)
{
    int result = 0;

    for (const string& line : data)
    {
        int first = 0;
        int last = 0;

        for (const char c : line)
        {
            if (c >= '0' && c <= '9')
            {
                first = c - '0';
                break;
            }
        }

        for (int i = (int)line.size() - 1; i >= 0; --i)
        {
            const char c = line[i];
            if (c >= '0' && c <= '9')
            {
                last = c - '0';
                break;
            }
        }

        first *= 10;
        result += first + last;
    }

    return result;
}


int calculate2(const vector<string>& data)
{
    int result = 0;

    const string digit_names[] = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };

    for (const string& line : data)
    {
        int first = 0;
        int last = 0;

        {
            int index = (int)line.size();
            for (int i = 0; i < (int)line.size(); ++i)
            {
                const char c = line[i];
                if (c >= '0' && c <= '9')
                {
                    first = c - '0';
                    index = i;
                    break;
                }
            }

            for (int i = 0; i < 10; ++i)
            {
                size_t pos = line.find(digit_names[i]);
                if (pos == string::npos) continue;
                if ((int)pos < index)
                {
                    first = i;
                    index = (int)pos;
                    continue;
                }
            }
        }

        {
            int index = 0;
            for (int i = (int)line.size() - 1; i >= 0; --i)
            {
                const char c = line[i];
                if (c >= '0' && c <= '9')
                {
                    last = c - '0';
                    index = i;
                    break;
                }
            }

            for (int i = 0; i < 10; ++i)
            {
                size_t pos = line.rfind(digit_names[i]);
                if (pos == string::npos) continue;
                if ((int)pos > index)
                {
                    last = i;
                    index = (int)pos;
                    continue;
                }
            }
        }

        first *= 10;
        result += first + last;
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
