#include "mainwindow.h"
#include <QCoreApplication>
#include "game.h"
int main(int argc, char *argv[])
{
    //std::default_random_engine e2;
    //std::normal_distribution<double> n(2.1, 2);
    //auto normal_rand = std::bind(n, e2);
    //int winner_score;
    //int c = 100;
    //while (c--) {
    //    winner_score = std::floor(normal_rand());
    //    std::cout << winner_score << std::endl;
    //}
    //while (1) {

    //}
    QCoreApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    while(1)
    {
        Game fifa;
        fifa.playGame();
    }


    return 0;
    //return a.exec();
}
