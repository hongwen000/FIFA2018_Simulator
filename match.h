#ifndef MATCH_H
#define MATCH_H
#include "base.h"
#include "nationalteam.h"
using std::pair;


struct MatchResult {
    int winner_idx;
    bool draw;
    int scoreNum[2];
    int penalty[2];
    NationalTeam* pteam[2];
    std::array<std::vector<Player*>, 2> starters;
    struct scoreInfo_t{
        NationalTeam* team;
        Player* player;
        int minute;
    };
    std::array<std::vector<scoreInfo_t>, 2> scoreRecords;
};

auto compare_score_time = [](auto lhs, auto rhs){
        return lhs.minute < rhs.minute;
    };

enum stage_t {
    GROUP_STAGE = 0, ROUND_OF_16 = 1, QUARTER_FINALS = 2,
    SEMI_FINALS = 3, FINAL = 4, STATS = 5
};

class Match
{
public:
    QString playMatch(std::map<QString, NationalTeam*> name2team);
    int id;
    QString time;
    QString place;
    QString team1;
    QString team2;
    stage_t stage;
    QString showMatchInfo(std::map<QString, NationalTeam*> name2team);
    MatchResult getMatchResult() const;

private:
    MatchResult matchResult;
    QString getMonthDay(QString time);
    std::vector<Player *> judgeScorePlayer(const std::vector<Player *> &ps, int scoreNumber);
    std::vector<int> judegScoreTime();
    void judgeScoreNumber(int,int);
    void judgeWinner();
    QString showPaticipantInfo();
    QString reportMatchResult();

    void process();
    pair<int, int> oneAction(pair<int, int> start);
};

#endif // MATCH_H
