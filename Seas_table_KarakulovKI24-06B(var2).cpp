#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <map>
#include <functional>
#define NOMINMAX
#include <Windows.h>

const int kMaxSeas = 10;              // Максимальное количество морей
const double kMinDepth = 0.0;         // Минимальная допустимая глубина
const double kMaxDepth = 12000.0;     // Максимальная допустимая глубина
const double kMinSalinity = 0.0;      // Минимальная допустимая солёность
const double kMaxSalinity = 45.0;     // Максимальная допустимая солёность

// Структура для хранения данных о море
struct Sea {
    std::string name;        // Название моря
    double depth;       // Глубина в метрах
    double salinity;    // Солёность в промилле
};

// Структура для информации о функции меню
struct MenuInfo {
    std::function<void()> callback;
    std::string description;
};

// Функция для безопасного ввода числа с проверкой диапазона
double SafeInputDouble(const std::string& prompt, double min_value, double max_value) {
    double value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ошибка! Введите число: ";
        }
        else if (value < min_value || value > max_value) {
            std::cout << "Ошибка! Значение должно быть от " << min_value
                << " до " << max_value << ": ";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
    }
    return value;
}

// Функция для ввода данных об одном море
void InputSeaData(Sea& sea) {
    std::cout << "\n--- Ввод данных о море ---" << std::endl;

    std::cout << "Введите название моря: ";
    std::getline(std::cin, sea.name);
    while (sea.name.empty()) {
        std::getline(std::cin, sea.name);
    }

    sea.depth = SafeInputDouble("Введите глубину (м): ", kMinDepth, kMaxDepth);
    sea.salinity = SafeInputDouble("Введите солёность (‰): ", kMinSalinity, kMaxSalinity);
}

// Функция для вывода данных об одном море в табличном формате
void PrintSeaData(const Sea& sea) {
    std::cout << "| " << std::setw(20) << std::left << sea.name << " | "
        << std::setw(10) << std::right << std::fixed << std::setprecision(2) << sea.depth << " | "
        << std::setw(12) << std::right << sea.salinity << " |" << std::endl;
}

// Функция для поиска индекса самого глубокого моря
int FindDeepestSeaIndex(const std::vector<Sea>& seas) {
    if (seas.empty()) return -1;

    int max_index = 0;
    for (int i = 1; i < seas.size(); ++i) {
        if (seas[i].depth > seas[max_index].depth) {
            max_index = i;
        }
    }
    return max_index;
}

// Функция для поиска индекса моря с минимальной солёностью
int FindLeastSaltySeaIndex(const std::vector<Sea>& seas) {
    if (seas.empty()) return -1;

    int min_index = 0;
    for (int i = 1; i < seas.size(); ++i) {
        if (seas[i].salinity < seas[min_index].salinity) {
            min_index = i;
        }
    }
    return min_index;
}

// Функция для вычисления средней глубины
double CalculateAverageDepth(const std::vector<Sea>& seas) {
    if (seas.empty()) return 0.0;

    double sum = 0.0;
    for (const auto& sea : seas) {
        sum += sea.depth;
    }
    return sum / seas.size();
}

// Функция для сортировки морей по глубине (по убыванию)
void SortSeasByDepth(std::vector<Sea>& seas) {
    std::sort(seas.begin(), seas.end(), [](const Sea& a, const Sea& b) {
        return a.depth > b.depth; // Сортировка по убыванию глубины
        });
}

// Функция для поиска морей по солёности
void FindSeasBySalinity(const std::vector<Sea>& seas, double target_salinity, double tolerance = 0.1) {
    std::cout << "\n=== МОРЯ С СОЛЁНОСТЬЮ " << target_salinity << "‰ (±" << tolerance << "‰) ===" << std::endl;
    std::cout << "+----------------------+------------+--------------+" << std::endl;

    bool found = false;
    for (const auto& sea : seas) {
        if (std::abs(sea.salinity - target_salinity) <= tolerance) {
            PrintSeaData(sea);
            found = true;
        }
    }

    if (!found) {
        std::cout << "| Моря с указанной солёностью не найдены. |" << std::endl;
    }
    std::cout << "+----------------------+------------+--------------+" << std::endl;
}

