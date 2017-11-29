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
    int id;			//!比赛场次
    QString time;	//!比赛时间
    QString place;	//!比赛地点
    QString team1;	//!比赛队伍一名称（别名）
    QString team2;	//!比赛队伍二名称（别名）
    stage_t stage;	//!在整场世界杯中所属的阶段
    //!赛前输出本场比赛的相关信息
    QString showMatchInfo(std::map<QString, NationalTeam*> name2team);
    //!返回比赛结果
    MatchResult getMatchResult() const;
    //!进行比赛
    QString playMatch(std::map<QString, NationalTeam*> name2team);

private:
    MatchResult matchResult; 			//!存储比赛结果
    QString getMonthDay(QString time);	//!返回年月格式的比赛日期
    void recordScoreNumber(int,int);	//!将进球数信息写入相关结构体
    QString showPaticipantInfo();		//!显示上场球员信息
    QString reportMatchResult();		//!赛后输出本场比赛的结果信息
    void process();						//!执行比赛模拟
    pair<int, int> oneAction(pair<int, int> start); //!模拟比赛中的一次动作
};

    //void judgeWinner();
    //std::vector<Player *> judgeScorePlayer(const std::vector<Player *> &ps, int scoreNumber);
    //std::vector<int> judegScoreTime();
#endif // MATCH_H
