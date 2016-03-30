#pragma once

#include <ostream>
#include <string>

#include <QString>

class DiceRoller;

class Character {
public:
    enum class AttackingWeapon {
        Melee,
        Ranged,
    };

    Character(bool good, QString name,
        int weaponSkill,
        int ballisticSkill,
        int strength,
        int toughness,
        int agility,
        int attacks,
        int wounds,
        int meleeWeapon,
        int rangedWeapon,
        int armor,
        bool hasEvadeSkill);

    bool changeAttackingWeapon(AttackingWeapon);
    bool loadWeapon();
    bool focusAttack();
    bool attack(const DiceRoller&);
    bool parry(const DiceRoller&);
    bool evade(const DiceRoller&);
    bool walk();
    bool useAction(int complexity = 1);

    void newTurn();
    void hit(int dmg);

    int numberOfAttacks() const { return attacks; }
    int remainingActions() const { return actionsRemaining; }
    int initative() const { return agility; }
    bool isGood() const { return good; }
    bool isAlive() const { return currentHp > 0; }
    bool isWeaponLoaded() const { return weaponLoaded; }
    AttackingWeapon currentAttackingWeapon() const { return attackingWeapon; }
    int health() const { return currentHp; }
    bool canParry() const { return !didParry; }

private:
    static constexpr const unsigned kActionsPerRound = 2;

    friend std::ostream & operator <<(std::ostream &out, const Character &character);

    // stats
    bool good;
    bool hasEvadeSkill;
    int weaponSkill;
    int ballisticSkill;
    int strength;
    int toughness;
    int agility;
    int attacks;
    int meleeWeapon;
    int rangedWeapon;
    int armor;
    QString name;

    // health
    int wounds;
    int currentHp;

    // state
    AttackingWeapon attackingWeapon;
    bool weaponLoaded;
    int actionsRemaining;
    bool didParry;
    bool didEvade;
    bool isFocused;
    bool didAttack;

public:
    Character *target;
};

inline std::ostream & operator <<(std::ostream &out, const Character &character) {
    out << "Char:{" << (character.good ? "Good" : "Evil") << " " << character.name.toUtf8().constData()
        << " " << character.currentHp << "/" << character.wounds << " HP"
        << " A: " << character.actionsRemaining << "/" << Character::kActionsPerRound;
    if (!character.didParry || !character.didEvade) {
        out << " ";
        if (!character.didParry) {
            out << "P";
        }
        if (!character.didEvade) {
            out << "E";
        }
    }
    out << ", WS:" << character.weaponSkill
        << ", BS:" << character.ballisticSkill
        << ", Str:" << character.strength
        << ", Tgh:" << character.toughness
        << ", Agi:" << character.agility
        << ", Att:" << character.attacks
        << ", MWpn:" << character.meleeWeapon
        << ", RWpn:" << character.rangedWeapon
        << ", Armor:" << character.armor;

    if (character.hasEvadeSkill) {
        out << ", Evade";
    }
    out << "}";
    return out;
}
