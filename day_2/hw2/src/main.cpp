#include <QApplication>
#include <iostream>

#include "../include/day2_hw2/main_window.hpp"

int main(int argc, char* argv[]) //프로그램 시작, 메인 윈도우 실행
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
