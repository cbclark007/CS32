#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    s = nullptr;
}


//various helper functions

//distance function. Calculates the distance between (x1, y1) and (x2, y2).
double calculateDistance(double x1, double y1, double x2, double y2)
{
    double x = abs(x1-x2);
    double y = abs(y1-y2);
    return sqrt(x*x + y*y);
}

//calculates the angle to go from x1, y1 to x2, y2
int calculateDirection(double x1, double y1, double x2, double y2)
{
    double radian = atan2((y1-y2), (x1-x2));
    int degrees = radian*180/PI();
    return (degrees+180)%360;
}

//checks to see if two coordinates overlap (within 8 pixels)
bool doesOverlap(double x1, double y1, double x2, double y2)
{
    double dist = calculateDistance(x1, y1, x2, y2);
    if(dist > 2*SPRITE_RADIUS)
    {
        return false;
    }
    return true;
}

//checks to see if two coordinates overlap for movement (within 4 pixels)
bool doesMovementOverlap(double x1, double y1, double x2, double y2)
{
    double dist = calculateDistance(x1, y1, x2, y2);
    if(dist > SPRITE_RADIUS)
    {
        return false;
    }
    return true;
}

//Takes coords (x, y), the list of actors, and
//a boolean that describes whether or not it matters for an actor to have overlap.
//Then, it loops through the actors to see if any of them overlap where they shouldn't.
//if they do overlap, return true. Otherwise, return false.
bool doesOverlapSpawn(double x, double y, list<Actor*> actors, bool callerMatters)
{
    if(actors.size() == 0) {
        return false;
    }
    list<Actor*>::iterator it = actors.begin();
    for(; it != actors.end(); it++) {
        if(callerMatters || (*it)->overlapMattersSpawn()) {
            double dirtX = (*it)->getX();
            double dirtY = (*it)->getY();
            if(doesOverlap(x, y, dirtX, dirtY)) return true;
        }
    }
    return false;
}

//get coords will set arr[0] and arr[1] to a coordinate pair (x, y) respectively
//that is a working coordinate available to be spawned in.
void getCoords(double *arr, list<Actor*> actors, bool callerMatters)
{
    int coord1 = randInt(0, 255);
    int coord2 = randInt(0, 255);
    while(doesOverlapSpawn(coord1, coord2, actors, callerMatters) || calculateDistance(VIEW_WIDTH/2, VIEW_HEIGHT/2, coord1, coord2) >= VIEW_RADIUS - 9)
    {
        coord1 = randInt(0, 255);
        coord2 = randInt(0, 255);
    }
    arr[0] = coord1;
    arr[1] = coord2;
}

//goodie spawn coords
void getGoodieCoords(double *arr) {
    int randAngle = randInt(0, 359);
    double x = cos(randAngle*PI()/180)*VIEW_RADIUS + VIEW_RADIUS;
    double y = sin(randAngle*PI()/180)*VIEW_RADIUS + VIEW_RADIUS;
    arr[0] = x;
    arr[1] = y;
}

//gets a value for PI
double PI() {
    return 4 * atan(1);
}

//end of helper functions

//true if overlaps with socrates. Otherwise, returns false.
bool StudentWorld::overlapsWithSocrates(Actor *a)
{
    if(doesOverlap(a->getX(), a->getY(), s->getX(), s->getY())) return true;
    return false;
}

//iterates through the actors list and checks to see if a overlaps with any of them
//and the actor it overlaps with is alive and damageable.
//if so, either damage or kill it. then, set a to dead and return true
//otherwise, returns false.
bool StudentWorld::checkBulletOverlap(Actor *a, int damage)
{
    for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
        //if damageable and overlaps
        if((*it)->isAlive() && (*it)->isDamageable() && doesOverlap(a->getX(), a->getY(), (*it)->getX(), (*it)->getY())) {
            //if it has hp, decrement hp.
            if((*it)->hasHP()) {
                (*it)->decreaseHP(damage);
            } else {
                //otherwise, just delete it
                (*it)->setDead();
            }
            //get rid of the flame and exit the loop
            a->setDead();
            return true;
        }
    }
    return false;
}

