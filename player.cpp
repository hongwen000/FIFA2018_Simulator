#include "player.h"

QString playerNameEx(const Player* player){
    QString ret = player->name;
    if(player->is_captain)
        ret += " (Captain), ";
    if(player->is_vice_captain)
        ret += " (Vice-Captain), ";
    return ret;
}
