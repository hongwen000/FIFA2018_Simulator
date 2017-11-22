#include "player.h"

QString playerNameEx(const Player* player){
    QString ret = player->name;
    if(player->is_captain)
        ret += " (Captain), ";
    if(player->is_vice_captain)
        ret += " (Vice-Captain), ";
    return ret;
}


std::map<PlayerRole, QString> Player::role_string = {{PlayerRole::GK, "GK"}, {PlayerRole::DF, "DF"},
                                                    {PlayerRole::MF, "MF"}, {PlayerRole::FW, "FW"}};

QString Player::getRoleString(PlayerRole role) {
    return role_string[role];
}
