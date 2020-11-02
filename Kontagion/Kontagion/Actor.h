#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

//Actor is the base class for all the other actors in this game.
class Actor : public GraphObject
{
public:
    //Constructor
    Actor(int ID, double x, double y, Direction d, int depth, StudentWorld *studentworld);
    
    //virtual destructor
    virtual ~Actor();
    
    //pure virtual doSomething()
    virtual void doSomething() = 0;
    
    //check if actor is alive
    virtual bool isAlive() {
        return alive;
    }
    
    //set actor to dead
    void setDead() {
        alive = false;
    }
    
    //gets pointer to the world
    StudentWorld* getWorld() {
        return world;
    }

    //whether or not overlap matters when spawning in
    virtual bool overlapMattersSpawn() {
        return true;
    }
    
    //whether or not the actor can be damaged
    virtual bool isDamageable() {
        return true;
    }
    
    //whether or not the actor has hp
    virtual bool hasHP() { return false; }
    
    //decreases HP. won't do anything for base class, but applies to Agent subclass
    virtual void decreaseHP(int howMuch) {howMuch = howMuch; return;}
    
    //whether or not the actor can be consumed
    virtual bool isConsumable() { return false; }
    
    //if the actor is a barrier to movement
    virtual bool isUnmoving() { return false; }
    
private:
    bool alive;
    StudentWorld *world;
};

//DERIVED CLASSES

//Agent
class Agent : public Actor
{
public:
    //Constructor and Destructor
    Agent(double x, double y, StudentWorld *world, int ID, Direction d, int hp);
    virtual ~Agent();
    
    //doSomething method - pure virtual
    virtual void doSomething() = 0;
    
    //if HP>0, the agent is alive
    virtual bool isAlive() {
        return hitPoints > 0;
    }
    
    //decreases hp by how much
    virtual void decreaseHP(int howMuch) {
        hitPoints -= howMuch;
        playHurtSound();
    }
    
    //gets how much hp an actor has
    int getHP() {
        return hitPoints;
    }
    
    //sets hitPoints to hp
    void setHP(int hp) {
        hitPoints = hp;
    }
    
    //the agent has hp
    virtual bool hasHP() { return true; }
    
    //play sounds: pure virtual since different agents play different sounds
    virtual void playHurtSound() = 0;
    virtual void playDeathSound() = 0;
private:
    int hitPoints;
};

//Dirt
class Dirt : public Actor
{
public:
    //constructor and destructor
    Dirt(double x, double y, StudentWorld *world);
    virtual ~Dirt();
    //the required doSomething() method
    virtual void doSomething();
    
    //dirts can overlap when spawning in
    virtual bool overlapMattersSpawn() {
        return false;
    }
    
    //dirts are barriers to movement
    virtual bool isUnmoving() { return true; }
private:
};

//Socrates:
class Socrates : public Agent
{
public:
    //constructor and destructor
    Socrates(StudentWorld *world);
    virtual ~Socrates();
    
    //what socrates does every tick
    virtual void doSomething();
    
    //gets proper direction for Socrates rotation
    Direction getRealDirection() {
        return (getDirection()+180) % 360;
    }
    
    //repositions socrates based on direction
    void reposition();
    
    //shoots a spray
    void shootSpray();
    
    //shoots his flamethrower
    void shootFlames();
    
    //gets the spray count
    int getSprayCount() {return sprayCount;}
    
    //gets the count for flame charges
    int getFlameCount() {return flameCount;}
    
    //adds 5 to the flame count
    void addFlames() { flameCount += 5; }
    
    //increase the kill count
    void increaseKillCount() {killCount++;}
    
    //gets the kill count
    int getKillCount() {return killCount;}
    
    //resets hp to 100
    void resetHP() { setHP(100); }
    
    //plays hurt and death sounds
    virtual void playHurtSound();
    virtual void playDeathSound();
private:
    int sprayCount;
    int flameCount;
    int killCount;
    int tickCount;
};

//Pit
class Pit : public Actor
{
public:
    //constructor and deconstructor
    Pit(double x, double y, StudentWorld *world);
    virtual ~Pit();
    
    //the dosomething method
    virtual void doSomething();
    
    //is not damageable
    virtual bool isDamageable() { return false; }
private:
    int regCount;
    int aggroCount;
    int ecoliCount;
};

/*THE FOLLOWING IS ACTIVATOR CLASSES*/
//Activating Object
class Activator : public Actor
{
public:
    //constructor and destructor
    Activator(double x, double y, StudentWorld *world, int ID, Direction d);
    virtual ~Activator();
    
    //still pure virtual because different Activators do different things
    virtual void doSomething() = 0;
    
    //by default, not damageable. Doesn't apply to every activator (e.g. goodies)
    virtual bool isDamageable() {return false;}
private:
};

//Food (pizza)
class Food : public Activator
{
public:
    //Constructor and destructor
    Food(double x, double y, StudentWorld *world);
    virtual ~Food();
    
    //the dosomething method (does nothing)
    virtual void doSomething();
    
    //food is consumable
    virtual bool isConsumable() { return true; }
private:
};

class Bullet : public Activator
{
public:
    //Constructor and destructor
    Bullet(double x, double y, StudentWorld *world, int ID, Direction d, int max, int damage);
    virtual ~Bullet();
    
    //different bullets do different things
    virtual void doSomething() = 0;
    
    //moves forward 8 pixels and then dies if it reaches the max distance
    void moveForward();
    
    //returns the max distance the bullet can travel
    int getMaxDist() {return maxDist;}
    
    //increases the total distance the bullet has travelled
    void increaseDistanceTravelled() {distanceTravelled += SPRITE_WIDTH;}
    
    //checks to see if the bullet overlaps with something that can be damaged, and damage it
    bool checkOverlap();
    
private:
    int distanceTravelled;
    int maxDist;
    int damage;
};

