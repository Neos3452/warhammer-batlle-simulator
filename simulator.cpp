#include "simulator.h"

#include <cassert>
#include <numeric>

Simulator::Simulator(const std::atomic<bool> &runningAllowed, std::vector<Character> chars, std::unique_ptr<DiceRoller>&& roller)
    : result(true)
    , roundCount(0)
    , runningAllowed(runningAllowed)
    , dice(std::move(roller))
    , characters(std::move(chars))
    , alive(characters.begin(), characters.end())
{
    guys = std::accumulate(alive.cbegin(), alive.cend(), std::pair<int, int>(0, 0), [](std::pair<int, int> guys, const Character &ch) {
        if (ch.isGood()) {
            ++guys.first;
        } else {
            ++guys.second;
        }
        return guys;
    });
}

bool Simulator::pickTarget(Character &ch)
{
    // TODO: Don't walk to close target
    if (ch.currentAttackingWeapon() != Character::AttackingWeapon::Ranged && !ch.walk()) {
        return false;
    }

    // randomly pick target
    int targetIdx = dice->roll(ch.isGood() ? guys.second : guys.first);
    const auto it = std::find_if_not(alive.cbegin(), alive.cend(), [&](const Character &target) {
        return ch.isGood() == target.isGood() || --targetIdx;
    });
    assert(it != alive.cend());
    ch.target = std::addressof(it->get());
    return true;
}

bool Simulator::simulate()
{
    // sort by initative
    std::sort(alive.begin(), alive.end(), [](const Character &first, const Character &second) { return first.initative() < second.initative(); });

    while (guys.first > 0 && guys.second > 0) {
        // runningAllowed will be changed from the other thread so we cannot check it at the end of function
        if (!runningAllowed) {
            result = guys.first > 0;
            return false;
        }

        ++roundCount;

        for (Character &ch : alive) {
            ch.newTurn();
        }

        for (Character &ch : alive) {
            if ((!ch.target || !ch.target->isAlive()) && !pickTarget(ch)) {
                continue;
            }

            assert(ch.target);

            if (!ch.loadWeapon()) {
                continue;
            }

            if (ch.numberOfAttacks() == 1 && ch.remainingActions() > 1) {
                ch.focusAttack();
            }

            // we have a target lets hit it
            if (ch.attack(*dice)) {
                if (!ch.target->isAlive()) {
                    if (ch.target->isGood()) {
                        --guys.first;
                    } else {
                        --guys.second;
                    }
                }

                if (guys.first <= 0 || guys.second <= 0) {
                    break;
                }

                if (!ch.target->isAlive()) {
                    pickTarget(ch);
                }

                ch.loadWeapon();
            }
        }

        std::remove_if(alive.begin(), alive.end(), [](const Character &ch) { return !ch.isAlive(); });
    }

    result = guys.first > 0;
    return true;
}
