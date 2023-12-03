#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>
#include <onut/Texture.h>

void initSettings()
{
    oSettings->setGameName("Advent of Code 2015 - 8");
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

vector<string> lines;
int processing_count = 0;
int result1 = -1;
int result2 = -1;

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
    lines = getDataLines();
}

int calculate1()
{
    auto code_len = 0;
    auto mem_len = 0;

    for (const auto &line : lines)
    {
        code_len += (int)line.size();
        for (int i = 1, len = (int)line.size() - 1; i < len; i++)
        {
            if (line[i] == '\\')
            {
                if (line[i + 1] == 'x')
                    i += 3;
                else
                    i++;
            }
            mem_len++;
        }
    }

    return code_len - mem_len;
}

int calculate2()
{
    auto code_len = 0;
    auto new_len = 0;

    for (const auto &line : lines)
    {
        code_len += (int)line.size();
        new_len += 2;
        for (int i = 0, len = (int)line.size(); i < len; i++)
        {
            if (line[i] == '\\' || line[i] == '"')
            {
                new_len++;
            }
            new_len++;
        }
    }

    return new_len - code_len;
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
