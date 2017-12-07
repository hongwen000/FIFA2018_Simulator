#include <QCoreApplication>
#include "game.h"
int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    Game fifa;
    std::string choice = "0";
    while (choice != "q") {
        choice = fifa.playGame(choice);
    }
    return 0;
    //return a.exec();
}
