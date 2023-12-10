#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>
#include <onut/Texture.h>
#include "main.h"


atomic<int> calculated_count = 0;
mutex calculated_count_mutex;
vector<puzzle_t*> puzzles;

int64_t calculate_part1(puzzle_t* puzzle);
int64_t calculate_part2(puzzle_t* puzzle);

int64_t (*calculate_part_fns[])(puzzle_t* puzzle) = { nullptr, calculate_part1, calculate_part2 };


void initSettings()
{
    oSettings->setGameName("Advent of Code " PROBLEM_YEAR " - " PROBLEM_ID);
    oSettings->setIsResizableWindow(true);
    oSettings->setResolution({1600, 900});
    oSettings->setShowFPS(true);
    oSettings->setShowOnScreenLog(true);
}


void shutdown()
{
    for (auto puzzle : puzzles)
    {
        delete puzzle;
    }
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


void load_data(puzzle_t* puzzle)
{
    auto time_start = chrono::steady_clock::now();

    // Load content
    auto filename = puzzle->data_filename;
    auto raw_data = onut::getFileData(filename);
    string data_string;
    data_string.resize(raw_data.size());
    memcpy(data_string.data(), raw_data.data(), raw_data.size());

    onut::replace(data_string, "\r", "");

    // Split lines
    puzzle->data = onut::splitString(data_string, '\n', false);

    auto time_end = chrono::steady_clock::now();
    puzzle->data_parsing_duration = time_end - time_start;
}


void calculate_worker(int64_t (*calculate_fn)(puzzle_t* puzzle), puzzle_t* puzzle)
{
    auto time_start = chrono::steady_clock::now();
    puzzle->result = calculate_fn(puzzle);
    puzzle->progress = 1.0f;
    auto time_end = chrono::steady_clock::now();
    puzzle->duration = time_end - time_start;

    calculated_count++;
}


void start_calculations()
{
    for (auto puzzle : puzzles)
        thread{calculate_worker, calculate_part_fns[puzzle->part], puzzle}.detach();
}


void load_data_worker(puzzle_t* puzzle)
{
    load_data(puzzle);

    bool should_start_calculations = false;
    calculated_count_mutex.lock();
    calculated_count++;
    if (calculated_count == (int)puzzles.size()) should_start_calculations = true;
    calculated_count_mutex.unlock();
    if (should_start_calculations)
    {
        start_calculations();
    }
}


void create_tests(int part)
{
    // Setup puzzles and tests
    if (onut::fileExists("assets/test_data" + to_string(part) + ".txt"))
    {
        puzzle_t* puzzle = new puzzle_t();
        puzzle->is_test = true;
        puzzle->data_filename = "assets/test_data" + to_string(part) + ".txt";
        puzzle->part = part;
        puzzle->name = "Part " + to_string(part) + " test";
        puzzles.push_back(puzzle);
    }
    else if (onut::fileExists("assets/test_data" + to_string(part) + ".1.txt"))
    {
        int i = 1;
        while (onut::fileExists("assets/test_data" + to_string(part) + "." + to_string(i) + ".txt"))
        {
            puzzle_t* puzzle = new puzzle_t();
            puzzle->is_test = true;
            puzzle->data_filename = "assets/test_data" + to_string(part) + "." + to_string(i) + ".txt";
            puzzle->part = part;
            puzzle->name = "Part " + to_string(part) + " test " + to_string(i);
            puzzles.push_back(puzzle);
            ++i;
        }
    }
}


void create_puzzle(int part)
{
    puzzle_t* puzzle = new puzzle_t();
    puzzle->is_test = true;
    puzzle->data_filename = "assets/puzzle_data.txt";
    puzzle->part = part;
    puzzle->name = "Part " + to_string(part);
    puzzles.push_back(puzzle);
}


void init()
{
    // Create puzzles
    create_tests(1);
    create_puzzle(1);

    create_tests(2);
    create_puzzle(2);

    // Load data
    for (auto puzzle : puzzles)
        thread{load_data_worker, puzzle}.detach();
}


static string duration_to_string(const duration_t& duration)
{
    auto ns = duration.count();
    auto us = ns / 1000;
    auto ms = us / 1000;
    auto s = ms / 1000;

    char buf[64];
    if (s > 0) snprintf(buf, 64, "%lld.%03lld s", s, ms % 1000);
    else if (ms > 0) snprintf(buf, 64, "%lld.%03lld ms", ms, us % 1000);
    else snprintf(buf, 64, "%lld.%03lld us", us, ns % 1000);
    return buf;
}


vector<int64_t> parse_numbers(const string& str)
{
    auto splits = onut::splitString(str, ' ');
    vector<int64_t> ret;
    for (const auto& split : splits)
    {
        ret.push_back(stoll(split));
    }
    return ret;
}


void renderUI()
{
    ImGui::Begin("Result");

    for (const auto puzzle : puzzles)
    {
        if (puzzle->progress < 1.0f)
        {
            ImGui::Text("%s:", puzzle->name.c_str());
            ImGui::ProgressBar(puzzle->progress);
        }
        else
        {
            auto text = to_string(puzzle->result);
            ImGui::InputText((puzzle->name + " result").c_str(), (char*)text.c_str(), text.size() + 1);
            ImGui::Indent();
            ImGui::Text("Took %s", duration_to_string(puzzle->duration).c_str());
            ImGui::Unindent();
        }
    }

    ImGui::End();
}
