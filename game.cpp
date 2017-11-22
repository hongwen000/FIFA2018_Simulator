#include "game.h"
using namespace std::placeholders;
Game::Game(QString country_file_name, QString player_floder, QString timeplace_file_name)
{
    QString country_string;
    QFile country_file;
#ifdef Q_OS_MAC
    QDir::setCurrent("../../../");
#endif
    country_file.setFileName(country_file_name);
    country_file.open(QIODevice::ReadOnly | QIODevice::Text);
    country_string = country_file.readAll();
    QJsonDocument country_d = QJsonDocument::fromJson(country_string.toUtf8());
    QJsonArray country_arr = country_d.array();
    for(QJsonArray::iterator it = country_arr.begin(); it != country_arr.end(); ++it) {
        QJsonObject country_data = (*it).toObject();
        NationalTeam* team = new NationalTeam;
        team->name = country_data["Name"].toString();
        team->is_host = team->name == "Russia" ? true : false;
        team->continent = country_data["Continent"].toString();
        team->flag = country_data["Flag"].toString();
        team->world_rank = country_data["Rank"].toString().toInt();
        team->loadPlayers(player_floder + "/" + team->name + "Players.json");
        teams.push_back(team);
        name2team[team->name] = team;
    }
    country_file.close();
    QString timeplace_string;
    QFile timeplace_file;
    timeplace_file.setFileName(timeplace_file_name);
    timeplace_file.open(QIODevice::ReadOnly | QIODevice::Text);
    timeplace_string = timeplace_file.readAll();
    QJsonDocument timeplace_d = QJsonDocument::fromJson(timeplace_string.toUtf8());
    QJsonArray timeplace_arr = timeplace_d.array();
    for(QJsonArray::iterator it = timeplace_arr.begin(); it != timeplace_arr.end(); ++it) {
        QJsonObject timeplace_data = (*it).toObject();
        Match* match = new Match;
        match->id    = timeplace_data["ID"].toString().toInt();
        match->team1 = timeplace_data["Team1"].toString();
        match->team2 = timeplace_data["Team2"].toString();
        match->place = timeplace_data["Place"].toString();
        match->time  = timeplace_data["Time"].toString();
        if(match->id <= 48)
            match->stage = GROUP_STAGE;
        else if (match->id <= 56)
            match->stage = ROUND_OF_16;
        else if (match->id <= 60)
            match->stage = QUARTER_FINALS;
        else if (match->id <= 62)
            match->stage = SEMI_FINALS;
        else
            match->stage = Final;
        matches.push_back(match);
    }
    timeplace_file.close();
    std::sort(matches.begin(), matches.end(), [](auto m1, auto m2) {
        return m1->id < m2->id;
    });
}

void Game::playGame()
{
    t1_showQuaified();
    groups = t2_divideGroups(name2team);
    t3_groupStageMatches();
}

pots_t Game::prepare_pots()
{
    QString* text = new QString;
    QTextStream ss(text);
    pots_t pots;
    std::vector<NationalTeam*> sortedTeams(teams);
    std::sort(sortedTeams.begin(), sortedTeams.end(), [](auto t1, auto t2) {
        return t1->world_rank < t2->world_rank;
    });
    for(auto it = sortedTeams.begin(); it != sortedTeams.end(); ++it)
        if((*it)->is_host) {
            pots[0].push_back(*it);
            sortedTeams.erase(it);
            break;
        }
    for(int i = 1; i < 32; ++i) {
       pots[i / 8].push_back(sortedTeams[i - 1]);
    }
    ss << "First stage" << endl;
    for(int i = 0; i < 4; ++i) {
        ss << "pot" << i << endl;
        for(auto team : pots[i])
            ss << " " << teamNameEx(team) <<endl;
    }
    dualPrint(*text, "finalDraw.txt");
    delete text;
    return pots;
}

