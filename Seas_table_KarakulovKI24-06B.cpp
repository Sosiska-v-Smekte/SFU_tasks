#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <fstream>
#include <algorithm>
#include <cmath>
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
int FindDeepestSeaIndex(const Sea seas[], int sea_count) {
    if (sea_count <= 0) return -1;

    int max_index = 0;
    for (int i = 1; i < sea_count; ++i) {
        if (seas[i].depth > seas[max_index].depth) {
            max_index = i;
        }
    }
    return max_index;
}

// Функция для поиска индекса моря с минимальной солёностью
int FindLeastSaltySeaIndex(const Sea seas[], int sea_count) {
    if (sea_count <= 0) return -1;

    int min_index = 0;
    for (int i = 1; i < sea_count; ++i) {
        if (seas[i].salinity < seas[min_index].salinity) {
            min_index = i;
        }
    }
    return min_index;
}

// Функция для вычисления средней глубины
double CalculateAverageDepth(const Sea seas[], int sea_count) {
    if (sea_count <= 0) return 0.0;

    double sum = 0.0;
    for (int i = 0; i < sea_count; ++i) {
        sum += seas[i].depth;
    }
    return sum / sea_count;
}

// Функция для сортировки морей по глубине (по убыванию)
void SortSeasByDepth(Sea seas[], int sea_count) {
    std::sort(seas, seas + sea_count, [](const Sea& a, const Sea& b) {
        return a.depth > b.depth; // Сортировка по убыванию глубины
        });
}

// Функция для поиска морей по солёности
void FindSeasBySalinity(const Sea seas[], int sea_count, double target_salinity, double tolerance = 0.1) {
    std::cout << "\n=== МОРЯ С СОЛЁНОСТЬЮ " << target_salinity << "‰ (±" << tolerance << "‰) ===" << std::endl;
    std::cout << "+----------------------+------------+--------------+" << std::endl;

    bool found = false;
    for (int i = 0; i < sea_count; ++i) {
        if (std::abs(seas[i].salinity - target_salinity) <= tolerance) {
            PrintSeaData(seas[i]);
            found = true;
        }
    }

    if (!found) {
        std::cout << "| Моря с указанной солёностью не найдены. |" << std::endl;
    }
    std::cout << "+----------------------+------------+--------------+" << std::endl;
}

// Функция для чтения данных из файла
int ReadSeasFromFile(const std::string& filename, Sea seas[]) {
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

            seas[count] = sea;
            count++;
        }
    }

    file.close();
    return count;
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

// Основная функция программы
int main() {
    SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
    SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода
    setlocale(LC_ALL, "ru_RU");

    Sea seas[kMaxSeas];  // Массив для хранения данных о морях
    int sea_count = 0;   // Фактическое количество морей

    // Настройка вывода дробных чисел
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "=== ПРОГРАММА ДЛЯ РАБОТЫ С ДАННЫМИ О МОРЯХ ===" << std::endl;

    // Меню выбора способа ввода данных
    std::cout << "\nВыберите способ ввода данных:" << std::endl;
    std::cout << "1 - Ввод с клавиатуры" << std::endl;
    std::cout << "2 - Чтение из файла (seas.txt)" << std::endl;
    std::cout << "Ваш выбор: ";

    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice == 1) {
        // Ввод количества морей
        std::cout << "\nВведите количество морей (1-" << kMaxSeas << "): ";
        std::cin >> sea_count;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Проверка корректности введённого количества
        if (sea_count < 1 || sea_count > kMaxSeas) {
            std::cout << "Ошибка! Количество морей должно быть от 1 до " << kMaxSeas << std::endl;
            return 1;
        }

        // Ввод данных о морях
        std::cout << "\n=== ВВОД ДАННЫХ О МОРЯХ ===" << std::endl;
        for (int i = 0; i < sea_count; ++i) {
            std::cout << "\nМоре #" << (i + 1) << ":" << std::endl;
            InputSeaData(seas[i]);
        }
    }
    else if (choice == 2) {
        // Чтение данных из файла
        sea_count = ReadSeasFromFile("seas.txt", seas);
        if (sea_count == 0) {
            std::cout << "Не удалось прочитать данные из файла." << std::endl;
            return 1;
        }
        std::cout << "Успешно прочитано " << sea_count << " морей из файла." << std::endl;
    }
    else {
        std::cout << "Неверный выбор!" << std::endl;
        return 1;
    }

    // Вывод исходной таблицы всех морей
    PrintAllSeas(seas, sea_count);

    // Сортировка по глубине
    SortSeasByDepth(seas, sea_count);
    std::cout << "\n=== ТАБЛИЦА МОРЕЙ (ОТСОРТИРОВАННАЯ ПО ГЛУБИНЕ) ===" << std::endl;
    std::cout << "+----------------------+------------+--------------+" << std::endl;
    std::cout << "| Название             | Глубина (м)| Солёность (‰)|" << std::endl;
    std::cout << "+----------------------+------------+--------------+" << std::endl;
    for (int i = 0; i < sea_count; ++i) {
        PrintSeaData(seas[i]);
    }
    std::cout << "+----------------------+------------+--------------+" << std::endl;

    // Поиск по солёности
    double target_salinity;
    std::cout << "\n=== ПОИСК ПО СОЛЁНОСТИ ===" << std::endl;
    std::cout << "Введите солёность для поиска: ";
    std::cin >> target_salinity;
    FindSeasBySalinity(seas, sea_count, target_salinity);

    // Поиск и вывод самого глубокого моря
    int deepest_index = FindDeepestSeaIndex(seas, sea_count);
    if (deepest_index != -1) {
        std::cout << "\n+ САМОЕ ГЛУБОКОЕ МОРЕ -+------------+--------------+" << std::endl;
        PrintSeaData(seas[deepest_index]);
    }

    // Поиск и вывод самого пресного моря
    int least_salty_index = FindLeastSaltySeaIndex(seas, sea_count);
    if (least_salty_index != -1) {
        std::cout << "+- САМОЕ ПРЕСНОЕ МОРЕ -+------------+--------------+" << std::endl;
        PrintSeaData(seas[least_salty_index]);
        std::cout << "+----------------------+------------+--------------+" << std::endl;
    }

    // Вывод статистики
    double average_depth = CalculateAverageDepth(seas, sea_count);
    std::cout << "\n=== СТАТИСТИКА ===" << std::endl;
    std::cout << "Средняя глубина всех морей: " << average_depth << " м" << std::endl;

    // Поиск морей глубже средней глубины
    std::cout << "\n=== МОРЯ ГЛУБЖЕ СРЕДНЕЙ ГЛУБИНЫ ===" << std::endl;
    std::cout << "+----------------------+------------+--------------+" << std::endl;
    bool found_deeper = false;
    for (int i = 0; i < sea_count; ++i) {
        if (seas[i].depth > average_depth) {
            PrintSeaData(seas[i]);
            found_deeper = true;
        }
    }
    if (!found_deeper) {
        std::cout << "| Моря глубже средней глубины не найдены. |" << std::endl;
    }
    std::cout << "+----------------------+------------+--------------+" << std::endl;

    std::cout << "\nПрограмма завершена успешно!" << std::endl;
    return 0;
}