#include "match.h"
#include<iostream>
using std::pair;
using std::make_pair;
using std::max;
using std::min;

QString showPlayerInfo(const Player* player) {
    QString* text = new QString;
    QTextStream ss(text);
    ss << "#" << player->index << ", " << playerNameEx(player) << " ";
    ss << Player::getRoleString(player->role) << endl;
    QString ret(*text);
    delete text;
    return ret;
}

QString Match::showPaticipantInfo() {
    QString* text = new QString;
    QTextStream ss(text);
    auto pteam1 = matchResult.pteam[0];
    auto pteam2 = matchResult.pteam[1];
    if(this->id == 64) {
        ss << "This match is the battle of the champions" << endl;
    }
    if(this->id == 63) {
        ss << "This match is the third place play-off" << endl;
    }
    ss << teamNameEx(pteam1) << " vs " << teamNameEx(pteam2) << endl;
    ss << teamNameEx(pteam1) << endl;
    auto starters1 = pteam1->getStarters();
    auto starters2 = pteam2->getStarters();
    for(auto player : starters1) {
        ss << showPlayerInfo(player);
    }
    ss << teamNameEx(pteam2) << endl;
    for(auto player: starters2) {
        ss << showPlayerInfo(player);
    }
    ss << "Playing..." << endl;
    QString ret(*text);
    delete text;
    return ret;
}
QString Match::reportMatchResult() {
    QString* text = new QString;
    QTextStream ss(text);
    std::vector<MatchResult::scoreInfo_t> mergedScoreRecord = matchResult.scoreRecords[0];
    for(auto rcd: matchResult.scoreRecords[1]) {
        mergedScoreRecord.push_back(rcd);
    }
    std::sort(mergedScoreRecord.begin(), mergedScoreRecord.end(), compare_score_time);
    int count[2] = {0};
    for(auto score : mergedScoreRecord) {
        ss << teamNameEx(score.team) << " did a goal, and it was" <<
             " #" << score.player->index << ", " << score.player->name << " did the goal" << endl;
        if(score.team == matchResult.pteam[0]) count[0]++;
        if(score.team == matchResult.pteam[1]) count[1]++;
        ss << "Now it is " << count[0] << ":" << count[1] << endl;
    }
    auto pwinner_team = matchResult.pteam[matchResult.winner_idx];
    auto ploser_team = matchResult.pteam[!matchResult.winner_idx];
    if(matchResult.draw)
        ss << "It is a draw with " << count[0] << ":" << count[1] << endl;
    else
        ss << teamNameEx(pwinner_team) << " win with " << count[0] << ":" << count[1] << endl;

    if (stage != GROUP_STAGE && matchResult.draw) {
        ss << teamNameEx(pwinner_team) << " win with " << matchResult.penalty[0] << ":" << matchResult.penalty[1] << " in a shoot-out" << endl;
    }

    if(this->id == 64) {
        ss << pwinner_team->name << " is the champion, congratulations!" << endl;
        ss << ploser_team->name << " won the second place." << endl;
    }
    if(this->id == 63) {
        ss << pwinner_team->name << " won the third place." << endl;
    }
    QString ret(*text);
    delete text;
    return ret;
}

QString Match::playMatch(std::map<QString, NationalTeam *> name2team)
{
    QString* text = new QString;
    QTextStream ss(text);
    matchResult.pteam[0] = name2team[team1];
    matchResult.pteam[1] = name2team[team2];
    ss << showPaticipantInfo();
    matchResult.starters[0] = matchResult.pteam[0]->getStarters();
    matchResult.starters[1] = matchResult.pteam[1]->getStarters();
/*  First Edition
    judgeWinner();
    judgeScoreNumber();
    auto scoreTime = judegScoreTime();
    auto scorePlayer1 = judgeScorePlayer(matchResult.starters[0], matchResult.scoreNum[0]);
    auto scorePlayer2 = judgeScorePlayer(matchResult.starters[1], matchResult.scoreNum[1]);

    for(size_t i = 0; i < scorePlayer1.size(); ++i) {
        matchResult.scoreRecords[0].push_back({matchResult.pteam[0], scorePlayer1[i], scoreTime[i]});
    }
    std::sort(matchResult.scoreRecords[0].begin(), matchResult.scoreRecords[0].end(), compare_score_time);
    for(size_t i = 0; i < scorePlayer2.size(); ++i) {
        matchResult.scoreRecords[1].push_back({matchResult.pteam[1], scorePlayer2[i], scoreTime[i + scorePlayer1.size()]});
    }
    std::sort(matchResult.scoreRecords[1].begin(), matchResult.scoreRecords[1].end(), compare_score_time);
*/

    process();

    ss << reportMatchResult();
    QString ret_text(*text);
    delete text;
    return ret_text;
}

