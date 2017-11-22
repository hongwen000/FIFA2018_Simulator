#include "nationalteam.h"

NationalTeam::NationalTeam()
{

}

QString teamNameEx(const NationalTeam* team) {
    QString ret(team->name);
    ret += (team->is_host ? " (hosts)" : "");
    return ret;
}

//!TODO
std::vector<Player*> NationalTeam::getStarters()
{
    std::vector<Player*> ret;
    for(int i = 0; i < 11; ++i) {
        auto pplayer = getFinalPlayers(i);
        if(pplayer->role == PlayerRole::NOT_ASSIGNED) {
            if(i < 1) pplayer->role = PlayerRole::GK;
            else if (i < 4) pplayer->role = PlayerRole::DF;
            else if (i < 7) pplayer->role = PlayerRole::MF;
            else pplayer->role = PlayerRole::FW;
        }
        ret.push_back(pplayer);
    }
    //unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count();
    //std::default_random_engine e(seed);
    //std::uniform_int_distribution<int> u(0,10);
    //auto uniform_rand = std::bind(u, e);
    auto captain_idx = RandLib::uniform_rand() % 11;
    int vice_captain_idx;
    while (true) {
        vice_captain_idx = RandLib::uniform_rand() % 11;
        if (captain_idx != vice_captain_idx)
            break;
    }
    for(auto player : ret) {
        player->is_captain = false;
        player->is_vice_captain = false;
    }
    ret[captain_idx]->is_captain = true;
    ret[vice_captain_idx]->is_vice_captain = true;
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
        ++index;
        all_players.push_back(player);
    }
    player_file.close();
}

//!TODO
Player* NationalTeam::getFinalPlayers(int idx)
{
    if(final_players.empty()) {
        for(int i = 0; i < 23; ++i) {
            final_players.push_back(all_players[i]);
        }
    }
    return final_players[idx];
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