//loops through all the actors in the list
//if it finds that a overlaps with an actor that is both consumable and alive,
//it means it has found a food. Set that food to dead and return true.
//otherwise, return false.
bool StudentWorld::checkFoodOverlap(Actor *a)
{
    for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
        //if consumable and overlaps
        if((*it)->isConsumable() && doesOverlap(a->getX(), a->getY(), (*it)->getX(), (*it)->getY()) && (*it)->isAlive()) {
            //remove pizza and return true
            (*it)->setDead();
            return true;
        }
    }
    return false;
}

//the actor a finds the direction towards the closest food item within 128 pixels.
//it then tries to move towards that food, and returns true if it moves forward.
//in all other cases, it sets its direction to a random direction and returns false.
bool StudentWorld::findFoodDirection(Actor *a)
{
    Direction d;
    Direction shortestDirection = 0;
    bool foundFood = false;
    double distance;
    double shortestDistance = 10000;
    for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
        //if consumable and overlaps
        distance = calculateDistance(a->getX(), a->getY(), (*it)->getX(), (*it)->getY());
        d = calculateDirection(a->getX(), a->getY(), (*it)->getX(), (*it)->getY());
        if((*it)->isConsumable() && distance <= 128 && (*it)->isAlive()) {
            //found a food within the thing
            foundFood = true;
            if(distance < shortestDistance) {
                shortestDistance = distance;
                shortestDirection = d;
            }
        }
    }
    
    //food can't be found, rand direction
    if(!foundFood) {
        a->setDirection(randInt(0, 359));
        return false;
    } else {
        if(canMoveForward(shortestDirection, a)) {
            a->setDirection(shortestDirection);
            return true;
        } else {
            a->setDirection(randInt(0, 359));
            return false;
        }
    }
}

//tries to move actor a towards socrates
void StudentWorld::moveTowardsSocrates(Actor *a)
{
    Direction d = calculateDirection(a->getX(), a->getY(), s->getX(), s->getY());
    a->setDirection(d);
    canMoveForward(a->getDirection(), a);
}

//a tries to move forward in direction d. Returns false if collides
//otherwise, it moves forward and returns true.
bool StudentWorld::canMoveForward(Direction d, Actor *a)
{
    double destX = cos(d*PI()/180)*3 + a->getX();
    double destY = sin(d*PI()/180)*3 + a->getY();
    for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
        //if unmoving, overlaps, and is alive
        if(((*it)->isUnmoving() && doesMovementOverlap(destX, destY, (*it)->getX(), (*it)->getY()) && (*it)->isAlive()) || calculateDistance(destX, destY, VIEW_WIDTH/2, VIEW_HEIGHT/2) >= VIEW_RADIUS) {
            //collision happens and we return false
            return false;
        }
    }
    //no collisions
    a->moveTo(destX, destY);
    return true;
}

// constructs a representation of the current level and populates it with initial objects using
// various data structures. It is automatically called by provided code either when the game first
// starts, whem the player completes the current level and advances to a new level (that needs to be
// initialized), or when the user loses a life (but has more lives left) and the game is ready to restart
// at the current level
// returns GWSTATUS_CONTINUE_GAME as stated in the spec
int StudentWorld::init()
{
    //socrates goes on
    s = new Socrates(this);
    
    double *coords = new double[2];
    //First, add pits
    for(int i = 0; i < getLevel(); i++) {
        getCoords(coords, actors, true);
        actors.push_back(new Pit(coords[0], coords[1], this));
    }
    
    //then, add food
    int numFood = min(5 * getLevel(), 25);
    for(int i = 0; i < numFood; i++) {
        getCoords(coords, actors, true);
        actors.push_back(new Food(coords[0], coords[1], this));
    }
    
    //finally, put dirts on screen
    int numDirts = max(180-20*getLevel(), 20);
    for(int i = 0; i < numDirts; i++) {
        getCoords(coords, actors, false);
        actors.push_back(new Dirt(coords[0], coords[1], this));
    }
    
    numBacteria = 10*getLevel();
    
    return GWSTATUS_CONTINUE_GAME;
}