pair<int, int> Match::oneAction(pair<int, int> start) {
    // if ans.first == -1 , then ans.second has a goal
    // tid for teamid, pid for playerid
    int tid = start.first;
    int pid = start.second;

    if (pid == 0) { // GK
        int tmp = RandLib::uniform_rand(1, 4);
        return make_pair(tid, tmp);
    }
    else if (pid <= 4) { // DF
        int power1 = matchResult.starters[tid][pid]->getPower();
        int power2 = 0;
        for (int i = 8; i <= 10; ++i)
            power2 += matchResult.starters[tid^1][i]->getPower();
        power2 /= 3;

        int back = RandLib::binomial_rand(0.1);
        if (back) {
            return make_pair(tid, 0);
        }
        else {
            int forward = RandLib::binomial_rand(0.7);
            if (forward) {  // DF -> MF
                double p = (100 - power1) * power2 / 10000.0;
                int fail = RandLib::binomial_rand(p);
                if (fail) {
                    int tmp = RandLib::uniform_rand(8,10);
                    return make_pair(tid^1, tmp);
                }
                else {
                    int tmp = RandLib::uniform_rand(5,7);
                    return make_pair(tid, tmp);
                }
            }
            else {  // DF -> DF
                power2 -= 30;
                double p = (100 - power1) * power2 / 10000.0;
                int fail = RandLib::binomial_rand(p);
                if (fail) {
                    int tmp = RandLib::uniform_rand(8,10);
                    return make_pair(tid^1, tmp);
                }
                else {
                    int tmp = RandLib::uniform_rand(1, 4);
                    return make_pair(tid, tmp);
                }
            }
        }
    }
    else if (pid <= 7) { // MF
        int power1 = matchResult.starters[tid][pid]->getPower();
        int power2 = 0;
        for (int i = 5; i <= 7; ++i)
            power2 += matchResult.starters[tid^1][i]->getPower();
        power2 /= 3;

        int back = RandLib::binomial_rand(0.2);
        if (back) { // MF -> DF
            power2 -= 20;
            double p = (100 - power1) * power2 / 10000.0;
            int fail = RandLib::binomial_rand(p);
            if (fail) {
                int tmp = RandLib::uniform_rand(5,7);
                return make_pair(tid^1, tmp);
            }
            else {
                int tmp = RandLib::uniform_rand(8,10);
                return make_pair(tid, tmp);
            }
        }
        else {
            int forward = RandLib::binomial_rand(0.375);
            if (forward) {  // MF -> FW
                double p = power1 * (100 - power2) / 10000.0;
                int succ = RandLib::binomial_rand(p);
                if (succ) {
                    int tmp = RandLib::uniform_rand(8,10);
                    return make_pair(tid, tmp);
                }
                else {
                    int tmp = RandLib::uniform_rand(5,7);
                    return make_pair(tid^1, tmp);
                }
            }
            else {  // MF -> MF
                double p = (100 - power1) * power2 / 10000.0;
                int fail = RandLib::binomial_rand(p);
                if (fail) {
                    int tmp = RandLib::uniform_rand(5,7);
                    return make_pair(tid^1, tmp);
                }
                else {
                    int tmp = RandLib::uniform_rand(5,7);
                    return make_pair(tid, tmp);
                }
            }
        }
    }
    else { // FW
        int power1 = matchResult.starters[tid][pid]->getPower();
        int power2 = 0;
        for (int i = 1; i <= 4; ++i)
            power2 += matchResult.starters[tid^1][i]->getPower();
        power2 /= 3;

        int back = RandLib::binomial_rand(0.3);
        if (back) { // FW -> MF
            double p = (100 - power1) * power2 / 10000.0;
            int fail = RandLib::binomial_rand(p);
            if (fail) {
                int tmp = RandLib::uniform_rand(1,4);
                return make_pair(tid^1, tmp);
            }
            else {
                int tmp = RandLib::uniform_rand(5,7);
                return make_pair(tid, tmp);
            }
        }
        else {
            int forward = RandLib::binomial_rand(0.5);
            if (forward) {  // FW -> Goal
//                power1 -= 10;
                double p = power1 * 0.95 * (100 - power2 * 0.85) / 10000.0;
                int succ = RandLib::binomial_rand(p);
                if (succ) {
                    return make_pair(-1, pid);
                }
                else {
                    int tmp = RandLib::uniform_rand(1,4);
                    return make_pair(tid^1, tmp);
                }
            }
            else {  // FW -> FW
//                power1 -= 20;
                double p = (100 - power1) * power2 / 10000.0;
                int fail = RandLib::binomial_rand(p);
                if (fail) {
                    int tmp = RandLib::uniform_rand(1,4);
                    return make_pair(tid^1, tmp);
                }
                else {
                    int tmp = RandLib::uniform_rand(8,10);
                    return make_pair(tid, tmp);
                }
            }
        }
    }
}

