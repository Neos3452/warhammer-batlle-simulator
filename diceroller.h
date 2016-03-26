#pragma once

#include <random>

class DiceRoller {
public:
    DiceRoller();

    int roll(const int sides) const;

private:
    mutable std::mt19937 generator;
};
