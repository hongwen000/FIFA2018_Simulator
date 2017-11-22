#ifndef PLAYER_H
#define PLAYER_H
#include "base.h"

class Player;

QString playerNameEx(const Player *player);

enum class PlayerRole{
    GK,DF,MF,FW, NOT_ASSIGNED
};

class Player
{
public:
    Player() : scoreNumber(0), is_captain(false), is_vice_captain(false), role(PlayerRole::NOT_ASSIGNED) {}
    QString name;
    QString nation;
    int ID;
    int index;
    int overall;
    int potential;
    int scoreNumber;
    bool is_captain;
    bool is_vice_captain;
    PlayerRole role;
    static QString getRoleString(PlayerRole role);
private:
    static std::map<PlayerRole, QString> role_string;
};

#endif // PLAYER_H
