#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>

void initSettings()
{
    oSettings->setGameName("Advent of Code 2020 - 9");
    oSettings->setIsResizableWindow(true);
    oSettings->setResolution({1280, 720});
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
    oRenderer->clear();
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

int processing_count = 0;
int64_t result1 = -1;
int64_t result2 = -1;

vector<string> getDataLines()
{
    // Load content
    auto filename = oContentManager->findResourceFile("puzzle_data.txt");
    auto raw_data = onut::getFileData(filename);
    string data_string;
    data_string.resize(raw_data.size());
    memcpy(data_string.data(), raw_data.data(), raw_data.size());

    onut::replace(data_string, "\r", "");

    // Split lines
    return move(onut::splitString(data_string, '\n', false));
}

vector<int64_t> port_data;
#define PREAMBLE_COUNT 25
vector<int64_t> contiguous_set;

void parseData()
{
    auto lines = getDataLines();

    for (const auto& line : lines)
        port_data.push_back(stoll(line));
}

bool isValid(uint64_t number, int index)
{
    for (int i = index - PREAMBLE_COUNT; i < index - 1; ++i)
        for (int j = i + 1; j < index; ++j)
            if (port_data[i] + port_data[j] == number)
                return true;

    return false;
}

int64_t findInvalidNumber(int* at_index = nullptr)
{
    for (int i = PREAMBLE_COUNT, len = (int)port_data.size(); i < len; ++i)
    {
        if (!isValid(port_data[i], i))
        {
            if (at_index)
                *at_index = i;
            return port_data[i];
        }
    }

    return -1;
}

int64_t calculate1()
{
    return findInvalidNumber();
}

int64_t calculate2()
{
    int invalid_index = -1;
    auto invalid_number = findInvalidNumber(&invalid_index);

    for (int i = 0; i < invalid_index - 1; ++i)
    {
        int64_t sum = 0;
        int64_t smallest = port_data[i];
        int64_t biggest = smallest;
        for (int j = i; j < invalid_index; ++j)
        {
            sum += port_data[j];
            smallest = onut::min(smallest, port_data[j]);
            biggest = onut::max(biggest, port_data[j]);

            if (sum > invalid_number)
                break;

            if (sum == invalid_number)
                return smallest + biggest;
        }
    }

    return -1;
}

void init()
{
    parseData();

    processing_count = 2;
    
    thread thread1([]()
    {
        auto result = calculate1();
        OSync([=]()
        {
            result1 = result;
            processing_count--;
        });
    });
    thread1.detach();

    thread thread2([]()
    {
        auto result = calculate2();
        OSync([=]()
        {
            result2 = result;
            processing_count--;
        });
    });
    thread2.detach();
}

void renderUI()
{
    ImGui::Begin("Result");

    if (processing_count > 0)
    {
        const char spinner[] = {'/', '-', '\\', '|'};
        static int spinner_anim = 0;
        spinner_anim++;
        ImGui::LabelText("Processing", "%c", spinner[(spinner_anim / 4) % 4]);
    }

    {
        auto text = to_string(result1);
        ImGui::InputText("Result 1", (char*)text.c_str(), text.size() + 1);
    }
    {
        auto text = to_string(result2);
        ImGui::InputText("Result 2", (char*)text.c_str(), text.size() + 1);
    }

    ImGui::End();
}
