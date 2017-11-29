#ifndef NATIONALTEAM_H
#define NATIONALTEAM_H
#include "base.h"
#include "player.h"
#define _LOST_ 0
#define _DRAW_ 1
#define _WON_ 3
class NationalTeam;

QString teamNameEx(const NationalTeam* team);

struct MatchSummary {
    void update(int status, int _GF, int _GA);
    void punish(int _yello = 0, int _indirect_red = 0, int _direct_red = 0, int _yello_and_red = 0);
    void merge(const MatchSummary& rhs);
    std::vector<int> get();

public:
    int getPts() const;

    int getGD() const;

    int getGF() const;

    int getPts_concern_card() const;

private:
    bool paticipated = false;
    int W = 0;
    int D = 0;
    int L = 0;
    int GF = 0;
    int GA = 0;
    int GD = 0;
    int Pts = 0;
    int yello = 0;
    int indirect_red = 0;
    int direct_red = 0;
    int yello_and_red = 0;
    int Pts_concern_card = 0;
};

class NationalTeam
{
public:
    int world_rank; 							//!国家队世界排名
    bool is_host;								//!是否为东道主
    QString name;								//!队名
    QString flag;								//!国旗图片地址
    QString continent;							//!属于哪一足协
    std::vector<Player *> getStarters();		//!获取一场比赛的首发阵容
    std::vector<Player *> getFinalPlayers();	//!获取参赛的23人名单
    void loadPlayers(QString player_file_name);	//!从文件加载队员信息
    std::array<MatchSummary, 6> match_summaries;//!各比赛阶段赛况汇总
    //!和各支队伍比赛结果汇总
    std::map<NationalTeam*, MatchSummary> co_country_summaries;
private:
    std::vector<Player*> all_players;			//!该国家队所有球员名单
    std::vector<Player*> final_players;			//!参加世界杯的23人名单
};

#endif // NATIONALTEAM_H
