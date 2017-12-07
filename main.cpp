#include <QCoreApplication>
#include "game.h"
int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    std::string choice = "0";
    while (choice != "q") {
        Game fifa;
        choice = fifa.playGame(choice);
    }
    return 0;
    //return a.exec();
}
