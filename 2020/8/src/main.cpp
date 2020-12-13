#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>

void initSettings()
{
    oSettings->setGameName("Advent of Code 2020 - 8");
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

#define OP_NOP 0
#define OP_ACC 1
#define OP_JMP 2

#define ERROR_NONE 0
#define ERROR_INVALID_INST 1
#define ERROR_INFINITE_LOOP 2

struct Inst
{
    int op;
    int arg;
};

struct Registers
{
    int accumulator = 0;
    int program_pointer = 0;
};

vector<Inst> program;
Registers registers;

int stringToOp(const string& str)
{
    if (str == "nop") return OP_NOP;
    if (str == "acc") return OP_ACC;
    if (str == "jmp") return OP_JMP;
    return -1;
}

void parseData()
{
    auto lines = getDataLines();

    for (const auto& line : lines)
    {
        auto split = onut::splitString(line, ' ');
        Inst inst;

        inst.op = stringToOp(split[0]);

        auto arg_sign = (split[1][0] == '-') ? -1 : 1;
        auto arg = stoi(split[1].substr(1));
        inst.arg = arg * arg_sign;

        program.push_back(inst);
    }
}

int execute(const vector<Inst>& p)
{
    memset(&registers, 0, sizeof(registers));

    set<int> visited;

    int program_size = (int)p.size();
    while (registers.program_pointer < program_size)
    {
        const auto& inst = p[registers.program_pointer];

        if (visited.count(registers.program_pointer))
            return ERROR_INFINITE_LOOP;
        visited.insert(registers.program_pointer);

        switch (inst.op)
        {
            case OP_NOP:
                registers.program_pointer++;
                break;
            case OP_ACC:
                registers.accumulator += inst.arg;
                registers.program_pointer++;
                break;
            case OP_JMP:
                registers.program_pointer += inst.arg;
                break;
            default:
                return ERROR_INVALID_INST;
        }
    }

    return ERROR_NONE;
}

int calculate1()
{
    execute(program);
    return registers.accumulator;
}

int calculate2()
{
    for (int i = 0, len = (int)program.size(); i < len; ++i)
    {
        if (program[i].op == OP_NOP)
        {
            if (i + program[i].arg >= len ||
                i + program[i].arg < 0)
                continue;

            auto prg_copy = program;
            prg_copy[i].op = OP_JMP;

            if (execute(prg_copy) == ERROR_NONE)
                break;
        }
        else if (program[i].op == OP_JMP)
        {
            auto prg_copy = program;
            prg_copy[i].op = OP_NOP;

            if (execute(prg_copy) == ERROR_NONE)
                break;
        }
    }

    return registers.accumulator;
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
