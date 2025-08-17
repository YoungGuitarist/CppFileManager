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

// Глобальные параметры
fs::path CURRENT_PATH = "/home/vlad/code/cpp/CppFileManager";
int WIDTH_SIZE = 45;

// Функция возвращает вектор элементов в текущей директории
vector<string> GetItems() {
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

// Возвращает информацию о выбранном файле
Element GetInfoAboutFile(int selected) {
  vector<fs::path> items;
  for (auto &item : fs::directory_iterator(CURRENT_PATH)) {
    items.push_back(item);
  }

  if (items.empty()) {
    return vbox(text("Нет файлов в директории"));
  }

  // Проверка границ выбранного элемента
  if (selected >= items.size()) {
    selected = items.size() - 1;
  }
  if (selected < 0) {
    selected = 0;
  }

  string name = items[selected].filename().string();
  string size;

  if (!fs::is_regular_file(items[selected])) {
    size = "-- это директория --";
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

  return vbox(text("Имя: " + name), separator(), text("Размер: " + size));
}

int main(int argc, char *argv[]) {
  cout << "\033[2J\033[H";

  string input_text;
  string active_window = "main";

  auto screen = ScreenInteractive::TerminalOutput();
  int selected = 0;
  vector<string> items = GetItems();

  // Создаем компоненты
  auto menu = Menu(&items, &selected);
  auto searchInput =
      Input(&input_text, "Введите путь...") | center | border | flex;

  // Контейнер для компонентов
  auto components = Container::Vertical({
      menu,
      searchInput,
  });

  auto render = Renderer(components, [&] {
    // Глобальные элементы
    auto name = hbox(text("ФАЙЛОВЫЙ МЕНЕДЖЕР 0.1")) | hcenter | border;
    auto PathBar = window(text("Текущий путь"), text(CURRENT_PATH));

    // Основной экран
    auto left_side =
        hbox(text("Файлы"), separator(),
             menu->Render() | frame | size(HEIGHT, LESS_THAN, 15)) |
        size(WIDTH, EQUAL, WIDTH_SIZE) | border;
    auto right_side =
        window(text("Информация"), GetInfoAboutFile(selected)) | flex;
    auto MainScreen =
        vbox({name, PathBar, hbox({left_side, right_side | flex})});

    // Экран смены пути
    auto inputPlace =
        hbox(text("Введите путь: ") | vcenter, searchInput->Render()) | center;
    auto ChangePathScreen = vbox({name, PathBar, inputPlace});

    if (active_window == "main") {
      return MainScreen;
    } else if (active_window == "find") {
      return ChangePathScreen;
    }
    return vbox(text("Ошибка")) | center;
  });

  auto component = CatchEvent(render, [&](Event e) {
    if (e == Event::Escape && active_window == "main") {
      screen.Exit();
      return true;
    } else if (e == Event::Escape) {
      screen.Post([&] { menu->TakeFocus(); });
      selected = 1;
      active_window = "main";
    } else if (e == Event::Character('f')) {
      active_window = "find";
      screen.Post([&] { searchInput->TakeFocus(); });
    } else if (e == Event::Return && active_window == "find") {
      CURRENT_PATH = input_text;
      input_text.clear();
      active_window = "main";
      items = GetItems();
      selected = 0;
      input_text.clear();
      return true;
    }
    return false;
  });

  screen.Loop(component);
}
