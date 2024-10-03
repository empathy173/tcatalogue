#include <unordered_set>
#include <iomanip>
#include <algorithm>
#include <vector>
#include "transport_catalogue.h"

void ParseAndPrintStat(transport::Catalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {
    // Определим тип запроса: Bus или Stop
    if (request.substr(0, 3) == "Bus") {
        std::string_view bus_name = request.substr(4);
        const transport::Bus* bus = transport_catalogue.GetBus(bus_name);

        if (bus == nullptr) {
            output << "Bus " << bus_name << ": not found\n";
            return;
        }

        int total_stops = bus->stops.size();
        std::unordered_set<const transport::Stop*> unique_stops(bus->stops.begin(), bus->stops.end());
        int unique_stop_count = unique_stops.size();

        double route_length = transport::detail::GetRouteLenght(transport_catalogue, bus->stops);
        double geo_length = transport::detail::GetRouteLenghtFirst(*bus);

        double curvature = route_length / geo_length;

        output << "Bus " << bus_name << ": "
               << total_stops << " stops on route, "
               << unique_stop_count << " unique stops, "
               << std::fixed << std::setprecision(0) << route_length << " route length, "
               << std::fixed << std::setprecision(4) << curvature << " curvature\n";
    }
    else if (request.substr(0, 4) == "Stop") {
        std::string_view stop_name = request.substr(5);
        std::vector<std::string_view> buses = transport_catalogue.GetBusesForStop(stop_name);

        if (buses.empty()) {
            const transport::Stop* stop = transport_catalogue.GetStop(stop_name);
            if (stop == nullptr) {
                output << "Stop " << stop_name << ": not found\n";
            } else {
                output << "Stop " << stop_name << ": no buses\n";
            }
        } else {
            output << "Stop " << stop_name << ": buses";
            for (const auto& bus : buses) {
                output << " " << bus;
            }
            output << "\n";
        }
    }
}