// Функция для чтения данных из файла
int ReadSeasFromFile(const std::string& filename, std::vector<Sea>& seas) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла " << filename << std::endl;
        return 0;
    }

    int count = 0;
    std::string line;

    // Пропускаем заголовок если есть
    std::getline(file, line);

    while (count < kMaxSeas && std::getline(file, line)) {
        if (line.empty()) continue;

        Sea sea;
        std::size_t pos1 = line.find(';');
        std::size_t pos2 = line.find(';', pos1 + 1);

        if (pos1 != std::string::npos && pos2 != std::string::npos) {
            sea.name = line.substr(0, pos1);
            sea.depth = std::stod(line.substr(pos1 + 1, pos2 - pos1 - 1));
            sea.salinity = std::stod(line.substr(pos2 + 1));

            seas.push_back(sea);
            count++;
        }
    }

    file.close();
    return count;
}

// Функция для записи данных в файл
void WriteSeasToFile(const std::string& filename, const Sea seas[], int sea_count) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла " << filename << " для записи" << std::endl;
        return;
    }

    // Записываем заголовок
    file << "Название;Глубина;Солёность" << std::endl;

    // Записываем данные
    for (int i = 0; i < sea_count; ++i) {
        file << seas[i].name << ";" << seas[i].depth << ";" << seas[i].salinity << std::endl;
    }

    file.close();
    std::cout << "Данные успешно записаны в файл " << filename << std::endl;
}

// Функция для удаления моря из файла
void RemoveSeaFromFile(const std::string& filename, const std::string& sea_name) {
    // Сначала читаем все данные из файла
    Sea seas[kMaxSeas];
    int sea_count = ReadSeasFromFile(filename, seas);

    if (sea_count == 0) {
        std::cout << "Файл пуст или не содержит данных" << std::endl;
        return;
    }

    // Ищем море для удаления
    bool found = false;
    int new_count = 0;
    Sea new_seas[kMaxSeas];

    for (int i = 0; i < sea_count; ++i) {
        if (seas[i].name != sea_name) {
            new_seas[new_count] = seas[i];
            new_count++;
        }
        else {
            found = true;
            std::cout << "Море '" << sea_name << "' найдено и будет удалено" << std::endl;
        }
    }

    if (!found) {
        std::cout << "Море с названием '" << sea_name << "' не найдено в файле" << std::endl;
        return;
    }

    // Записываем обновленные данные обратно в файл
    WriteSeasToFile(filename, new_seas, new_count);
    std::cout << "Море успешно удалено из файла. Осталось " << new_count << " морей." << std::endl;
}

// Функция для вывода всех морей
void PrintAllSeas(const Sea seas[], int sea_count) {
    std::cout << "\n=== ТАБЛИЦА МОРЕЙ ===" << std::endl;
    std::cout << "+----------------------+------------+--------------+" << std::endl;
    std::cout << "| Название             | Глубина (м)| Солёность (‰)|" << std::endl;
    std::cout << "+----------------------+------------+--------------+" << std::endl;

    for (int i = 0; i < sea_count; ++i) {
        PrintSeaData(seas[i]);
    }
    std::cout << "+----------------------+------------+--------------+" << std::endl;
}

// Функция для ввода данных с клавиатуры
void InputFromKeyboard(std::vector<Sea>& seas) {
    int sea_count;
    std::cout << "\nВведите количество морей (1-" << kMaxSeas << "): ";
    std::cin >> sea_count;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Проверка корректности введённого количества
    if (sea_count < 1 || sea_count > kMaxSeas) {
        std::cout << "Ошибка! Количество морей должно быть от 1 до " << kMaxSeas << std::endl;
        return;
    }

    // Ввод данных о морях
    std::cout << "\n=== ВВОД ДАННЫХ О МОРЯХ ===" << std::endl;
    for (int i = 0; i < sea_count; ++i) {
        std::cout << "\nМоре #" << (i + 1) << ":" << std::endl;
        Sea sea;
        InputSeaData(sea);
        seas.push_back(sea);
    }
}

// Функция для чтения данных из файла
void InputFromFile(std::vector<Sea>& seas) {
    int count = ReadSeasFromFile("seas.txt", seas);
    if (count == 0) {
        std::cout << "Не удалось прочитать данные из файла." << std::endl;
        return;
    }
    std::cout << "Успешно прочитано " << count << " морей из файла." << std::endl;
}

// Функция для вывода отсортированной таблицы
void PrintSortedSeas(std::vector<Sea>& seas) {
    SortSeasByDepth(seas);
    std::cout << "\n=== ТАБЛИЦА МОРЕЙ (ОТСОРТИРОВАННАЯ ПО ГЛУБИНЕ) ===" << std::endl;
    std::cout << "+----------------------+------------+--------------+" << std::endl;
    std::cout << "| Название             | Глубина (м)| Солёность (‰)|" << std::endl;
    std::cout << "+----------------------+------------+--------------+" << std::endl;
    for (const auto& sea : seas) {
        PrintSeaData(sea);
    }
    std::cout << "+----------------------+------------+--------------+" << std::endl;
}

