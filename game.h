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
    std::vector<NationalTeam*> sortRank(const std::vector<NationalTeam*> &to_sort, stage_t stage);
    std::vector<QString> stage_name_uppercase = {"Group", "Round of 16", "Quarter-finals", "Semi-finals", "Finals"};
    std::vector<QString> stage_name_lowercase = {"group", "round of 16", "quarter-finals", "semi-finals", "finals"};
    std::vector<int> stage_start_match = {0, 48, 56, 60, 62};
    std::vector<int> stage_match_num = {48, 8, 4, 2, 2};
    std::vector<int> stage_team_num = {32, 16, 8, 4, 4};
    std::vector<int> stage_filename_idx = {16, 8, 4, 2, 1};
    void t3_1_showGroupMatches();
    void t3_2_playGroupMatches();
    void t3_3_summaryGroupMatches();
    void t3_4_showNextTeams();
    void t3_groupStageMatches();

    void t4_t7_showKnockoutMatches(stage_t stage);
    void t4_t7_playKnockoutMatches(stage_t stage);
    void t4_t7_summaryKnockoutMatches(stage_t stage);
    void t4_t7_showNextTeams(stage_t stage);
    void t4_t7_knockoutStageMatches(stage_t stage);
    void t8_showWholeGameStats();
};

#endif // GAME_H
