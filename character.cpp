#include "character.h"

#include "diceroller.h"

#include <cassert>
#include <QDebug>

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
      attackingWeapon(ballisticSkill >= weaponSkill ? AttackingWeapon::Ranged : AttackingWeapon::Melee),
      weaponLoaded(true), // character starts battle with weapon ready
      target(nullptr)
{
    newTurn();
}

bool Character::changeAttackingWeapon(AttackingWeapon weapon)
{
    if (weapon != attackingWeapon) {
        if (!useAction()) {
            return false;
        }
        weaponLoaded = false;
        attackingWeapon = weapon;
    }
    return true;
}

bool Character::loadWeapon()
{
    if (attackingWeapon == AttackingWeapon::Ranged) {
        if (!useAction()) {
            return false;
        }
        weaponLoaded = true;
    }
    return true;
}

bool Character::attack(const DiceRoller& dice)
{
    assert(target != nullptr);
    assert(attackingWeapon != AttackingWeapon::Ranged || weaponLoaded);

    // attacking action
    if (!useAction()) {
        return false;
    }

    // always attack with better skill
    auto skill = std::max(attackingWeapon == AttackingWeapon::Ranged ? ballisticSkill : weaponSkill, 1);

    if (isFocused) {
        skill += 10;
    }

    skill = std::min(skill, 99);

    // hit enemy with all attacks
    for (int attackCounter = attacks; attackCounter > 0; --attackCounter) {
        const auto roll = dice.roll(100);
        if (roll <= skill && (attackingWeapon == AttackingWeapon::Ranged
            || (!target->parry(dice) && !target->evade(dice)))) {
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
            dmg += attackingWeapon == AttackingWeapon::Ranged ? rangedWeapon : ((strength/10) + meleeWeapon);
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

bool Character::focusAttack()
{
    if (!isFocused) {
        if (!useAction()) {
            return false;
        }
        isFocused = true;
    }
    return true;
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
    isFocused = false;
}

void Character::hit(int dmg)
{
    currentHp -= std::max((dmg - ((toughness/10) + armor)), 0);
}
