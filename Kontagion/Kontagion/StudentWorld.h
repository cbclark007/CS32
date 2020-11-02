#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>
#include <string>
using namespace std;

//helper method headers
double PI();
bool doesOverlap(double x1, double y1, double x2, double y2);
bool doesMovementOverlap(double x1, double y1, double x2, double y2);
double calculateDistance(double x1, double y1, double x2, double y2);
int calculateDirection(double x1, double y1, double x2, double y2);
bool doesOverlapSpawn(double x, double y, list<Actor*> actors, bool callerMatters);
void getCoords(double *arr, list<Actor*> actors, bool callerMatters);
void getGoodieCoords(double *arr);

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    
    //init, move, and cleanUp as required by the spec
    //init spawns socrates, pits, food, and dirt.
    virtual int init();
    //move makes all the actors doSomething(), cleans up dead actors, adds new goodies,
    //and updates text
    virtual int move();
    //frees all actors
    virtual void cleanUp();
    
    //deconstructor
    virtual ~StudentWorld();
    
    //add actor to actors
    void addActor(Actor *a);
    
    //gets the game stats text
    string getGameText();
    
    bool overlapsWithSocrates(Actor *a);
    bool checkFoodOverlap(Actor *a);
    bool canMoveForward(Direction d, Actor *a);
    bool findFoodDirection(Actor *a);
    void moveTowardsSocrates(Actor *a);
    void damageSocrates(int howMuch) { s->decreaseHP(howMuch); }
    void resetSocratesHP();
    bool checkBulletOverlap(Actor *a, int damage);
    void increaseSocratesKillCount();
    
    //increases the count of flame charges for Socrates
    void increaseFlamecount() { s->addFlames(); }
    //increases the count for number of bacteria in the game
    void increaseBacteria() { numBacteria++; }
    //get Socrates's x and y coordinates
    double getSocratesX() { return s->getX(); }
    double getSocratesY() { return s->getY(); }
    
private:
    list<Actor*> actors;
    Socrates* s;
    int numBacteria;
};

#endif // STUDENTWORLD_H_
