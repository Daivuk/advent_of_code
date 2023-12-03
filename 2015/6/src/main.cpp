#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>
#include <onut/Texture.h>

void initSettings()
{
    oSettings->setGameName("Advent of Code 2015 - 6");
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

vector<uint32_t> lights1;
vector<uint32_t> lights2;
OTextureRef lights_texture1;
OTextureRef lights_texture2;

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

struct Instruction
{
    enum class Action
    {
        TurnOn,
        TurnOff,
        Toggle
    } action;
    Point from;
    Point to;
};

vector<Instruction> instructions;

void parseData()
{
    auto lines = getDataLines();

    instructions.reserve(lines.size());

    for (const auto &line : lines)
    {
        auto split = onut::splitString(line, ' ');

        Instruction instruction;

        if (split[0] == "toggle") instruction.action = Instruction::Action::Toggle;
        else if (split[1] == "on") instruction.action = Instruction::Action::TurnOn;
        else instruction.action = Instruction::Action::TurnOff;

        auto from_split = onut::splitString(split[split.size() - 3], ',');
        auto to_split = onut::splitString(split[split.size() - 1], ',');

        instruction.from = { stoi(from_split[0]), stoi(from_split[1]) }; 
        instruction.to = { stoi(to_split[0]), stoi(to_split[1]) };

        instructions.push_back(instruction);
    }
}

int calculate1()
{
    lights1.resize(1000 * 1000, 0xFF000000);

    for (const auto &instruction : instructions)
    {
        for (int y = instruction.from.y; y <= instruction.to.y; ++y)
        {
            for (int x = instruction.from.x; x <= instruction.to.x; ++x)
            {
                auto &light = lights1[y * 1000 + x];
                switch (instruction.action) // More efficient to switch first, then loop. But we want smaller code.
                {
                    case Instruction::Action::Toggle:
                        if (light == 0xFF000000) light = 0xFFFFFFFF;
                        else light = 0xFF000000;
                        break;
                    case Instruction::Action::TurnOn:
                        light = 0xFFFFFFFF;
                        break;
                    case Instruction::Action::TurnOff:
                        light = 0xFF000000;
                        break;
                }
            }
        }
    }

    result1 = 0;
    for (auto light : lights1) if (light == 0xFFFFFFFF) result1++;
    return result1;
}

int calculate2()
{
    vector<int> brightnesses;
    brightnesses.resize(1000 * 1000, 0);
    lights2.resize(1000 * 1000, 0xFF000000);

    for (const auto &instruction : instructions)
    {
        for (int y = instruction.from.y; y <= instruction.to.y; ++y)
        {
            for (int x = instruction.from.x; x <= instruction.to.x; ++x)
            {
                auto &brightness = brightnesses[y * 1000 + x];
                switch (instruction.action) // More efficient to switch first, then loop. But we want smaller code.
                {
                    case Instruction::Action::Toggle:
                        brightness += 2;
                        break;
                    case Instruction::Action::TurnOn:
                        brightness++;
                        break;
                    case Instruction::Action::TurnOff:
                        brightness = max(0, brightness - 1);
                        break;
                }
            }
        }
    }

    result2 = 0;
    int highest = 0;
    for (auto brightness : brightnesses)
    {
        result2 += brightness;
        highest = max(highest, brightness);
    }

    for (int i = 0, len = 1000 * 1000; i < len; ++i)
    {
        uint8_t *pixel = (uint8_t *)&lights2[i];
        auto brightness = brightnesses[i];
        uint8_t amount = (uint8_t)(brightness * 255 / highest);
        pixel[0] = amount;
        pixel[1] = amount;
        pixel[2] = amount;
    }

    return result2;
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
            lights_texture1 = OTexture::createFromData((const uint8_t *)lights1.data(), {1000, 1000}, false);
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
            lights_texture2 = OTexture::createFromData((const uint8_t *)lights2.data(), {1000, 1000}, false);
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

    if (lights_texture1)
    {
        ImGui::Image(&lights_texture1, {1000, 1000});
    }
    if (lights_texture2)
    {
        ImGui::Image(&lights_texture2, {1000, 1000});
    }

    ImGui::End();
}