void Match::process() {
    int start = RandLib::binomial_rand(0.5);
    pair<int, int> now,next;
    now.first = start, now.second = 6;

    int scores[2];
    scores[0] = scores[1] = 0;
    std::vector<pair<Player *, int> > goals[2];

    for (int i = 0; i < 360; ++i) {
        next = oneAction(now);
        if (next.first == -1) { // team now.first, now.second has a goal
            // record
            matchResult.starters[now.first][now.second]->scoreNumber++;
            scores[now.first]++;
            goals[now.first].push_back(make_pair(matchResult.starters[now.first][now.second], i / 4));

            now.first = now.first ^ 1;
            now.second = 0;
        }
        else {
            now = next;
        }
    }

    if (stage != GROUP_STAGE && scores[0] == scores[1]) { // 加时赛
        now.first = RandLib::binomial_rand(0.5), now.second = 6;
        for (int i = 360; i < 500; ++i) {
            next = oneAction(now);
            if (next.first == -1) { // team now.first, now.second has a goal
                // record
                matchResult.starters[now.first][now.second]->scoreNumber++;
                scores[now.first]++;
                goals[now.first].push_back(make_pair(matchResult.starters[now.first][now.second], i / 4));

                now.first = now.first ^ 1;
                now.second = 0;
            }
            else {
                now = next;
            }
        }
    }

    matchResult.penalty[0] = matchResult.penalty[1] = 0;
    if (stage != GROUP_STAGE && scores[0] == scores[1]) { // 点球大战
        for (int i = 0; i < 5; ++i) {
            matchResult.penalty[0] += RandLib::binomial_rand(0.8);
            matchResult.penalty[1] += RandLib::binomial_rand(0.8);
        }
        while(matchResult.penalty[0] == matchResult.penalty[1]) {
            matchResult.penalty[0] += RandLib::binomial_rand(0.8);
            matchResult.penalty[1] += RandLib::binomial_rand(0.8);
        }
    }

    // judgeWinner
    matchResult.winner_idx = (scores[0] > scores[1]
            || (scores[0] == scores[1] && matchResult.penalty[0] > matchResult.penalty[1]))
            ? 0 : 1;

    // judgeScoreNumber
    int winner_score = max(scores[0], scores[1]);
    int loser_score = min(scores[0], scores[1]);
    auto pteam1 = matchResult.pteam[0];
    auto pteam2 = matchResult.pteam[1];
    if(loser_score == winner_score) {
        matchResult.draw = true;
        matchResult.scoreNum[0] = winner_score;
        matchResult.scoreNum[1] = winner_score;
        if (stage == GROUP_STAGE) {
            pteam1->match_summaries[stage].update(_DRAW_, winner_score, winner_score);
            pteam2->match_summaries[stage].update(_DRAW_, winner_score, winner_score);
            pteam1->co_country_summaries[pteam2].update(_DRAW_, winner_score, winner_score);
            pteam2->co_country_summaries[pteam1].update(_DRAW_, winner_score, winner_score);
        }
        else {
            auto pwinner_team = matchResult.pteam[matchResult.winner_idx];
            auto ploser_team = matchResult.pteam[!matchResult.winner_idx];
            pwinner_team->match_summaries[stage].update(_WON_, winner_score, loser_score);
            ploser_team->match_summaries[stage].update(_LOST_, loser_score, winner_score);
            pwinner_team->co_country_summaries[ploser_team].update(_WON_, winner_score, loser_score);
            ploser_team->co_country_summaries[pwinner_team].update(_LOST_, loser_score, winner_score);
        }
    } else {
        matchResult.scoreNum[0] = matchResult.winner_idx == 0 ? winner_score : loser_score;
        matchResult.scoreNum[1] = matchResult.winner_idx == 1 ? winner_score : loser_score;
        matchResult.draw = false;
        auto pwinner_team = matchResult.pteam[matchResult.winner_idx];
        auto ploser_team = matchResult.pteam[!matchResult.winner_idx];
        pwinner_team->match_summaries[stage].update(_WON_, winner_score, loser_score);
        ploser_team->match_summaries[stage].update(_LOST_, loser_score, winner_score);
        pwinner_team->co_country_summaries[ploser_team].update(_WON_, winner_score, loser_score);
        ploser_team->co_country_summaries[pwinner_team].update(_LOST_, loser_score, winner_score);
    }


    for(size_t i = 0; i < goals[0].size(); ++i) {
        matchResult.scoreRecords[0].push_back({matchResult.pteam[0], goals[0][i].first, goals[0][i].second});
    }
    std::sort(matchResult.scoreRecords[0].begin(), matchResult.scoreRecords[0].end(), compare_score_time);
    for(size_t i = 0; i < goals[1].size(); ++i) {
        matchResult.scoreRecords[1].push_back({matchResult.pteam[1], goals[1][i].first, goals[1][i].second});
    }
    std::sort(matchResult.scoreRecords[1].begin(), matchResult.scoreRecords[1].end(), compare_score_time);
}

