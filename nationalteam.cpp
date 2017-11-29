#include "nationalteam.h"

QString teamNameEx(const NationalTeam* team) {
    QString ret(team->name);
    ret += (team->is_host ? " (hosts)" : "");
    return ret;
}

//!TODO
std::vector<Player*> NationalTeam::getStarters()
{
    std::vector<Player*> ret;
    getFinalPlayers();
    std::sort(final_players.begin(), final_players.end(), [](auto p1, auto p2) {
        return p1->overall > p2->overall;
    });
    Player* p;
    // GK
    p = nullptr;
    for(auto it : final_players) {
        if(it->role == PlayerRole::GK && (p == nullptr || it->overall > p->overall)) {
            p = it;
        }
    }
    ret.push_back(p);
    // DF
    int cnt = 0;
    for(auto it : final_players) {
        if(it->role == PlayerRole::DF) {
            ret.push_back(it);
            cnt++;
            if(cnt == 4) break;
        }
    }
    //MF
    cnt = 0;
    for(auto it : final_players) {
        if(it->role == PlayerRole::MF) {
            ret.push_back(it);
            cnt++;
            if(cnt == 3) break;
        }
    }

    cnt = 0;
    for(auto it : final_players) {
        if(it->role == PlayerRole::FW) {
            ret.push_back(it);
            cnt++;
            if(cnt == 3) break;
        }
    }
    ret[0]->is_captain = true;
    ret[6]->is_vice_captain = true;
    return ret;
}

void NationalTeam::loadPlayers(QString player_file_name)
{
    QString player_string;
    QFile player_file;
    player_file.setFileName(player_file_name);
    player_file.open(QIODevice::ReadOnly | QIODevice::Text);
    player_string = player_file.readAll();
    QJsonDocument player_d = QJsonDocument::fromJson(player_string.toUtf8());
    QJsonArray player_arr = player_d.array();
    int index = 1;
    for(QJsonArray::iterator it = player_arr.begin(); it != player_arr.end(); ++it) {
        QJsonObject player_data = (*it).toObject();
        Player* player = new Player;
        player->ID = player_data["ID"].toString().toInt();
        player->name = player_data["Fullname"].toString();
        player->nation = this->name;
        player->overall = player_data["Overall"].toString().toInt();
        player->potential = player_data["Potential"].toString().toInt();
        player->index = index;
        QString pos = player_data["Preferred Positions"].toString();
        for(int i = 0; i < 4; ++i) {
            player->can_play[i] = false;
        }
        if(pos.contains(QRegExp("GK"))) {
            player->can_play[0] = true;
        }
        if(pos.contains(QRegExp("(RS|ST|LS|RW|LW|RF|CF|LF)"))) {
            player->can_play[3] = true;
        }
        if (pos.contains(QRegExp("(RAM|CAM|LAM|RCM|CM|LCM|RM|LM|RDM|CDM|LDM)"))) {
            player->can_play[2] = true;
        }
        if (pos.contains(QRegExp("(RCB|CB|LCB|RB|LB|RWB|LWB)"))) {
            player->can_play[1] = true;
        }

        ++index;
        all_players.push_back(player);
    }
    player_file.close();
}

