#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <iostream>
/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
/*coord::Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto first_comma = str.find(',');

    if (first_comma == str.npos) {
        return {nan, nan};
    }

    auto not_space_after_comma = str.find_first_not_of(' ', first_comma + 1);
    auto second_comma = str.find(',', not_space_after_comma);

    if (second_comma == str.npos) {
        return {nan, nan};
    }

  //  auto not_space_after_second_comma = str.find_first_not_of(' ', second_comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, first_comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space_after_comma, second_comma - not_space_after_comma)));

    return {lat, lng};
}*/
std::pair<std::string_view, coord::Coordinates> ParseCoordinates(std::string_view str) {
    auto first_comma = str.find(',');
    auto second_comma = str.find(',', first_comma + 1);

    double lat = std::stod(std::string(str.substr(0, first_comma)));
    double lon = std::stod(std::string(str.substr(str.find_first_not_of(' ', first_comma + 1), second_comma - first_comma - 1)));

    return {str.substr(0, first_comma), {lat, lon}};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

std::pair<std::string_view, size_t> ParseDistance(std::string_view data) {
    auto start_pos = data.find_first_not_of(' ');
    auto m_pos = data.find('m', start_pos);

    size_t distance = std::stoull(std::string(data.substr(start_pos, m_pos - start_pos)));

    auto to_pos = data.find(" to ", m_pos + 1);
    std::string_view stop_name = data.substr(data.find_first_not_of(' ', to_pos + 4));

    return {stop_name, distance};
}



input::CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

void input::Reader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

     input::Reader::RequestType input::Reader::GetRequestType(std::string_view request){
        if (request == "Bus") {
            return RequestType::Bus;
        } else if (request == "Stop") {
            return RequestType::Stop;
        } else {
            return RequestType::Unknown;
        }
}

void input::Reader::ApplyCommands(transport::Catalogue& catalogue) const {
    std::vector<const CommandDescription*> stop_commands;

    for (const auto& command : commands_) {
        if (GetRequestType(command.command) == RequestType::Stop) {
            stop_commands.push_back(&command);
            auto [name, coordinates] = ParseCoordinates(command.description);
            transport::Stop stop;
            stop.name = command.id;
            stop.coordinates = coordinates;
            catalogue.AddStop(stop);
        }
    }

    for (const auto* command : stop_commands) {
        std::vector<std::string_view> parts = Split(command->description, ',');
        if (parts.size() > 2) {
            for (size_t i = 2; i < parts.size(); ++i) {
                auto [neighbor_stop_name, distance] = ParseDistance(parts[i]);
                transport::Stop* current_stop = catalogue.GetStop(command->id);
                transport::Stop* neighbor_stop = catalogue.GetStop(neighbor_stop_name);
                if (current_stop && neighbor_stop) {
                    catalogue.SetDistance(current_stop, neighbor_stop, distance);
                }
            }
        }
    }

    for (const auto& command : commands_) {
        if (GetRequestType(command.command) == RequestType::Bus) {
            transport::Bus new_bus;
            new_bus.name = command.id;
            std::vector<std::string_view> stop_names = ParseRoute(command.description);
            for (const auto& stop_name : stop_names) {
                transport::Stop* stop = catalogue.GetStop(stop_name);
                if (stop) {
                    new_bus.stops.push_back(stop);
                }
            }
            catalogue.AddBus(new_bus);
        }
    }
}




