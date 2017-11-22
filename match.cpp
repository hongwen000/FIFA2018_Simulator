#include "match.h"

QString showPlayerInfo(const Player* player) {
    QString* text = new QString;
    QTextStream ss(text);
    ss << "#" << player->index << ", " << playerNameEx(player);
    switch (player->role) {
    case PlayerRole::GK:
        ss << "GK";
        break;
    case PlayerRole::DF:
        ss << "DF";
        break;
    case PlayerRole::FW:
        ss << "FW";
        break;
    case PlayerRole::MF:
        ss << "MF";
        break;
    default:
        break;
    }
    ss << endl;
    QString ret(*text);
    delete text;
    return ret;
}

QString Match::showPaticipantInfo() {
    QString* text = new QString;
    QTextStream ss(text);
    auto pteam1 = matchResult.pteam[0];
    auto pteam2 = matchResult.pteam[1];
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
    if(matchResult.draw)
        ss << "It is a draw with " << count[0] << ":" << count[1] << endl;
    else
        ss << teamNameEx(matchResult.pteam[matchResult.winner_idx]) << " win with " << count[0] << ":" << count[1] << endl;
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
    ss << reportMatchResult();
    QString ret_text(*text);
    delete text;
    return ret_text;
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
    ss << pteam1->name << teamNameEx(pteam1) <<
          (pteam2->name) << teamNameEx(pteam2) << ", " << place << ", " << getMonthDay(time);
    QString ret(*text);
    delete text;
    return ret;
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

void Match::judgeScoreNumber() {
    //unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count();
    //std::default_random_engine e(seed);
    //std::normal_distribution<double> n(2.1, 2);
    //auto normal_rand = std::bind(n, e);
    int winner_score;
    while (true) {
        winner_score = std::floor(RandLib::normal_rand());
        if(winner_score >= 0 && winner_score <= 7)
            break;
    }
    auto pteam1 = matchResult.pteam[0];
    auto pteam2 = matchResult.pteam[1];
    if(winner_score == 0) {
        matchResult.draw = true;
        matchResult.scoreNum[0] = 0;
        matchResult.scoreNum[1] = 0;
        pteam1->match_summaries[stage].update(_DRAW_, 0, 0);
        pteam2->match_summaries[stage].update(_DRAW_, 0, 0);
        pteam1->co_country_summaries[pteam2].update(_DRAW_, 0, 0);
        pteam2->co_country_summaries[pteam1].update(_DRAW_, 0, 0);
        return;
    }
    //std::uniform_int_distribution<int> u(0,10);
    //auto uniform_rand = std::bind(u, e);
    int loser_score = winner_score - (RandLib::uniform_rand() % winner_score);
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