QString Match::getMonthDay(QString time)
{
    QRegExp re("\^(\\d|\\d\\d)");
    QRegExp re2("(June|July)");
    re.indexIn(time);
    re2.indexIn(time);
    QString day = re.cap();
    QString month = re2.cap();
    return month + " " + day;
}

QString Match::showMatchInfo(std::map<QString, NationalTeam *> name2team)
{
    QString* text = new QString;
    QTextStream ss(text);
    auto pteam1 = name2team[team1];
    auto pteam2 = name2team[team2];
    if(!pteam1) {
        qDebug() << id;
        qDebug() << team1;
        throw;
    }
    if(!pteam2) {
        qDebug() << id;
        qDebug() << team2;
        throw;
    }
    ss << teamNameEx(pteam1) << " vs " << teamNameEx(pteam2) << ", " << place << ", " << getMonthDay(time);
    QString ret(*text);
    delete text;
    return ret;
}

MatchResult Match::getMatchResult() const
{
    return matchResult;
}

void Match::judgeWinner() {
    int total1 = 0;
    int total2 = 0;
    auto ps1 = matchResult.starters[0];
    auto ps2 = matchResult.starters[1];
    for(auto p : ps1){
        total1 += p->overall;
    }
    for(auto p : ps2){
        total2 += p->overall;
    }
    int better_team_idx = total1 < total2;
    //unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count();
    //std::default_random_engine e(seed);
    //std::binomial_distribution<int> b(1,0.6);
    //auto binomial_rand = std::bind(b, e);
    int r = RandLib::binomial_rand();
    matchResult.winner_idx = r ? better_team_idx : !better_team_idx;
}

