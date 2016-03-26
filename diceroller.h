#pragma once

#include <random>
#include <vector>

class DiceRoller {
public:
    virtual int roll(const int sides) const = 0;
};

class RandomDiceRoller : public DiceRoller {
public:
    RandomDiceRoller();

    int roll(const int sides) const override;

private:
    mutable std::mt19937 generator;
};

class PredeterminedDiceRoller : public DiceRoller {
public:
    PredeterminedDiceRoller(std::vector<int> rolls);

    int roll(const int sides) const override;

private:
    std::vector<int> rolls;
    mutable decltype(rolls)::const_iterator nextRoll;
};
