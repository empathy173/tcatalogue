#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

void ParseAndPrintStat(transport::Catalogue& transport_catalogue, std::string_view request,
                       std::ostream& output);