//flame
class Flame : public Bullet
{
public:
    //constructor, destructor, and doSomething() methods.
    Flame(double x, double y, StudentWorld *world, Direction d);
    virtual ~Flame();
    virtual void doSomething();
private:
};

//spray (bullets)
class Spray : public Bullet
{
public:
    //constructor, destructor, and doSomething() methods.
    Spray(double x, double y, StudentWorld *world, Direction d);
    virtual ~Spray();
    virtual void doSomething();
private:
};

//goodie: base class for the goodies
class Goodie : public Activator
{
public:
    //constructor and destructor
    Goodie(double x, double y, StudentWorld *world, int ID);
    virtual ~Goodie();
    
    //pure virtual because goodies do different things
    virtual void doSomething() = 0;
    
    //goodies can be damaged
    virtual bool isDamageable() { return true; }
    
    //gets the duration
    int getDuration() { return duration; }
    
    //gets the max duration it can exist for (lifetime)
    int getLifetime() { return lifetime; }
    
    //increments duration
    void increaseDuration() { duration++; }
    
    //does the common goodie stuff when a player gets the goodie
    void commonGoodieCollision(int points);
    
    //sets to dead if max duration or increments duration
    void durationCheck();
private:
    int duration;
    int lifetime;
};

//Restorehealthgoodie
class RestoreHealthGoodie : public Goodie
{
public:
    //constructor, destructor, and doSomething methods.
    RestoreHealthGoodie(double x, double y, StudentWorld *world);
    virtual ~RestoreHealthGoodie();
    virtual void doSomething();
private:
};

//FlamethrowerGoodie
class FlamethrowerGoodie : public Goodie
{
public:
    //constructor, destructor, and doSomething methods.
    FlamethrowerGoodie(double x, double y, StudentWorld *world);
    virtual ~FlamethrowerGoodie();
    virtual void doSomething();
private:
};

//ExtraLifeGoodie
class ExtraLifeGoodie : public Goodie
{
public:
    //constructor, destructor, and doSomething methods.
    ExtraLifeGoodie(double x, double y, StudentWorld *world);
    virtual ~ExtraLifeGoodie();
    virtual void doSomething();
private:
};

//Fungus
class Fungus : public Goodie
{
public:
    //constructor, destructor, and doSomething methods.
    Fungus(double x, double y, StudentWorld *world);
    virtual ~Fungus();
    virtual void doSomething();
private:
};

/*THE FOLLOWING IS BACTERIA-RELATED CLASSES*/
//base class for bacteria
class Bacteria : public Agent
{
public:
    //Constructor and destructor
    Bacteria(double x, double y, StudentWorld *world, int ID, int hp);
    virtual ~Bacteria();
    
    //doSomething is still pure virtual
    virtual void doSomething() = 0;
    
    //increases food eaten
    void eatFood() { foodEaten++; }
    
    //whether or not the bacteria should divide
    bool shouldDivide() { return foodEaten == 3; }
    
    //resets the food eaten
    void resetFoodEaten() { foodEaten = 0; }
    
    //gets movementPlan
    int getMovementPlan() { return movementPlan; }
    
    //decrements movement plan
    void decrementMovement() { movementPlan--; }
    
    //resets movement plan to 10
    void resetMovementPlan() { movementPlan = 10; }
    
    //tries to look for food and move towards it, or changes direction
    void lookForFood();
    
    //tries to divide into 2 bacteria
    bool tryDivide();
    
    //damages socrates if overlaps
    bool tryDamagingSocrates(int dmg);
    
    //pure virtual functions to spawn new bacteria and play sounds
    virtual void spawnNewBacteria(double x, double y, StudentWorld *world) = 0;
    virtual void playHurtSound() = 0;
    virtual void playDeathSound() = 0;
private:
    int movementPlan;
    int foodEaten;
};

class EColi : public Bacteria
{
public:
    //constructor and destructor
    EColi(double x, double y, StudentWorld *world);
    virtual ~EColi();
    //the dosomething method
    virtual void doSomething();
    //spawns a new ecoli
    virtual void spawnNewBacteria(double x, double y, StudentWorld *world);
    //playing the hurt and death sounds
    virtual void playHurtSound();
    virtual void playDeathSound();
private:
};

//base class for salmonella
class Salmonella : public Bacteria
{
public:
    //constructor and destructor
    Salmonella(double x, double y, StudentWorld *world, int hp);
    virtual ~Salmonella();
    
    //dosomething is virtual
    virtual void doSomething() = 0;
    
    //tries to move forward if the movement plan is greater than 0.
    bool tryMoving();
    
    //spawning is still pure virtual because we don't know the type of salmonella to spawn
    virtual void spawnNewBacteria(double x, double y, StudentWorld *world) = 0;
    
    //plays the hurt and death sounds
    virtual void playHurtSound();
    virtual void playDeathSound();
private:
};

class RegularSalmonella : public Salmonella
{
public:
    //constructor and destructor
    RegularSalmonella(double x, double y, StudentWorld *world);
    virtual ~RegularSalmonella();
    
    //the dosomething method!
    virtual void doSomething();
    
    //spawns a new regular salmonella at (x, y) in world
    virtual void spawnNewBacteria(double x, double y, StudentWorld *world);
private:
};

class AggressiveSalmonella : public Salmonella
{
public:
    //constructor and destructor
    AggressiveSalmonella(double x, double y, StudentWorld *world);
    virtual ~AggressiveSalmonella();
    
    //the dosomething method!
    virtual void doSomething();
    
    //spawns a new aggressive salmonella at (x, y) in world.
    virtual void spawnNewBacteria(double x, double y, StudentWorld *world);
private:
};


#endif // ACTOR_H_
