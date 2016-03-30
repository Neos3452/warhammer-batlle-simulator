#include <gtest/gtest.h>

#include "character.h"
#include "diceroller.h"
#include "simulator.h"

#include <memory>
#include <vector>

TEST(Simulator, SortByInitiative)
{
    const bool running = true;
    std::vector<Character> chars;
    constexpr const auto health = 5;
    chars.emplace_back(false, "trainee1", 50, 0, 0, 0, 20, 1, health, 0 ,0 ,0, false);
    // trainee 2 should go first
    chars.emplace_back(true, "trainee2", 50, 0, 0, 0, 30, 1, health, 0 ,0 ,0, false);

    Simulator sim(running, chars, std::make_unique<PredeterminedDiceRoller>(std::vector<int>{30, 100, 5}));
    EXPECT_TRUE(sim.simulate());
    EXPECT_TRUE(sim.goodWon());
    EXPECT_EQ(sim.goodAlive(), 1);
    EXPECT_EQ(sim.rounds(), 1);
}