// Функция для поиска по солёности
void SearchBySalinity(const std::vector<Sea>& seas) {
    double target_salinity;
    std::cout << "\n=== ПОИСК ПО СОЛЁНОСТИ ===" << std::endl;
    std::cout << "Введите солёность для поиска: ";
    std::cin >> target_salinity;
    FindSeasBySalinity(seas, target_salinity);
}

// Функция для вывода самого глубокого моря
void PrintDeepestSea(const std::vector<Sea>& seas) {
    int deepest_index = FindDeepestSeaIndex(seas);
    if (deepest_index != -1) {
        std::cout << "\n+ САМОЕ ГЛУБОКОЕ МОРЕ -+------------+--------------+" << std::endl;
        PrintSeaData(seas[deepest_index]);
    }
}

// Функция для вывода самого пресного моря
void PrintLeastSaltySea(const std::vector<Sea>& seas) {
    int least_salty_index = FindLeastSaltySeaIndex(seas);
    if (least_salty_index != -1) {
        std::cout << "+- САМОЕ ПРЕСНОЕ МОРЕ -+------------+--------------+" << std::endl;
        PrintSeaData(seas[least_salty_index]);
        std::cout << "+----------------------+------------+--------------+" << std::endl;
    }
}

// Функция для вывода статистики
void PrintStatistics(const std::vector<Sea>& seas) {
    double average_depth = CalculateAverageDepth(seas);
    std::cout << "\n=== СТАТИСТИКА ===" << std::endl;
    std::cout << "Средняя глубина всех морей: " << average_depth << " м" << std::endl;

    // Поиск морей глубже средней глубины
    std::cout << "\n=== МОРЯ ГЛУБЖЕ СРЕДНЕЙ ГЛУБИНЫ ===" << std::endl;
    std::cout << "+----------------------+------------+--------------+" << std::endl;
    bool found_deeper = false;
    for (const auto& sea : seas) {
        if (sea.depth > average_depth) {
            PrintSeaData(sea);
            found_deeper = true;
        }
    }
    if (!found_deeper) {
        std::cout << "| Моря глубже средней глубины не найдены. |" << std::endl;
    }
    std::cout << "+----------------------+------------+--------------+" << std::endl;
}

// Функция для вывода меню выбора способа ввода с использованием map
int ShowInputMenu(std::vector<Sea>& seas) {
    // Создаем карту меню с функциями и описаниями
    const std::map<int, MenuInfo> menu_options = {
        {1, { [&seas]() { InputFromKeyboard(seas); }, "Ввод с клавиатуры" }},
        {2, { [&seas]() { InputFromFile(seas); }, "Чтение из файла (seas.txt)" }}
    };

    std::cout << "\nВыберите способ ввода данных:" << std::endl;

    // Выводим все пункты меню
    for (const auto& option : menu_options) {
        std::cout << option.first << " - " << option.second.description << std::endl;
    }

    std::cout << "Ваш выбор: ";

    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Проверяем, существует ли выбранный пункт меню
    auto it = menu_options.find(choice);
    if (it != menu_options.end()) {
        // Вызываем соответствующую функцию
        it->second.callback();
        return choice;
    }
    else {
        std::cout << "Неверный выбор!" << std::endl;
        return -1;
    }
}

// Основная функция программы
int main() {
    SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
    SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода
    setlocale(LC_ALL, "ru_RU");

    std::vector<Sea> seas;  // Вектор для хранения данных о морях

    // Настройка вывода дробных чисел
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "=== ПРОГРАММА ДЛЯ РАБОТЫ С ДАННЫМИ О МОРЯХ ===" << std::endl;

    // Меню выбора способа ввода данных с использованием map
    int result = ShowInputMenu(seas);

    if (result == -1 || seas.empty()) {
        std::cout << "Нет данных для обработки." << std::endl;
        return 1;
    }

    // Вывод исходной таблицы всех морей
    PrintAllSeas(seas);

    // Сортировка и вывод отсортированной таблицы
    PrintSortedSeas(seas);

    // Поиск по солёности
    SearchBySalinity(seas);

    // Поиск и вывод самого глубокого моря
    PrintDeepestSea(seas);

    // Поиск и вывод самого пресного моря
    PrintLeastSaltySea(seas);

    // Вывод статистики
    PrintStatistics(seas);

    std::cout << "\nПрограмма завершена успешно!" << std::endl;
    return 0;
}