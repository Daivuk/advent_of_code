#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>
#include <onut/Texture.h>

void initSettings()
{
    oSettings->setGameName("Advent of Code 2015 - 7");
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

struct Wire
{
    enum class Instruction
    {
        INVALID,
        IMMEDIATE,
        NOT,
        LSHIFT,
        RSHIFT,
        OR,
        AND
    } instruction;
    string left_val;
    string right_val;
    string store_reg;
};

using Wires = unordered_map<string, Wire>;
Wires wires;

Wire::Instruction parseInst(const string &s)
{
    if (s == "NOT") return Wire::Instruction::NOT;
    if (s == "LSHIFT") return Wire::Instruction::LSHIFT;
    if (s == "RSHIFT") return Wire::Instruction::RSHIFT;
    if (s == "OR") return Wire::Instruction::OR;
    if (s == "AND") return Wire::Instruction::AND;
    assert(false);
    return Wire::Instruction::INVALID;
}

void parseData()
{
    auto lines = getDataLines();
    for (const auto &line : lines)
    {
        auto split = onut::splitString(line, ' ');

        Wire wire;
        if (split.size() == 5)
        {
            wire.left_val = split[0];
            wire.instruction = parseInst(split[1]);
            wire.right_val = split[2];
            wire.store_reg = split[4];
        }
        else if (split.size() == 4)
        {
            wire.instruction = parseInst(split[0]);
            wire.right_val = split[1];
            wire.store_reg = split[3];
        }
        else if (split.size() == 3)
        {
            wire.instruction = Wire::Instruction::IMMEDIATE;
            wire.left_val = split[0];
            wire.store_reg = split[2];
        }
        else
        {
            assert(false);
            wire.instruction = Wire::Instruction::INVALID;
        }
        wires[wire.store_reg] = wire;
    }
}

using Cache = map<string, uint16_t>;

uint16_t solve(const string &reg, Cache &cache)
{
    auto it = cache.find(reg);
    if (it != cache.end()) return it->second;

    if (reg[0] < 'a' || reg[0] > 'z')
    {
        auto val = (uint16_t)stoi(reg);
        cache[reg] = val;
        return val;
    }

    const auto &wire = wires[reg];

    uint16_t ret = 0;
    switch (wire.instruction)
    {
        case Wire::Instruction::IMMEDIATE:
            ret = solve(wire.left_val, cache);
            break;
        case Wire::Instruction::NOT:
            ret = ~solve(wire.right_val, cache);
            break;
        case Wire::Instruction::LSHIFT:
            ret = solve(wire.left_val, cache) << solve(wire.right_val, cache);
            break;
        case Wire::Instruction::RSHIFT:
            ret = solve(wire.left_val, cache) >> solve(wire.right_val, cache);
            break;
        case Wire::Instruction::OR:
            ret = solve(wire.left_val, cache) | solve(wire.right_val, cache);
            break;
        case Wire::Instruction::AND:
            ret = solve(wire.left_val, cache) & solve(wire.right_val, cache);
            break;
        default:
            assert(false);
            ret = 0;
            break;
    }

    cache[reg] = ret;
    return ret;
}

int calculate1()
{
    Cache cache;

    return solve("a", cache);
}

int calculate2()
{
    Cache cache;

    auto a_val = solve("a", cache);
    
    cache.clear();
    cache["b"] = a_val;

    return solve("a", cache);
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
