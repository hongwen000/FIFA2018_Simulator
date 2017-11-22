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
    NationalTeam();
    int world_rank;
    bool is_host;
    QString name;
    QString flag;
    QString continent;
    std::vector<Player *> getStarters();
    Player* getFinalPlayers(int idx);
    Player getReverser(bool is_GK);
    void loadPlayers(QString player_file_name);
    std::array<MatchSummary, 6> match_summaries;
    std::map<NationalTeam*, MatchSummary> co_country_summaries;
private:
#ifdef _MY_DEBUG_
public:
    std::vector<Player*> all_players;
    std::vector<Player*> final_players;
#endif

};

#endif // NATIONALTEAM_H
