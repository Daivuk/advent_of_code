#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>

void initSettings()
{
    oSettings->setGameName("Advent of Code 2015 - 3");
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
string line;

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
    line = lines[0];
}

bool operator<(const Point& a, const Point& b)
{
    return (a.x * 10000 + a.y) < (b.x * 10000 + b.y);
}

Point move(char dir)
{
    switch (dir)
    {
        case '^': return {0, -1};
        case 'v': return {0, 1};
        case '>': return {1, 0};
        case '<': return {-1, 0};
    }
    return {};
}

int calculate1()
{
    set<Point> visited_houses;
    Point santa_pos;

    for (auto c : line)
    {
        santa_pos += move(c);
        visited_houses.insert(santa_pos);
    }

    return (int)visited_houses.size();
}

int calculate2()
{
    set<Point> visited_houses;
    Point santa_pos;
    Point robot_pos;
    int turn = 0;

    for (auto c : line)
    {
        if (turn % 2 == 1)
        {
            santa_pos += move(c);
            visited_houses.insert(santa_pos);
        }
        else
        {
            robot_pos += move(c);
            visited_houses.insert(robot_pos);
        }
        ++turn;
    }

    return (int)visited_houses.size();
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
