#ifndef GAME_H
#define GAME_H
#include "base.h"
#include "match.h"
#include "nationalteam.h"

//分组信息的类型
typedef std::array<std::vector<NationalTeam*>, 8> groups_t;
//抽签罐子的类型
typedef std::array<std::vector<NationalTeam*>, 4> pots_t;
//球队“别名”到具体球队的映射关系的类型
typedef std::map<QString, NationalTeam *> name2team_map_t;
class Game
{
public:
    //!构造函数，从默认从CountryData.json中读取参赛国家，从Players文件夹下读取各国球员信息，从MatchTimePlace.json中读取比赛时间日期
    Game(QString country_file_name = "./CountryData.json", QString player_floder = "Players", QString timeplace_file_name = "MatchTimePlace.json");
    void playGame();    				//!执行世界杯预演全过程
private:
    name2team_map_t name2team;				//!记录球队“别名”到具体球队的映射关系
    std::vector<NationalTeam*> teams;		//!储存所有队伍
    std::vector<NationalTeam*> teams16;		//!储存晋级淘汰赛的队伍
    std::vector<Match*> matches;			//!储存所有比赛
    groups_t groups;						//!储存小组赛分组情况
    pots_t prepare_pots();					//!准备抽签罐子
    //!从罐子执行抽签
    groups_t drawFromPots(name2team_map_t& name2team, const pots_t& pots);
    bool checkDraw(const groups_t& groups); //!检查抽签结果是否合理
    //!同时向文件和标准输出进行输出
    void dualPrint(const QString& text, const QString& filename, bool append = false);
    //!实现任务一
    void t1_showQuaified();
    //!实现任务二
    void t2_divideGroups(name2team_map_t& name2team);
    //!实现任务三的日程安排部分
    void t3_1_showGroupMatches();
    //!实现任务三的模拟比赛过程
    void t3_2_playGroupMatches();
    //!实现任务三的统计结果部分
    void t3_3_summaryGroupMatches();
    //!实现任务三的显示晋级部分
    void t3_4_showNextTeams();
    //!调用以上四个函数实现任务三
    void t3_groupStageMatches();
    //!实现任务四到七的日程安排部分
    void t4_t7_showKnockoutMatches(stage_t stage);
    //!实现任务四到七的模拟比赛过程
    void t4_t7_playKnockoutMatches(stage_t stage);
    //!实现任务四到七的统计结果部分
    void t4_t7_summaryKnockoutMatches(stage_t stage);
    //!实现任务三的显示晋级部分
    void t4_t7_showNextTeams(stage_t stage);
    //!调用以上四个函数实现任务四到七
    void t4_t7_knockoutStageMatches(stage_t stage);
    //!实现任务八
    void t8_showWholeGameStats();

    //!依据各个阶段比赛结果，对参赛队伍进行排名
    std::vector<NationalTeam*> sortRank(const std::vector<NationalTeam*> &to_sort, stage_t stage);
    //!各个阶段名称的首字母大写字符串
    std::vector<QString> stage_name_uppercase = {"Group", "Round of 16", "Quarter-finals", "Semi-finals", "Finals"};
    //!各个阶段名称的首字母小写字符串
    std::vector<QString> stage_name_lowercase = {"group", "round of 16", "quarter-finals", "semi-finals", "finals"};
    //!各个阶段第一次比赛的编号
    std::vector<int> stage_start_match = {0, 48, 56, 60, 62};
    //!各个阶段的比赛数
    std::vector<int> stage_match_num = {48, 8, 4, 2, 2};
    //!各个阶段剩余的队伍数目
    std::vector<int> stage_team_num = {32, 16, 8, 4, 4};
    //!各个阶段相关的要写入的文件名中的数字
    std::vector<int> stage_filename_idx = {16, 8, 4, 2, 1};
};

#endif // GAME_H
