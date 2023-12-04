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

vector<string> test_data1;
vector<string> test_data2;
vector<string> puzzle_data;

int test_result1 = -1;
int test_result2 = -1;
int result1 = -1;
int result2 = -1;


int calculate_part1(const vector<string>& data);
int calculate_part2(const vector<string>& data);


vector<string> load_data(const string& in_filename)
{
    // Load content
    auto filename = oContentManager->findResourceFile(in_filename);
    auto raw_data = onut::getFileData(filename);
    string data_string;
    data_string.resize(raw_data.size());
    memcpy(data_string.data(), raw_data.data(), raw_data.size());

    onut::replace(data_string, "\r", "");

    // Split lines
    return move(onut::splitString(data_string, '\n', false));
}


void calculate_worker(int (*calculate_fn)(const vector<string>& data), const vector<string>& data, int* result)
{
    *result = calculate_fn(data);
    calculated_count++;
}


void start_calculations()
{
    thread{calculate_worker, calculate_part1, test_data1, &test_result1}.detach();
    thread{calculate_worker, calculate_part1, puzzle_data, &result1}.detach();
    thread{calculate_worker, calculate_part2, test_data2, &test_result2}.detach();
    thread{calculate_worker, calculate_part2, puzzle_data, &result2}.detach();
}


void load_data_worker(const string& filename, vector<string>* data)
{
    *data = load_data(filename);

    bool should_start_calculations = false;
    calculated_count_mutex.lock();
    calculated_count++;
    if (calculated_count == 3) should_start_calculations = true;
    calculated_count_mutex.unlock();
    if (should_start_calculations) start_calculations();
}


void load_all_data()
{
    thread{load_data_worker, "test_data1.txt", &test_data1}.detach();
    thread{load_data_worker, "test_data2.txt", &test_data2}.detach();
    thread{load_data_worker, "puzzle_data.txt", &puzzle_data}.detach();
}


void init()
{
    load_all_data();
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
        ImGui::ProgressBar((float)calculated_count / 4.0f);
    }
    else
    {
        if (ImGui::CollapsingHeader("Part 1", ImGuiTreeNodeFlags_DefaultOpen))
        {
            {
                auto text = to_string(test_result1);
                ImGui::InputText("Test result##test1", (char*)text.c_str(), text.size() + 1);
            }
            {
                auto text = to_string(result1);
                ImGui::InputText("Result##ret1", (char*)text.c_str(), text.size() + 1);
            }
        }

        if (ImGui::CollapsingHeader("Part 2", ImGuiTreeNodeFlags_DefaultOpen))
        {
            {
                auto text = to_string(test_result2);
                ImGui::InputText("Test result##test2", (char*)text.c_str(), text.size() + 1);
            }
            {
                auto text = to_string(result2);
                ImGui::InputText("Result##ret2", (char*)text.c_str(), text.size() + 1);
            }
        }
    }

    ImGui::End();
}
