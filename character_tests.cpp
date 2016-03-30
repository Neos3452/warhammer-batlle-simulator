#include <gtest/gtest.h>

#include "character.h"
#include "diceroller.h"

TEST(Character, NoToughnessNoArmorKillingBlow)
{
    constexpr const auto health = 5;
    Character c(false, "dummy", 0, 0, 0, 0, 0, 0, health, 0 ,0 ,0, false);
    EXPECT_TRUE(c.isAlive());
    c.hit(health);
    EXPECT_FALSE(c.isAlive());
}

TEST(Character, ArmorPreventsKillingBlow)
{
    constexpr const auto health = 5;
    constexpr const auto armor = 1;
    Character c(false, "dummy", 0, 0, 0, 0, 0, 0, health, 0 ,0 , armor, false);
    EXPECT_TRUE(c.isAlive());
    c.hit(health);
    EXPECT_TRUE(c.isAlive());
    EXPECT_EQ(c.health(), armor);
    c.hit(2*armor);
    EXPECT_FALSE(c.isAlive());
}

TEST(Character, ToughnessPreventsKillingBlow)
{
    constexpr const auto health = 5;
    constexpr const auto toughness = 38;
    Character c(false, "dummy", 0, 0, 0, toughness, 0, 0, health, 0 ,0 , 0, false);
    EXPECT_TRUE(c.isAlive());
    c.hit(health);
    EXPECT_TRUE(c.isAlive());
    EXPECT_EQ(c.health(), toughness / 10);
    c.hit(2*(toughness / 10));
    EXPECT_FALSE(c.isAlive());
    EXPECT_EQ(c.health(), 0);
}

TEST(Character, HitRoll)
{
    constexpr const auto health = 6;
    Character dummy(false, "dummy", 0, 0, 0, 0, 0, 0, health, 0 ,0 , 0, false);
    EXPECT_TRUE(dummy.isAlive());

    constexpr const auto ws = 50;
    constexpr const auto str = 20; // with dmg roll 2 hits to kill
    constexpr const auto attacks = 1;
    Character trainee(true, "trainee", ws, 0, str, 0, 0, attacks, health, 0, 0, 0, false);
    EXPECT_EQ(trainee.currentAttackingWeapon(), Character::AttackingWeapon::Melee);
    EXPECT_EQ(trainee.remainingActions(), 2);
    trainee.target = &dummy;

    PredeterminedDiceRoller roller({30 /* hit */, 100 /* parry */, 1 /* dmg */});

    EXPECT_TRUE(trainee.attack(roller));
    EXPECT_EQ(trainee.remainingActions(), 1);
    EXPECT_TRUE(dummy.isAlive());
    EXPECT_EQ(dummy.health(), health - (str / 10) - 1);

    EXPECT_FALSE(trainee.attack(roller)); // 1 attack per turn
    EXPECT_EQ(trainee.remainingActions(), 1);
    dummy.newTurn();
    trainee.newTurn();
    EXPECT_TRUE(trainee.attack(roller));
    EXPECT_EQ(trainee.remainingActions(), 1);
    EXPECT_FALSE(dummy.isAlive());
    EXPECT_EQ(dummy.health(), 0);
}

TEST(Character, MultipleAttacks)
{
    constexpr const auto health = 6;
    Character dummy(false, "dummy", 0, 0, 0, 0, 0, 0, health, 0 ,0 , 0, false);
    EXPECT_TRUE(dummy.isAlive());

    constexpr const auto ws = 50;
    constexpr const auto str = 20; // with dmg roll 2 hits to kill
    constexpr const auto attacks = 2;
    Character trainee(true, "trainee", ws, 0, str, 0, 0, attacks, health, 0, 0, 0, false);
    EXPECT_EQ(trainee.currentAttackingWeapon(), Character::AttackingWeapon::Melee);
    EXPECT_EQ(trainee.remainingActions(), 2);
    trainee.target = &dummy;

    PredeterminedDiceRoller roller({30 /* hit */, 100 /* parry */, 1 /* dmg */, 30 /* another hit */, 1 /* dmg */});
    EXPECT_TRUE(trainee.attack(roller));
    EXPECT_FALSE(dummy.isAlive());
    EXPECT_EQ(dummy.health(), 0);
    EXPECT_EQ(trainee.remainingActions(), 0);
}

TEST(Character, CannotParryTwice)
{
    constexpr const auto health = 6;
    Character dummy(false, "dummy", 0, 0, 0, 0, 0, 0, health, 0 ,0 , 0, false);
    EXPECT_TRUE(dummy.isAlive());
    EXPECT_TRUE(dummy.canParry());

    constexpr const auto ws = 50;
    constexpr const auto str = 20; // with dmg roll 2 hits to kill
    constexpr const auto attacks = 1;
    Character trainee(true, "trainee", ws, 0, str, 0, 0, attacks, health, 0, 0, 0, false);
    EXPECT_EQ(trainee.currentAttackingWeapon(), Character::AttackingWeapon::Melee);
    EXPECT_EQ(trainee.remainingActions(), 2);
    trainee.target = &dummy;

    PredeterminedDiceRoller roller({30 /* hit */, 100 /* parry */, 1 /* dmg */, 30 /* hit */, 1 /* dmg */});

    EXPECT_TRUE(trainee.attack(roller));
    EXPECT_EQ(trainee.remainingActions(), 1);
    EXPECT_TRUE(dummy.isAlive());
    EXPECT_FALSE(dummy.canParry());
    EXPECT_EQ(dummy.health(), health - (str / 10) - 1);
    // no new turn for dummy
    trainee.newTurn();
    EXPECT_TRUE(trainee.attack(roller));
    EXPECT_EQ(trainee.remainingActions(), 1);
    EXPECT_FALSE(dummy.isAlive());
    EXPECT_FALSE(dummy.canParry());
    EXPECT_EQ(dummy.health(), 0);
}
