#pragma once

#include "character.h"
#include "diceroller.h"

#include <atomic>
#include <memory>
#include <vector>

class Simulator {
public:
    Simulator(const std::atomic<bool> &runningAllowed, std::vector<Character> chars, std::unique_ptr<DiceRoller>&& = std::make_unique<RandomDiceRoller>());

    Simulator(Simulator&&) = default;

    bool simulate();
    bool goodWon() const { return result; }
    int rounds() const { return roundCount; }
    int goodAlive() const { return guys.first; }

private:
    bool pickTarget(Character &);

    bool result;
    int roundCount;
    const std::atomic<bool> &runningAllowed;
    std::unique_ptr<DiceRoller> dice;
    std::vector<Character> characters;
    std::vector<std::reference_wrapper<Character>> alive;
    // first - good, second - bad
    std::pair<int, int> guys;
};
