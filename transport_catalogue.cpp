#include <unordered_set>
#include <algorithm>
#include "transport_catalogue.h"

void transport::Catalogue::AddBus(const Bus& bus) {
    buses_.push_back(std::move(bus));
    Bus* new_bus = &buses_.back();
    busname_to_bus_[new_bus->name] = new_bus;

    for (const auto& stop : new_bus->stops) {
        stop_to_buses_[stop->name].insert(new_bus);  // Изменение здесь
    }
}


void transport::Catalogue::AddStop(const Stop& stop) {
    stops_.push_back(std::move(stop));
    stopname_to_stop_[stops_.back().name] = &stops_.back();
}

transport::Bus* transport::Catalogue::GetBus(std::string_view name) const{
    auto it = busname_to_bus_.find(name);
    if (it != busname_to_bus_.end()) {
        return it->second;
    }
    return nullptr;
}

transport::Stop* transport::Catalogue::GetStop(std::string_view name) {

    auto it = stopname_to_stop_.find(name);
    if (it != stopname_to_stop_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string_view> transport::Catalogue::GetBusesForStop(const std::string_view stop_name) {
    std::vector<std::string_view> buses;
    auto it = stop_to_buses_.find(stop_name);
    if (it != stop_to_buses_.end()) {
        for (const auto& bus : it->second) {
            buses.push_back(bus->name);
        }
    }
    return buses;
}


double transport::detail::GetRouteLenghtFirst(const Bus& bus){
    double result = 0.0;
    if (bus.stops.empty() || bus.stops.size() == 1) {
        return result;
    }

    const Stop* prev_stop = bus.stops[0];
    for (size_t i = 1; i < bus.stops.size(); ++i) {
       const  Stop* current_stop = bus.stops[i];
        result += ComputeDistance(prev_stop->coordinates, current_stop->coordinates);
        prev_stop = current_stop;
    }

    return result;
}

int transport::detail::GetUniqueStopsNum(const transport::Bus& bus){
    std::unordered_set<transport::Stop*> unique_stops(bus.stops.begin(), bus.stops.end());
    return unique_stops.size();
}

int transport::detail::GetStopsNum(const Bus& bus){
        return bus.stops.size();
}

size_t transport::Catalogue::GetDistance(const Stop* first_stop, const Stop* second_stop) {
    if (first_stop == nullptr || second_stop == nullptr) {
        return 0;
    }

    std::pair<Stop*, Stop*> key = std::make_pair(const_cast<Stop*>(first_stop), const_cast<Stop*>(second_stop));
    auto it = stop_to_stop_distance_.find(key);
    if (it != stop_to_stop_distance_.end()) {
        return it->second;
    }

    auto reverse_key = std::make_pair(const_cast<Stop*>(second_stop), const_cast<Stop*>(first_stop));
    it = stop_to_stop_distance_.find(reverse_key);
    if (it != stop_to_stop_distance_.end()) {
        return it->second;
    }

    return 0;
}


void transport::Catalogue::SetDistance(const Stop* first_stop, const Stop* second_stop, size_t distance) {
    if (first_stop == nullptr || second_stop == nullptr) {
        return;
    }
    stop_to_stop_distance_.emplace(std::make_pair(const_cast<Stop*>(first_stop), const_cast<Stop*>(second_stop)), distance).first->second = distance;
}


double transport::detail::GetRouteLenght(Catalogue catalogue, const std::vector<Stop*>& stops) {
    double result = 0.0;
    for (size_t i = 0; i < stops.size() - 1; ++i) {
        result += catalogue.GetDistance(stops[i], stops[i + 1]);
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const transport::Bus& bus) {
    os << bus.name;
    return os;
}