//!TODO
std::vector<Player*> NationalTeam::getFinalPlayers()
{
    if(final_players.empty()) {
        std::sort(all_players.begin(), all_players.end(), [](auto p1, auto p2) {
            return p1->overall + p1->potential / 2 > p2->overall + p2->potential / 2;
        });
        bool used[all_players.size()];
        for(size_t i = 0; i < all_players.size(); ++i) {
            used[i] = false;
        }
        // GK
        int cnt = 0;
        for (auto it = all_players.begin(); it != all_players.end(); ++it) {
            if((*it)->can_play[0] && !(used[std::distance(all_players.begin(), it)])) {
                used[std::distance(all_players.begin(), it)] = true;
                (*it)->role = PlayerRole::GK;
                final_players.push_back(*it);
                cnt++;
                if(cnt == 3) break;
            }
        }
        if(cnt < 3) {
            for (auto it = all_players.begin(); it != all_players.end(); ++it) {
                if(!(used[std::distance(all_players.begin(), it)])) {
                    used[std::distance(all_players.begin(), it)] = true;
                    (*it)->role = PlayerRole::GK;
                    final_players.push_back(*it);
                    cnt++;
                    if(cnt == 3) break;
                }
            }
        }
        // DF
        cnt = 0;
        for (auto it = all_players.begin(); it != all_players.end(); ++it) {
            if((*it)->can_play[1] && !(used[std::distance(all_players.begin(), it)])) {
                used[std::distance(all_players.begin(), it)] = true;
                (*it)->role = PlayerRole::DF;
                final_players.push_back(*it);
                cnt++;
                if(cnt == 8) break;
            }
        }
        if(cnt < 8) {
            for (auto it = all_players.begin(); it != all_players.end(); ++it) {
                if(!(used[std::distance(all_players.begin(), it)])) {
                    used[std::distance(all_players.begin(), it)] = true;
                    (*it)->role = PlayerRole::DF;
                    final_players.push_back(*it);
                    cnt++;
                    if(cnt == 8) break;
                }
            }
        }
         // MF
        cnt = 0;
        for (auto it = all_players.begin(); it != all_players.end(); ++it) {
            if((*it)->can_play[2] && !(used[std::distance(all_players.begin(), it)])) {
                used[std::distance(all_players.begin(), it)] = true;
                (*it)->role = PlayerRole::MF;
                final_players.push_back(*it);
                cnt++;
                if(cnt == 6) break;
            }
        }
        if(cnt < 6) {
            for (auto it = all_players.begin(); it != all_players.end(); ++it) {
                if(!(used[std::distance(all_players.begin(), it)])) {
                    used[std::distance(all_players.begin(), it)] = true;
                    (*it)->role = PlayerRole::MF;
                    final_players.push_back(*it);
                    cnt++;
                    if(cnt == 6) break;
                }
            }
        }
        // FW
        cnt = 0;
        for (auto it = all_players.begin(); it != all_players.end(); ++it) {
            if((*it)->can_play[3] && !(used[std::distance(all_players.begin(), it)])) {
                used[std::distance(all_players.begin(), it)] = true;
                (*it)->role = PlayerRole::FW;
                final_players.push_back(*it);
                cnt++;
                if(cnt == 6) break;
            }
        }
        if(cnt < 6) {
            for (auto it = all_players.begin(); it != all_players.end(); ++it) {
                if(!(used[std::distance(all_players.begin(), it)])) {
                    used[std::distance(all_players.begin(), it)] = true;
                    (*it)->role = PlayerRole::FW;
                    final_players.push_back(*it);
                    cnt++;
                    if(cnt == 6) break;
                }
            }
        }
    }
    return final_players;
}

void MatchSummary::update(int status, int _GF, int _GA)
{
    paticipated = true;
    switch (status) {
    case _WON_:
        W += 1;
        break;
    case _DRAW_:
        D += 1;
        break;
    case _LOST_:
        L += 1;
        break;
    default:
        break;
    }
    GF += _GF;
    GA += _GA;
    GD = GF - GA;
    Pts = 3 * W + D;
}

void MatchSummary::punish(int _yello, int _indirect_red, int _direct_red, int _yello_and_red)
{
    yello += _yello;
    indirect_red += _indirect_red;
    direct_red += _direct_red;
    yello_and_red += _yello_and_red;
    Pts_concern_card = Pts - yello - 3 * indirect_red - 4 * direct_red - 5 * yello_and_red;
}

void MatchSummary::merge(const MatchSummary &rhs)
{
    paticipated = true;
    W += rhs.W;
    D += rhs.D;
    L += rhs.L;
    GF += rhs.GF;
    GA += rhs.GA;
    GD = GF - GA;
    Pts = 3 * W + D;
    punish(rhs.yello, rhs.indirect_red, rhs.direct_red, rhs.yello_and_red);
}

std::vector<int> MatchSummary::get()
{
    std::vector<int> ret;
    ret.push_back(W);
    ret.push_back(D);
    ret.push_back(L);
    ret.push_back(GF);
    ret.push_back(GA);
    ret.push_back(GD);
    ret.push_back(Pts);
    return ret;
}

int MatchSummary::getPts() const
{
    return Pts;
}

int MatchSummary::getGD() const
{
    return GD;
}

int MatchSummary::getGF() const
{
    return GF;
}

int MatchSummary::getPts_concern_card() const
{
    return Pts_concern_card;
}