bool Game::checkDraw(const groups_t &groups)
{
    std::map<QString, int> count;
    for(auto teams : groups) {
        count.clear();
        for(auto team : teams)
            count[team->continent]++;
        for(auto cnt : count) {
            if(cnt.first == "UEFA") {
                if(cnt.second > 2) {
                    return false;
                }
            }
            else
                if(cnt.second > 1) {
                    return false;
                }
        }
    }
    return true;
}
//!TODO
groups_t Game::drawFromPots(name2team_map_t&name2team, const std::array<std::vector<NationalTeam *>, 4> &pots)
{
    QString* text = new QString;
    QTextStream ss(text);
    std::vector<int> perm;
    groups_t groups;
    while (true) {
        perm.clear();
        for(int i = 0; i < 8; ++i)
            groups[i].clear();
        for(int i = 0; i < 32; ++i)
            perm.push_back(i);
        unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count();
        std::shuffle (perm.begin (), perm.end(), std::default_random_engine (seed));
        std::vector<NationalTeam*> seq;
        for(int i = 0; i < 32; ++i) {
            if(perm[i] == 0)
                std::swap(perm[0], perm[i]);
        }
        for(int i = 0; i < 4; ++i)
            for(auto t : pots[i])
                seq.push_back(t);

        for(int i = 0; i < 32; ++i) {
            groups[i / 4].push_back(seq[perm[i]]);
        }
        if(checkDraw(groups))
            break;
    }
    ss << "The Final Draw" << endl;
    for(int i = 0; i < 8; ++i) {
        ss << "group " << (char)('A' + i) << endl;
        for(int j = 0; j < 4; ++j) {
            ss << " " << groups[i][j]->name << endl;
            name2team[QString() + ('A' + i) + QString::number(j + 1)] = groups[i][j];
        }
    }
    dualPrint(*text, "finalDraw.txt", true);
    delete text;
//    for(auto i : name2team)
//        std::cout << i.first.toStdString() << " " << i.second->name.toStdString() << std::endl;
    return groups;
}

void Game::dualPrint(const QString &text, const QString &filename, bool append)
{
    QString text2File(text);
    QFile file;
    file.setFileName(filename);
    if(append)
        file.open(QIODevice::Text | QIODevice::ReadWrite | QIODevice::Append);
    else
        file.open(QIODevice::Text | QIODevice::ReadWrite);
    QTextStream ss2File(&file);
    ss2File << text2File;
    file.close();
    std::cout << text.toStdString();
}

void Game::t1_showQuaified()
{
    QString* text = new QString;
    QTextStream ss(text);
    std::map<QString, int> count;
    std::vector<QString> confederation = {"AFC", "CAF", "CONCACAF", "CONMEBOL", "OFC", "UEFA"};

    for(auto team : teams)
        count[team->continent]++;
    for(int i = 0; i < 6; ++i) {
        ss << confederation[i] << "(" << count[confederation[i]] << ")" << endl;
        for(auto team :teams)
            if(team->continent ==  confederation[i])
                ss << "  " << team->name << "(" << team->world_rank << ")" << endl;
    }
    dualPrint(*text, "team32.txt");
    delete text;
}

groups_t Game::t2_divideGroups(name2team_map_t& name2team)
{
    pots_t pots = prepare_pots();
    return drawFromPots(name2team, pots);
}

