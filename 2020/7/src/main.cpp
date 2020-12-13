#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>

void initSettings()
{
    oSettings->setGameName("Advent of Code 2020 - 7");
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

std::vector<std::string> getDataLines()
{
    // Load content
    auto filename = oContentManager->findResourceFile("puzzle_data.txt");
    auto raw_data = onut::getFileData(filename);
    std::string data_string;
    data_string.resize(raw_data.size());
    memcpy(data_string.data(), raw_data.data(), raw_data.size());

    onut::replace(data_string, "\r", "");

    // Split lines
    return std::move(onut::splitString(data_string, '\n', false));
}

using BagColor = string;

struct Bag
{
    string raw;
    BagColor color;
    vector<pair<int, Bag*>> children;
};

vector<Bag> bags;
map<BagColor, Bag*> bag_by_color;

const BagColor MY_BAG = "shiny gold";

void parseData()
{
    auto lines = getDataLines();

    // First collect all root bags
    bags.resize(lines.size());
    int i = 0;
    for (const auto& line : lines)
    {
        Bag bag;

        auto bags_contain_pos = line.find(" bags contain ");

        bag.raw = line.substr(bags_contain_pos + strlen(" bags contain "));
        bag.color = line.substr(0, bags_contain_pos);

        bags[i] = bag;
        bag_by_color[bag.color] = &bags[i];
        i++;
    }

    // Add children
    for (auto& bag : bags)
    {
        auto bags_split = onut::splitString(bag.raw, ',');
        for (auto& child : bags_split)
        {
            auto split = onut::splitString(child, ' ');

            if (split[0] == "no")
                continue;

            auto count = stoi(split[0]);
            auto color = split[1] + ' ' + split[2];

            bag.children.push_back({count, bag_by_color[color]});
        }
    }
}

int recurseCountBag(const BagColor& color, Bag* bag, set<Bag*>& visited)
{
    int count = 0;

    for (auto& child : bag->children)
    {
        if (visited.count(child.second))
            continue;

        if (child.second->color == color)
            count++;

        visited.insert(child.second);
        count += recurseCountBag(color, child.second, visited);
    }

    return count;
}

int recurseCountBagsInside(Bag* bag)
{
    int count = 1;

    for (auto& child : bag->children)
    {
        count += recurseCountBagsInside(child.second) * child.first;
    }

    return count;
}

int calculate1()
{
    int count = 0;

    for (auto& bag : bags)
    {
        set<Bag*> visited;
        visited.insert(&bag);
        count += recurseCountBag(MY_BAG, &bag, visited);
    }

    return count;
}

int calculate2()
{
    return recurseCountBagsInside(bag_by_color[MY_BAG]) - 1;
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

    for (const auto& bag : bags)
    {
        if (ImGui::TreeNode((bag.color + " (" + to_string(bag.children.size()) + ")").c_str()))
        {
            for (const auto& it : bag.children)
            {
                ImGui::Text("%i %s", it.first, it.second->color.c_str());
            }
            ImGui::TreePop();
        }
    }

    ImGui::End();
}
