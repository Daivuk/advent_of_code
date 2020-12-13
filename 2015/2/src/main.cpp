#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>

void initSettings()
{
    oSettings->setGameName("Advent of Code 2015 - 2");
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

using namespace std;
using namespace chrono_literals;

int processing_count = 0;
int result1 = -1;
int result2 = -1;

struct Dim
{
    int l, w, h;
};
vector<Dim> dims;

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

void parseData()
{
    auto lines = getDataLines();

    for (const auto& line : lines)
    {
        auto split = onut::splitString(line, 'x');

        Dim dim;

        dim.l = stoi(split[0]);
        dim.w = stoi(split[1]);
        dim.h = stoi(split[2]);

        dims.push_back(dim);
    }
}

int surfaceArea(const Dim& dim)
{
    return (2 * dim.l * dim.w) + 
           (2 * dim.w * dim.h) + 
           (2 * dim.h * dim.l) + 
           onut::min(dim.l * dim.w, 
                     dim.w * dim.h, 
                     dim.h * dim.l);
}

int volume(const Dim& dim)
{
    return dim.l * dim.w * dim.h;
}

int ribbon(const Dim& dim)
{
    int sides[] = {dim.l, dim.w, dim.h};
    sort(sides, sides + 3);
    return sides[0] * 2 + sides[1] * 2 + volume(dim);
}

int calculate1()
{
    int total = 0;

    for (const auto& dim : dims)
        total += surfaceArea(dim);

    return total;
}

int calculate2()
{
    int total = 0;

    for (const auto& dim : dims)
        total += ribbon(dim);

    return total;
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

    ImGui::InputInt("Result 1", &result1);
    ImGui::InputInt("Result 2", &result2);

    ImGui::End();
}
