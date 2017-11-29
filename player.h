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
    QString name; // B
    QString nation;
    int ID;
    int index;
    int overall; // 综合能力
    int potential; // 球员潜力
    int scoreNumber; // 生涯进球
    bool is_captain; // 是否队长
    bool is_vice_captain; // 是否副队长
    PlayerRole role; // 角色
    bool can_play[4]; //能否胜任角色
    static QString getRoleString(PlayerRole role);
    int getPower() const;
private:
    static std::map<PlayerRole, QString> role_string;
};

#endif // PLAYER_H
