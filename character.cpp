#include "character.h"

#include "diceroller.h"

#include <cassert>
#include <QDebug>

constexpr const unsigned Character::kActionsPerRound;

Character::Character(bool good, QString name,
    int weaponSkill, int ballisticSkill,
    int strength, int toughness, int agility,
    int attacks, int wounds, int meleeWeapon,
    int rangedWeapon, int armor, bool hasEvadeSkill)
    : good(good),
      hasEvadeSkill(hasEvadeSkill),
      weaponSkill(weaponSkill),
      ballisticSkill(ballisticSkill),
      strength(strength),
      toughness(toughness),
      agility(agility),
      attacks(attacks),
      meleeWeapon(meleeWeapon),
      rangedWeapon(rangedWeapon),
      armor(armor),
      name(name),
      wounds(std::max(wounds, 0)),
      currentHp(this->wounds),
      target(nullptr)
{
    newTurn();
}

bool Character::attack(const DiceRoller& dice)
{
    assert(target != nullptr);

    // in case of range weapon we do not have to walk to the target so we can load instead and now attack
    // TODO: load if not walking
    // attacking action
    if (!useAction()) {
        return false;
    }

    // always attack with better skill
    auto skill = std::max(ballisticSkill > weaponSkill ? ballisticSkill : weaponSkill, 1);

    // aim if possible
    if (useAction()) {
        skill += 10;
    }

    skill = std::min(skill, 99);

    // hit enemy with all attacks
    for (int attackCounter = attacks; attackCounter > 0; --attackCounter) {
        const auto roll = dice.roll(100);
        if (roll <= skill && !target->parry(dice) && !target->evade(dice)) {
            constexpr const auto kDmgRoll = 10;
            auto dmg = dice.roll(kDmgRoll);
            if (kDmgRoll == dmg) {
                // crit
                const auto roll = dice.roll(100);
                if (roll <= skill) {
                    do {
                        dmg += dice.roll(kDmgRoll);
                    } while (0 == (dmg % kDmgRoll)); // while rolled crit roll another
                }
            }
            // weapon
            dmg += ballisticSkill > weaponSkill ? rangedWeapon : ((strength/10) + meleeWeapon);
            target->hit(dmg);
        }
    }
    return true;
}

bool Character::parry(const DiceRoller &dice)
{
    if (!didParry) {
        didParry = true;
        return dice.roll(100) < weaponSkill;
    }
    return false;
}

bool Character::evade(const DiceRoller &dice)
{
    if (!didEvade) {
        didEvade = true;
        return dice.roll(100) < agility;
    }
    return false;
}

bool Character::walk()
{
    return useAction();
}

bool Character::useAction(int complexity)
{
    if (actionsRemaining > 0) {
        actionsRemaining -= complexity;
    }
    return actionsRemaining >= 0;
}

void Character::newTurn()
{
    actionsRemaining = kActionsPerRound;
    didParry = false;
    didEvade = !hasEvadeSkill;
}

void Character::hit(int dmg)
{
//    qDebug() << name << " hit " << dmg;
    currentHp -= std::max((dmg - ((toughness/10) + armor)), 0);
}
