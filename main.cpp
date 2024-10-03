/*
#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    transport::Catalogue catalogue;

    int base_request_count;
    cin >> base_request_count >> ws;

    {
        input::Reader reader;
        for (int i = 0; i < base_request_count; ++i) {
            string line;
            getline(cin, line);
            reader.ParseLine(line);
        }
        reader.ApplyCommands(catalogue);
    }

    int stat_request_count;
    cin >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(cin, line);
        ParseAndPrintStat(catalogue, line, cout);
    }
}*/

#include <iostream>
#include <string>
#include <fstream> // Подключаем для работы с файлами

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    transport::Catalogue catalogue;

    string input_file = "test_input.txt"; // Имя файла для ввода
    string output_file = "output.txt"; // Имя файла для вывода

    // Открываем файл для чтения
    ifstream input(input_file);
    if (!input.is_open()) {
        cerr << "Could not open the input file: " << input_file << "\n";
        return 1;
    }

    // Читаем количество базовых запросов
    int base_request_count;
    input >> base_request_count >> ws;

    {
        input::Reader reader;
        for (int i = 0; i < base_request_count; ++i) {
            string line;
            getline(input, line);
            reader.ParseLine(line);
        }
        reader.ApplyCommands(catalogue);
    }

    // Читаем количество статистических запросов
    int stat_request_count;
    input >> stat_request_count >> ws;

    // Открываем файл для записи
    ofstream output(output_file);
    if (!output.is_open()) {
        cerr << "Could not open the output file: " << output_file << "\n";
        return 1;
    }

    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(input, line);
        ParseAndPrintStat(catalogue, line, output); // Записываем в выходной файл
    }

    // Закрываем файлы
    input.close();
    output.close();

    return 0;
}