std::vector<NationalTeam*> Game::sortGroupRank(const std::vector<NationalTeam *> &to_sort)
{
    std::map<NationalTeam*, NationalTeam*> disjoint_father;
    for(auto pteam : to_sort) {
        disjoint_father[pteam] = pteam;
    }
    std::function<NationalTeam*(NationalTeam*)> disjoint_find = [&](NationalTeam* t) {
        if(disjoint_father[t] != t)
            disjoint_father[t] = disjoint_find(disjoint_father[t]);
        return disjoint_father[t];
    };
    auto disjoint_union = [&](NationalTeam* t1, NationalTeam* t2) {
        auto t1Root = disjoint_find(t1);
        auto t2Root = disjoint_find(t2);
        if(t1Root == t2Root)
            return;
        else
            disjoint_father[t1Root] = t2Root;
    };
    std::set<NationalTeam*> equal;
    auto _rule_a_to_c = [&](const std::pair<MatchSummary, NationalTeam*>& t1, const std::pair<MatchSummary, NationalTeam*>& t2, bool side_effect) {
        if(t1.first.getPts() != t2.first.getPts()) {
            return t1.first.getPts() > t2.first.getPts();
        } else if (t1.first.getGD() != t2.first.getGD()) {
            return t1.first.getGD() > t2.first.getGD();
        } else if (t1.first.getGF() != t2.first.getGF()) {
            return t1.first.getGF() > t2.first.getGF();
        } else {
            if(side_effect) {
                disjoint_union(t1.second, t2.second);
                equal.insert(t1.second);
                equal.insert(t2.second);
            }
            return false;
        }
    };

    auto rule_a_to_c_side_effect  = std::bind(_rule_a_to_c, _1, _2, true);
    auto rule_a_to_c_without_side_effect  = std::bind(_rule_a_to_c, _1, _2, false);

    auto rule_d_to_h = [&](const std::pair<MatchSummary, NationalTeam*>& t1, const std::pair<MatchSummary, NationalTeam*>& t2) {
//        qDebug() << "In rule d to h";
//        qDebug() << "Pts:" << t1.second->name << t1.first.getPts() <<t2.second->name << t2.first.getPts();
//        qDebug() << "GD:" << t1.second->name << t1.first.getGD() <<t2.second->name << t2.first.getGD();
//        qDebug() << "GF:" << t1.second->name << t1.first.getGF() <<t2.second->name << t2.first.getGF();
//        qDebug() << "Pts_concern:" << t1.second->name << t1.first.getPts_concern_card() <<t2.second->name << t2.first.getPts_concern_card();
        if(t1.first.getPts() != t2.first.getPts()) {
            return t1.first.getPts() > t2.first.getPts();
        } else if (t1.first.getGD() != t2.first.getGD()) {
            return t1.first.getGD() > t2.first.getGD();
        } else if (t1.first.getGF() != t2.first.getGF()) {
            return t1.first.getGF() > t2.first.getGF();
        } else if (t1.first.getPts_concern_card() != t1.first.getPts_concern_card()){
            return t1.first.getPts_concern_card() > t1.first.getPts_concern_card();
        } else {
            return RandLib::uniform_rand() % 2 == 0;
        }
    };

    auto _packer = [](const std::vector<NationalTeam*>& to_sort, bool concerned) {
        std::vector<std::pair<MatchSummary, NationalTeam*> > ret;
        for(NationalTeam* pteam : to_sort) {
//            if(to_sort.size() != 4) qDebug() << "### " << pteam->name << "Play with";
            MatchSummary summ;
            if(concerned) {
                for(NationalTeam* co_pteam : to_sort) {
                    if(co_pteam == pteam)
                        continue;
                    else {
//                        if(to_sort.size() != 4) {
//                            qDebug() << co_pteam->name;
//                            for(auto i :pteam->co_country_summaries[co_pteam].get()) {
//                                qDebug() << i;
//                            }
//                        }
                        summ.merge(pteam->co_country_summaries[co_pteam]);
                    }
                }
//                if(to_sort.size() != 4) {
//                    qDebug() << "**** summ:";
//                    for(auto i :summ.get()) {
//                        qDebug() << i;
//                    }
//                    qDebug() << "**** right:" ;
//                    for(auto i :pteam->match_summaries[GROUP_STAGE].get()) {
//                        qDebug() << i;
//                    }
//                }
            } else {
                summ = pteam->match_summaries[GROUP_STAGE];
            }
            ret.push_back(std::make_pair(summ, pteam));
        }
        return ret;
    };
    auto packer_concerned_team  = std::bind(_packer, _1, true);
    auto packer_group_stage  = std::bind(_packer, _1, false);
    auto packed = packer_group_stage(to_sort);
    std::sort(packed.begin(), packed.end(), rule_a_to_c_side_effect);
    while(!equal.empty()) {
//        qDebug() << equal.size();
//        for(auto i : equal) {
//            qDebug() << '\t' << i->name;
//        }
        std::vector<NationalTeam*> to_sort_still_equal;
        auto pteamRoot = *(equal.begin());
        for(auto pteam : to_sort) {
            if(disjoint_find(pteam) == disjoint_find(pteamRoot)) {
                to_sort_still_equal.push_back(pteam);
 //               qDebug() << "1. Found " << pteam->name << " and " << pteamRoot->name << " are same ";
                equal.erase(pteam);
            }
        }

        auto packed_still_equal = packer_group_stage(to_sort_still_equal);
        std::sort(packed_still_equal.begin(), packed_still_equal.end(), rule_a_to_c_without_side_effect);
        auto p1 = packed_still_equal.front();
        auto p2 = packed_still_equal.back();
        auto it1 = std::find_if(packed.begin(), packed.end(), [p1](auto p) {
            return p.second == p1.second;
        });
        auto it2 = std::find_if(packed.begin(), packed.end(), [p2](auto p) {
            return p.second == p2.second;
        });
//        qDebug() << "it1 points " << it1->second->name << "\t" << std::distance(packed.begin(), it1);
//        qDebug() << "it2 points " << it2->second->name << "\t" << std::distance(packed.begin(), it2);
        auto packed_concerned = packer_concerned_team(to_sort_still_equal);
            std::sort(packed_concerned.begin(), packed_concerned.end(), rule_d_to_h);
            if(it1 > it2)
                throw(std::runtime_error("it1 > it2"));
            for(auto it = it1; it <= it2; ++it) {
                *it = packed_still_equal[std::distance(it1, it)];
            }
//        for(size_t i = 0; i < packed_still_equal.size(); ++i) {
//            (it1+i)->first = packed_still_equal[i].first;
//        }
//        std::sort(it1, it2 + 1, rule_d_to_h);
    }
    std::vector<NationalTeam*> sorted;
    for(auto p : packed)
        sorted.push_back(p.second);
    return sorted;
}

