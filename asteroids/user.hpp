#ifndef USER_HPP_
#define USER_HPP_

#include "gamedata.hpp"
#include <iostream>
#include <stdio.h>

class user{
  private:
    int vida;
    int fase;
    int win;
    int max_vida;
    int max_fase;
  public:
  user();
  void setVida(int x);
  int getVida();
  void setFase(int x);
  int getFase();
  void setWin(int x);
  int getWin();
  void setMaxVida(int x);
  int getMaxVida();
  void setMaxFase(int x);
  int getMaxFase();

};

/*
  struct User{
  int vida;        // Attribute (int variable)
  int fase;  // Attribute (string variable)
  int win;

  int max_vida;
  int max_fase;

}; typedef struct User x;
*/

#endif