void Match::judgeScoreNumber(int winner_score, int loser_score) {
    //unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count();
    //std::default_random_engine e(seed);
    //std::normal_distribution<double> n(2.1, 2);
    //auto normal_rand = std::bind(n, e);
/*
    int winner_score;
    while (true) {
        winner_score = std::floor(RandLib::normal_rand());
        if(winner_score >= 0 && winner_score <= 7)
            break;
    }
*/
    auto pteam1 = matchResult.pteam[0];
    auto pteam2 = matchResult.pteam[1];
//    if(stage != GROUP_STAGE && winner_score == 0)
//        winner_score = 1;
/*
    if(winner_score == loser_score) {
        matchResult.draw = true;
        matchResult.scoreNum[0] = winner_score;
        matchResult.scoreNum[1] = winner_score;
        pteam1->match_summaries[stage].update(_DRAW_, winner_score, winner_score);
        pteam2->match_summaries[stage].update(_DRAW_, winner_score, winner_score);
        pteam1->co_country_summaries[pteam2].update(_DRAW_, winner_score, winner_score);
        pteam2->co_country_summaries[pteam1].update(_DRAW_, winner_score, winner_score);
        return;
    }
*/
    //std::uniform_int_distribution<int> u(0,10);
    //auto uniform_rand = std::bind(u, e);

//    int loser_score = winner_score - (RandLib::uniform_rand() % winner_score);

//    if(stage != GROUP_STAGE && loser_score == winner_score)
//        loser_score = winner_score - 1;
    if(loser_score == winner_score) {
        matchResult.draw = true;
        matchResult.scoreNum[0] = winner_score;
        matchResult.scoreNum[1] = winner_score;
        pteam1->match_summaries[stage].update(_DRAW_, winner_score, winner_score);
        pteam2->match_summaries[stage].update(_DRAW_, winner_score, winner_score);
        pteam1->co_country_summaries[pteam2].update(_DRAW_, winner_score, winner_score);
        pteam2->co_country_summaries[pteam1].update(_DRAW_, winner_score, winner_score);
    } else {
        matchResult.scoreNum[0] = matchResult.winner_idx == 0 ? winner_score : loser_score;
        matchResult.scoreNum[1] = matchResult.winner_idx == 1 ? winner_score : loser_score;
        matchResult.draw = false;
        auto pwinner_team = matchResult.pteam[matchResult.winner_idx];
        auto ploser_team = matchResult.pteam[!matchResult.winner_idx];
        pwinner_team->match_summaries[stage].update(_WON_, winner_score, loser_score);
        ploser_team->match_summaries[stage].update(_LOST_, loser_score, winner_score);
        pwinner_team->co_country_summaries[ploser_team].update(_WON_, winner_score, loser_score);
        ploser_team->co_country_summaries[pwinner_team].update(_LOST_, loser_score, winner_score);
    }
}

std::vector<int> Match::judegScoreTime() {
    if(matchResult.scoreNum[0] == 0 && matchResult.scoreNum[1] == 0) {
        return std::vector<int>();
    }
    std::vector<int> scoreTime;
    //unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count();
    //std::default_random_engine e(seed);
    //std::uniform_int_distribution<int> u(3,90);
    //auto uniform_rand = std::bind(u, e);
    while (true) {
        scoreTime.clear();
        for(int i = 0; i < matchResult.scoreNum[0] + matchResult.scoreNum[1]; ++i) {
            scoreTime.push_back(RandLib::uniform_rand() % 88 + 3);
        }
        std::vector<int> sorted = scoreTime;
        std::sort(sorted.begin(), sorted.end());
        for(size_t i = 0; i < sorted.size() - 1; ++i) {
            if(sorted[i + 1] - sorted[i - 1] < 5)
                continue;
        }
        break;
    }
    return scoreTime;
}

std::vector<Player*> Match::judgeScorePlayer(const std::vector<Player *> &ps, int scoreNumber) {
    std::vector<Player*> ret;
    for(int i = 0; i < scoreNumber; ++i) {
        Player* whom = NULL;
        while(true) {
            //unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count();
            //std::default_random_engine e(seed);
            //std::uniform_int_distribution<int> u(0,10);
            //auto uniform_rand = std::bind(u, e);
            int idx = RandLib::uniform_rand() % 11;
            whom = ps[idx];
            if(whom->role != PlayerRole::GK)
                break;
        }
        whom->scoreNumber++;
        ret.push_back(whom);
    }
    return ret;
}