void Game::t3_1_showGroupMatches()
{
    QString* text = new QString;
    QTextStream ss(text);
    ss << "Matches by squads" << endl;
    for(int i = 0; i < 8; ++i) {
        ss << "Group " << (char)('A' + i) << endl;
        for(int j = 0; j < 6; ++j) {
            ss << " " << matches[6 * i + j]->showMatchInfo(name2team) << endl;
        }
    }
    dualPrint(*text, "schedule16.txt");
    delete text;
}

void Game::t3_2_playGroupMatches()
{
    QString* text = new QString;
    QTextStream ss(text);
    ss << "Group stage:" << endl;
    for(int i = 0; i < 48; ++ i) {
        ss << matches[i]->playMatch(name2team);
    }
    dualPrint(*text, "Result16.txt");
    delete text;
}

void Game::t3_3_summaryGroupMatches()
{
    QString* text = new QString;
    QTextStream ss(text);
    for(int i = 0; i < 8; ++i) {
          ss << "Final result for group " << (char)('A' + i) << endl;
          ss << qSetFieldWidth(20) << left << "Team";
          ss << reset;
          ss << "\tW\tD\tL\tGF\tGA\tGD\tPts" << endl;
          std::vector<NationalTeam*> to_sort;
          for(auto team : groups[i]) {
              to_sort.push_back(team);
          }
          auto sorted = sortGroupRank(to_sort);
          for(int j = 0; j < 4; ++j) {
              auto team = sorted[j];
              if(j == 0) {
                  name2team["Winner Group " + QString(QChar('A' + i))] = team;
                  teams16.push_back(team);
              }
              if(j == 1) {
                  name2team["Runner-up Group " + QString(QChar('A' + i))] = team;
                  teams16.push_back(team);
              }
              ss << qSetFieldWidth(20) << left << team->name;
              ss << reset;
              auto summ = team->match_summaries[GROUP_STAGE].get();
              for(auto t : summ) {
                  ss << '\t' <<  t ;
              }
              ss << endl;
          }
    }
    dualPrint(*text, "Result16.txt");
    delete text;
}

void Game::t3_groupStageMatches()
{
    t3_1_showGroupMatches();
    t3_2_playGroupMatches();
    t3_3_summaryGroupMatches();
}
