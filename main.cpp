#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/deprecated.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace ftxui;
namespace fs = std::filesystem;

// Global params
fs::path CURRENT_PATH = "/home/vlad/code/cpp/CppFileManager";
int WIDTH_SIZE = 45;

// Items func return vector of items in current Dir
vector<string> Items() {
  vector<string> output;

  for (auto &item : fs::directory_iterator(CURRENT_PATH)) {
    string element;
    if (item.is_directory()) {
      string name = item.path().filename();
      element = " " + name;
    } else if (item.is_regular_file()) {
      string name = item.path().filename();
      element = " " + name;
    }
    output.push_back(element);
  }

  return output;
}

// Returns vbox type element that shows data about selected element
Element GetInfoAboutFile(int &selected) {

  vector<fs::path> items;
  for (auto &item : fs::directory_iterator(CURRENT_PATH)) {
    items.push_back(item);
  }

  string name = items[selected].filename().string();
  string size;

  if (!fs::is_regular_file(items[selected])) {
    size = "-- it`s directory --";
  } else {
    uintmax_t rawSize = fs::file_size(items[selected]);
    if (rawSize < 1024) {
      size = to_string(rawSize) + " B";
    } else if (rawSize < 1024 * 1024) {
      size = to_string(rawSize / 1024) + " KB";
    } else if (rawSize < 1024 * 1024 * 1024) {
      size = to_string(rawSize / (1024 * 1024)) + " MB";
    } else {
      size = to_string(rawSize / (1024 * 1024 * 1024)) + " GB";
    }
  }

  auto output = vbox(text("Name: " + name), separator(), text("Size: " + size));
  return output;
}

int main(int argc, char *argv[]) {

  cout << "\033[2J\033[H";

  auto screen = ScreenInteractive::TerminalOutput();

  int selected = 0;
  auto menu = Menu(Items(), &selected);

  auto render = Renderer(menu, [&] {
    auto name = hbox(text("FILE MAN 0.1")) | hcenter | border;

    auto PathBar = window(text("Current Path"), text(CURRENT_PATH));

    auto left_side = hbox(text("Files"), separator(), menu->Render()) |
                     size(WIDTH, EQUAL, WIDTH_SIZE) | border;
    auto right_side = window(text("Info"), GetInfoAboutFile(selected)) | flex;

    return vbox({name, PathBar, hbox({left_side, right_side | flex})});
  });

  screen.Loop(render);
}
