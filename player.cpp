#include "player.h"
using std::min;

QString playerNameEx(const Player* player){
    QString ret = player->name;
    if(player->is_captain)
        ret += " (Captain), ";
    if(player->is_vice_captain)
        ret += " (Vice-Captain), ";
    return ret;
}

QString Player::getRoleString(PlayerRole role) {
    if(role == PlayerRole::GK) return "GK";
    else if (role == PlayerRole::DF) return "DF";
    else if (role == PlayerRole::MF) return "MF";
    else return "FW";
}

int Player::getPower() const {
    int tmp = RandLib::uniform_rand(0, 100);
    double ans = overall + potential * tmp / 10000.0;
    return min(ans, 100.0);
}