//returns the game stats text displayed at the top of the screen
string StudentWorld::getGameText() {
    ostringstream gss;
    gss.fill('0');
    gss << "Score: ";
    if(getScore() >= 0) {
        gss << setw(6) << getScore() << "  ";
    } else {
        gss << "-" << setw(5) << getScore()*-1 << "  ";
    }
    gss.fill(' ');
    gss << "Level: " << setw(2) << getLevel() << "  ";
    gss << "Lives: " << setw(2) << getLives() << "  ";
    gss << "Health: " << setw(3) << s->getHP() << "  ";
    gss << "Sprays: " << setw(3) << s->getSprayCount() << "  ";
    gss << "Flames: " << setw(3) << s->getFlameCount() << "  ";
    return gss.str();
}

// each time it is called, runs a single tick of the game
//move makes all the actors doSomething(), cleans up dead actors, adds new goodies,
//and updates text
int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    //decLives();
    
    //socrates does something
    s->doSomething();
    
    //every actor does something
    for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
        if((*it)->isAlive()) (*it)->doSomething();
        
        //socrates is dead
        if(!(s->isAlive())) {
            decLives();
            s->playDeathSound();
            return GWSTATUS_PLAYER_DIED;
        }
        
        if(s->getKillCount() == numBacteria) {
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    //clean up actors that have died in this tick
    Actor *del;
    for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
        if(!(*it)->isAlive()) {
            del = (*it);
            it = actors.erase(it);
            delete del;
            del = nullptr;
            it--;
        }
    }
    
    //adding goodies and fungi
    int chanceFungus = max(510-getLevel()*10, 200);
    int rand = randInt(0, chanceFungus-1);
    if(rand == 0) {
        double *coords = new double[2];
        getGoodieCoords(coords);
        actors.push_back(new Fungus(coords[0], coords[1], this));
    }
    
    int chanceGoodie = max(510-getLevel()*10, 250);
    rand = randInt(0, chanceGoodie-1);
    if(rand == 0) {
        //60% for restore health, 30% for flamethrower, 10% for extra life
        rand = randInt(1, 10);
        double *coords = new double[2];
        getGoodieCoords(coords);
        if(rand >= 1 && rand <= 6) {
            //restore health
            actors.push_back(new RestoreHealthGoodie(coords[0], coords[1], this));
        } else if (rand >= 7 && rand <= 9) {
            //flamethrower
            actors.push_back(new FlamethrowerGoodie(coords[0], coords[1], this));
        } else {
            //extra life!
            actors.push_back(new ExtraLifeGoodie(coords[0], coords[1], this));
        }
    }
    
    //sets the game stats text
    setGameStatText(getGameText());
    
    return GWSTATUS_CONTINUE_GAME;
}

// called when Socrates completes a level or loses a life. Frees all actors currently in the game.
void StudentWorld::cleanUp()
{
    //delete actors in the list first
    while(!actors.empty()) {
        Actor *a = actors.back();
        actors.pop_back();
        delete a;
    }
    
    //then delete socrates
    if(s != nullptr) {
        delete s;
        s = nullptr;
    }
    
    //reset bacteria in the level
    numBacteria = 10*getLevel();
}

//does the same thing as cleanup
StudentWorld::~StudentWorld()
{
    cleanUp();
}

void StudentWorld::addActor(Actor *a)
{
    actors.push_back(a);
}

void StudentWorld::resetSocratesHP()
{
    s->resetHP();
}

void StudentWorld::increaseSocratesKillCount()
{
    s->increaseKillCount();
}
