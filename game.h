#ifndef GAME_H
#define GAME_H
#include "base.h"
#include "match.h"
#include "nationalteam.h"

typedef std::array<std::vector<NationalTeam*>, 8> groups_t;
typedef std::array<std::vector<NationalTeam*>, 4> pots_t;
typedef std::map<QString, NationalTeam *> name2team_map_t;
class Game
{
public:
    Game(QString country_file_name = "./CountryData.json", QString player_floder = "Players", QString timeplace_file_name = "MatchTimePlace.json");
    void playGame();
private:
    name2team_map_t name2team;
    std::vector<NationalTeam*> teams;
    std::vector<NationalTeam*> teams16;
    std::vector<Match*> matches;
    groups_t groups;
    pots_t prepare_pots();
    bool checkDraw(const groups_t& groups);
    groups_t drawFromPots(name2team_map_t& name2team, const pots_t& pots);
    void dualPrint(const QString& text, const QString& filename, bool append = false);
    void t1_showQuaified();
    groups_t t2_divideGroups(name2team_map_t& name2team);
    std::vector<NationalTeam*> sortGroupRank(const std::vector<NationalTeam *> &to_sort);
    void t3_1_showGroupMatches();
    void t3_2_playGroupMatches();
    void t3_3_summaryGroupMatches();
    void t3_groupStageMatches();


};

#endif // GAME_H
