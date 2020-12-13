#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>

void initSettings()
{
    oSettings->setGameName("Advent of Code 2015 - 5");
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

vector<string> strings;

void parseData()
{
    auto lines = getDataLines();
    strings = lines;
}

int countVowels(const string& str)
{
    const string VOWELS = "aeiou";
    int count = 0;

    for (auto c : str)
    {
        for (auto vowel : VOWELS)
        {
            if (c == vowel)
            {
                count++;
                break;
            }
        }
    }

    return count;
}

bool hasDouble(const string& str)
{
    for (int i = 0, len = (int)str.size() - 1; i < len; ++i)
        if (str[i] == str[i + 1])
            return true;

    return false;
}

bool isClean(const string& str)
{
    if (str.find("ab") != string::npos)
        return false;

    if (str.find("cd") != string::npos)
        return false;

    if (str.find("pq") != string::npos)
        return false;

    if (str.find("xy") != string::npos)
        return false;

    return true;
}

bool hasPair(const string& str)
{
    for (int i = 0, len = (int)str.size() - 3; i < len; ++i)
    {
        auto pair1 = str.substr(i, 2);
        for (int j = i + 2; j < len + 2; ++j)
        {
            auto pair2 = str.substr(j, 2);
            if (pair1 == pair2)
                return true;
        }
    }

    return false;
}

bool hasSpacedRepeat(const string& str)
{
    for (int i = 0, len = (int)str.size() - 2; i < len; ++i)
        if (str[i] == str[i + 2] && str[i] != str[i + 1])
            return true;

    return false;
}

bool isNice2(const string& str)
{
    return hasPair(str) && hasSpacedRepeat(str);
}

int calculate1()
{
    int nice_count = 0;

    for (const auto& str : strings)
        if (isClean(str))
            if (hasDouble(str))
                if (countVowels(str) >= 3)
                    nice_count++;

    return nice_count;
}

int calculate2()
{
    int nice_count = 0;

    for (const auto& str : strings)
        if (isNice2(str))
            nice_count++;

    return nice_count;
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

    for (const auto& str : strings)
    {
        ImGui::TextColored({
            1, 
            hasPair(str) ? 1.f : 0.f, 
            hasSpacedRepeat(str) ? 1.f : 0.f, 
            1}, str.c_str());
    }


    ImGui::End();
}
