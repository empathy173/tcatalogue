#pragma once

#include <string>
#include <vector>
#include <deque>
#include "geo.h"
#include <unordered_map>
#include <set>
namespace transport {



    struct Stop {
        std::string name;
        coord::Coordinates coordinates;
    };

    struct Bus {
        std::string name;
        std::vector<Stop *> stops;
    };

    struct Hasher {
        std::size_t operator()(const std::pair<Stop*, Stop*>& stops) const {
            std::size_t h1 = std::hash<Stop*>()(stops.first);
            std::size_t h2 = std::hash<Stop*>()(stops.second);
            return h1 ^ (h2 * 31);
        }
    };


    class Catalogue {
    public:


        void AddBus(const Bus& us);

        void AddStop(const Stop& stop);


        Bus *GetBus(std::string_view name) const;

        Stop *GetStop(std::string_view name);

        std::vector<std::string_view> GetBusesForStop(const std::string_view stop_name);

        size_t GetDistance(const Stop* first_stop, const Stop* second_stop);

        void SetDistance(const Stop* first_stop, const Stop* second_stop, size_t distance);



    private:

        std::deque<Bus> buses_;
        std::deque<Stop> stops_;
        std::unordered_map<std::string_view, Bus *> busname_to_bus_;
        std::unordered_map<std::string_view, Stop *> stopname_to_stop_;
        std::unordered_map<std::string_view, std::set<Bus*>> stop_to_buses_;
        std::unordered_map<std::pair<Stop*, Stop*>, size_t, Hasher> stop_to_stop_distance_;
    };
    namespace detail {

        int GetUniqueStopsNum(const Bus &);

        int GetStopsNum(const Bus &);

        double GetRouteLenghtFirst(const Bus &bus);



        double GetRouteLenght(Catalogue catalogue, const std::vector<Stop*>& stops);
    }

}