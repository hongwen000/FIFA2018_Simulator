#ifndef PLAYER_H
#define PLAYER_H
#include "base.h"

struct Player;

QString playerNameEx(const Player *player);

enum class PlayerRole{
    GK,DF,MF,FW, NOT_ASSIGNED
};

struct Player
{
    //! 构造函数，
    Player() : scoreNumber(0), is_captain(false), is_vice_captain(false), role(PlayerRole::NOT_ASSIGNED) {}
    QString name; 			//! 球员姓名
    QString nation; 		//! 球员国籍
    int ID; 				//! 球员在FIFA数据库中的id
    int index; 				//! 球员编号
    int overall; 			//! 综合能力
    int potential; 			//! 球员潜力
    int scoreNumber; 		//! 生涯进球
    bool is_captain; 		//! 是否队长
    bool is_vice_captain; 	//! 是否副队长
    PlayerRole role; 		//! 球员角色，包括GK，DF，MF，FW
    bool can_play[4]; 		//! 能否胜任上述四个角色
    static QString getRoleString(PlayerRole role); //! 返回角色对应的字符串
    int getPower() const; //! 返回球员临场能力值
};

#endif // PLAYER_H
