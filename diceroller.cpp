#include "diceroller.h"

DiceRoller::DiceRoller()
    : generator(std::mt19937(std::random_device()()))
{
}

int DiceRoller::roll(const int sides) const
{
    return std::uniform_int_distribution<>(1, sides)(generator);
}
