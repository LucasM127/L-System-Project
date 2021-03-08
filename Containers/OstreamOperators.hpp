#ifndef OSTREAM_OPERATORS_HPP
#define OSTREAM_OPERATORS_HPP

#include <ostream>
#include "LSFileData.hpp"
#include "ProductionData.hpp"

std::ostream& operator<<(std::ostream &os, const LSYSTEM::LSFileData &fd);

std::ostream& operator<<(std::ostream &os, const LSYSTEM::ProductionData &pd);

std::ostream& operator<<(std::ostream &os, const LSYSTEM::ProductData &pd);

std::ostream& operator<<(std::ostream &os, const LSYSTEM::LSystemData &lsd);

#endif //OSTREAM_OPERATORS_HPP