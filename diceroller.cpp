#include "diceroller.h"

#include <cassert>

RandomDiceRoller::RandomDiceRoller()
    : generator(std::mt19937(std::random_device()()))
{
}

int RandomDiceRoller::roll(const int sides) const
{
    return std::uniform_int_distribution<>(1, sides)(generator);
}

PredeterminedDiceRoller::PredeterminedDiceRoller(std::vector<int> rolls)
    : rolls(std::move(rolls))
    , nextRoll(this->rolls.cbegin())
{
    assert(this->rolls.size() > 0);
}

int PredeterminedDiceRoller::roll(const int sides) const
{
    const auto result = *(nextRoll++) % sides;
    if (rolls.cend() == nextRoll) {
        nextRoll = rolls.cbegin();
    }
    return result;
}
