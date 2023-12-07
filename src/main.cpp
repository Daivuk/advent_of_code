#include <imgui/imgui.h>
#include <onut/ContentManager.h>
#include <onut/Renderer.h>
#include <onut/Settings.h>
#include <onut/Strings.h>
#include <onut/Texture.h>

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


atomic<int> calculated_count = 0;
mutex calculated_count_mutex;
atomic<float> sub_progress1 = 0.0f;
atomic<float> sub_progress2 = 0.0f;

vector<string> test_data1;
vector<string> test_data2;
vector<string> puzzle_data;

int64_t test_result1 = -1;
int64_t test_result2 = -1;
int64_t result1 = -1;
int64_t result2 = -1;

using duration_t = decltype(chrono::steady_clock::now() - chrono::steady_clock::now());

duration_t test_data1_parsing_duration;
duration_t test_data2_parsing_duration;
duration_t puzzle_data_loading_duration;
duration_t test1_duration;
duration_t part1_duration;
duration_t test2_duration;
duration_t part2_duration;


int64_t calculate_part1(const vector<string>& data);
int64_t calculate_part2(const vector<string>& data);


vector<string> load_data(const string& in_filename, duration_t* time_took)
{
    auto time_start = chrono::steady_clock::now();

    // Load content
    auto filename = oContentManager->findResourceFile(in_filename);
    auto raw_data = onut::getFileData(filename);
    string data_string;
    data_string.resize(raw_data.size());
    memcpy(data_string.data(), raw_data.data(), raw_data.size());

    onut::replace(data_string, "\r", "");

    // Split lines
    auto ret = onut::splitString(data_string, '\n', false);

    auto time_end = chrono::steady_clock::now();
    *time_took = time_end - time_start;

    return ret;
}


void calculate_worker(int64_t (*calculate_fn)(const vector<string>& data), const vector<string>& data, int64_t* result, duration_t* time_took)
{
    auto time_start = chrono::steady_clock::now();
    *result = calculate_fn(data);
    auto time_end = chrono::steady_clock::now();
    *time_took = time_end - time_start;

    calculated_count++;
}


void start_calculations()
{
    thread{calculate_worker, calculate_part1, test_data1, &test_result1, &test1_duration}.detach();
    thread{calculate_worker, calculate_part1, puzzle_data, &result1, &part1_duration}.detach();
    thread{calculate_worker, calculate_part2, test_data2, &test_result2, &test2_duration}.detach();
    thread{calculate_worker, calculate_part2, puzzle_data, &result2, &part2_duration}.detach();
}


void load_data_worker(const string& filename, vector<string>* data, duration_t* time_took)
{
    *data = load_data(filename, time_took);

    bool should_start_calculations = false;
    calculated_count_mutex.lock();
    calculated_count++;
    if (calculated_count == 3) should_start_calculations = true;
    calculated_count_mutex.unlock();
    if (should_start_calculations) start_calculations();
}


void load_all_data()
{
    thread{load_data_worker, "test_data1.txt", &test_data1, &test_data1_parsing_duration}.detach();
    thread{load_data_worker, "test_data2.txt", &test_data2, &test_data2_parsing_duration}.detach();
    thread{load_data_worker, "puzzle_data.txt", &puzzle_data, &puzzle_data_loading_duration}.detach();
}


void init()
{
    load_all_data();
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

    if (calculated_count < 7)
    {
        static const char CS[] = { '/', '-', '\\', '|' };
        static auto start_time = chrono::steady_clock::now();

        auto elapsed = chrono::steady_clock::now() - start_time;
        auto frame_passed = elapsed / chrono::milliseconds(100);

        ImGui::Text("Calculating... %c", CS[frame_passed % 4]);
        ImGui::ProgressBar((float)calculated_count / 7.0f);
        ImGui::ProgressBar((float)sub_progress1);
        ImGui::ProgressBar((float)sub_progress2);
    }
    else
    {
        ImGui::Text("Loading Puzzle data took %s", duration_to_string(puzzle_data_loading_duration).c_str());
        if (ImGui::CollapsingHeader("Part 1", ImGuiTreeNodeFlags_DefaultOpen))
        {
            {
                auto text = to_string(test_result1);
                ImGui::InputText("Test result##test1", (char*)text.c_str(), text.size() + 1);
                ImGui::Indent();
                ImGui::Text("Took %s", duration_to_string(test1_duration).c_str());
                ImGui::Unindent();
            }
            {
                auto text = to_string(result1);
                ImGui::InputText("Result##ret1", (char*)text.c_str(), text.size() + 1);
                ImGui::Indent();
                ImGui::Text("Took %s", duration_to_string(part1_duration).c_str());
                ImGui::Unindent();
            }
        }

        if (ImGui::CollapsingHeader("Part 2", ImGuiTreeNodeFlags_DefaultOpen))
        {
            {
                auto text = to_string(test_result2);
                ImGui::InputText("Test result##test2", (char*)text.c_str(), text.size() + 1);
                ImGui::Indent();
                ImGui::Text("Took %s", duration_to_string(test2_duration).c_str());
                ImGui::Unindent();
            }
            {
                auto text = to_string(result2);
                ImGui::InputText("Result##ret2", (char*)text.c_str(), text.size() + 1);
                ImGui::Indent();
                ImGui::Text("Took %s", duration_to_string(part2_duration).c_str());
                ImGui::Unindent();
            }
        }
    }

    ImGui::End();
}
