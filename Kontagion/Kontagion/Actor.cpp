#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//Actor - set alive to true and world to the parameter studentworld
Actor::Actor(int ID, double x, double y, Direction d, int depth, StudentWorld *studentworld) : GraphObject(ID, x, y, d, depth)
{
    alive = true;
    world = studentworld;
}

//Deconstructor: does nothing
Actor::~Actor()
{
    
}

//Dirt
Dirt::Dirt(double x, double y, StudentWorld *world) : Actor(IID_DIRT, x, y, 0, 1, world)
{
    //starts out alive; is either fully alive or fully dead
}

Dirt::~Dirt()
{
    
}

//Dirts do nothing
void Dirt::doSomething()
{
    return; //does nothing cuz it's dirt
}

//Pit
Pit::Pit(double x, double y, StudentWorld *world) : Actor(IID_PIT, x, y, 0, 1, world)
{
    regCount = 5;
    aggroCount = 3;
    ecoliCount = 2;
}

Pit::~Pit()
{
    
}

void Pit::doSomething()
{
    //no more stuff to spawn, set to dead
    if(regCount == 0 && aggroCount == 0 && ecoliCount == 0) {
        setDead();
        return;
    }
    
    //1 in 50 chance of spawning a bacteria
    int rand = randInt(1, 50);
    if(rand == 1)
    {
        //it's 1 in 50, now pick between one of reg, aggro, and ecoli to spawn
        int picker = randInt(1, 3);
        //picker being 1 corresponds to regular, 2 is aggressive, 3 is ecoli
        bool foundGoodCount = false;
        while(!foundGoodCount) {
            if(picker == 1 && regCount > 0) {
                foundGoodCount = true;
                //spawn regular salmonella
                getWorld()->addActor(new RegularSalmonella(getX(), getY(), getWorld()));
                
                //decrement regcount
                regCount--;
            } else if (picker == 2 && aggroCount > 0) {
                foundGoodCount = true;
                //spawn aggressive salmonella
                getWorld()->addActor(new AggressiveSalmonella(getX(), getY(), getWorld()));
                
                //decrement counter
                aggroCount--;
            } else if (picker == 3 && ecoliCount > 0) {
                foundGoodCount = true;
                //spawn ecoli
                getWorld()->addActor(new EColi(getX(), getY(), getWorld()));
                
                //decrement ecoli counter
                ecoliCount--;
            } else {
                //didn't match with a count that still existed, try again
                picker = randInt(1, 3);
            }
        }
        //bacteria has been born
        getWorld()->playSound(SOUND_BACTERIUM_BORN);
    }
}

//Agent
Agent::Agent(double x, double y, StudentWorld *world, int ID, Direction d, int hp) : Actor(ID, x, y, d, 0, world)
{
    hitPoints = hp;
}

Agent::~Agent()
{
}

//Socrates
Socrates::Socrates(StudentWorld *world) : Agent(0, VIEW_HEIGHT/2, world, IID_PLAYER, 0, 100)
{
    sprayCount = 20;
    flameCount = 5;
    killCount = 0;
    tickCount = 0;
}

Socrates::~Socrates()
{
}

//plays the player hurt sound
void Socrates::playHurtSound()
{
    getWorld()->playSound(SOUND_PLAYER_HURT);
}

//plays the player death sound
void Socrates::playDeathSound()
{
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

//repositions socrates for movement rotation
void Socrates::reposition()
{
    double newX = cos((getRealDirection())*PI()/180)*VIEW_RADIUS + VIEW_RADIUS;
    double newY = sin((getRealDirection())*PI()/180)*VIEW_RADIUS + VIEW_RADIUS;
    moveTo(newX, newY);
}

//shoots the spray
void Socrates::shootSpray()
{
    //no sprays left, exit.
    if(sprayCount <= 0) {
        return;
    }
    //otherwise, we spawn a spray 4 pixels in front of Socrates
    double x = cos((getDirection())*PI()/180)*SPRITE_WIDTH + getX();
    double y = sin((getDirection())*PI()/180)*SPRITE_WIDTH + getY();
    getWorld()->addActor(new Spray(x, y, getWorld(), getDirection()));
    
    //play sound and decrement spray count
    getWorld()->playSound(SOUND_PLAYER_SPRAY);
    sprayCount--;
}

//shoots the flames
void Socrates::shootFlames()
{
    //no charges left, return
    if(flameCount <= 0) {
        return;
    }
    
    Direction d = getDirection();
    double x;
    double y;
    //spawn 16 flames in 22 degree increments
    for(int i = 0; i < 16; i++) {
        x = cos(d*PI()/180)*SPRITE_WIDTH + getX();
        y = sin(d*PI()/180)*SPRITE_WIDTH + getY();
        getWorld()->addActor(new Flame(x, y, getWorld(), d));
        d += 22;
    }
    //decrement flame count and play sound
    flameCount--;
    getWorld()->playSound(SOUND_PLAYER_FIRE);
}

//the doSomething() method.
void Socrates::doSomething()
{
    //if socrates is dead, return immediately
    if(!isAlive()) return;
    
    int ch;
    if(getWorld()->getKey(ch))
    {
        
        switch(ch)
        {
            case KEY_PRESS_LEFT:
            {
                //counter clockwise
                setDirection(getDirection()+5);
                reposition();
                tickCount = 0;
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                //clockwise
                setDirection(getDirection()-5);
                reposition();
                tickCount = 0;
                break;
            }
            case KEY_PRESS_SPACE:
            {
                //shoots spray
                shootSpray();
                tickCount = 0;
                break;
            }
            case KEY_PRESS_ENTER:
            {
                //shoots flamethrower
                shootFlames();
                tickCount = 0;
                break;
            }
        }
    } else {
        //no key was pressed, and at least 5 ticks have passed, replenish charges by 1
        if(tickCount >= 5 && sprayCount < 20) {
            sprayCount++;
        } else {
            tickCount++;
        }
    }
}

//Activator
Activator::Activator(double x, double y, StudentWorld *world, int ID, Direction d) : Actor(ID, x, y, d, 1, world)
{
    
}

Activator::~Activator()
{
    
}

//Food
Food::Food(double x, double y, StudentWorld *world) : Activator(x, y, world, IID_FOOD, 90)
{
    
}

Food::~Food()
{
    
}

//does nothing
void Food::doSomething()
{
    //food doesn't do anything
    return;
}

//Bullet
Bullet::Bullet(double x, double y, StudentWorld *world, int ID, Direction d, int max, int dmg) : Activator(x, y, world, ID, d)
{
    distanceTravelled = 0;
    maxDist = max;
    damage = dmg;
}

Bullet::~Bullet()
{
    
}

void Bullet::moveForward()
{
    double newX = cos((getDirection())*PI()/180)*SPRITE_WIDTH + getX();
    double newY = sin((getDirection())*PI()/180)*SPRITE_WIDTH + getY();
    moveTo(newX, newY);
    increaseDistanceTravelled();
    if(distanceTravelled >= getMaxDist()) {
        setDead();
    }
}

bool Bullet::checkOverlap()
{
    return getWorld()->checkBulletOverlap(this, damage);
}

//Spray
Spray::Spray(double x, double y, StudentWorld *world, Direction d) : Bullet(x, y, world, IID_SPRAY, d, 112, 2)
{
}

Spray::~Spray()
{
    
}

void Spray::doSomething()
{
    //if it's dead, we just return
    if(!isAlive()) {
        return;
    }
    
    //check for collisions
    if(checkOverlap()) return;
    
    //otherwise, we move forward
    moveForward();
}

//Flames
Flame::Flame(double x, double y, StudentWorld *world, Direction d) : Bullet(x, y, world, IID_FLAME, d, 32, 5)
{
    
}

Flame::~Flame()
{
    
}

void Flame::doSomething()
{
    //if its not alive, does nothing
    if(!isAlive()) {
        return;
    }
    
    //check for collisions
    if(checkOverlap()) return;
    
    //otherwise, we move forward
    moveForward();
}


//Bacteria (base class)
Bacteria::Bacteria(double x, double y, StudentWorld *world, int ID, int hp) : Agent(x, y, world, ID, 90, hp)
{
    movementPlan = 0;
    foodEaten = 0;
}

Bacteria::~Bacteria()
{
    //all bacteria will increase socrates's kill count, increase score by 100
    getWorld()->increaseSocratesKillCount();
    getWorld()->increaseScore(100);
    //50% chance of spawning a food at death
    int rand = randInt(1, 2);
    if(rand == 1) {
        getWorld()->addActor(new Food(getX(), getY(), getWorld()));
    }
}

void Bacteria::lookForFood()
{
    //if it doesn't find food and move towards it, we reset the movement plan
    if(!getWorld()->findFoodDirection(this)) {
        resetMovementPlan();
    }
}

//tries to divide
bool Bacteria::tryDivide()
{
    //if it can divide, it'll calculate new x, new y coords and spawn a new bacteria there
    if(shouldDivide()) {
        double newX = getX();
        if(newX < VIEW_WIDTH/2) newX += SPRITE_RADIUS;
        else if(newX > VIEW_WIDTH/2) newX -= SPRITE_RADIUS;
        
        double newY = getY();
        if(newY < VIEW_HEIGHT/2) newY += SPRITE_RADIUS;
        else if(newY > VIEW_HEIGHT/2) newY -= SPRITE_RADIUS;
        
        spawnNewBacteria(newX, newY, getWorld());
        resetFoodEaten();
        
        getWorld()->increaseBacteria();
        
        return true;
    }
    //if it can't divide, return false
    return false;
}

//damages socrates with damage equivalent to dmg if it overlaps with socrates and returns true
//if it doesn't overlap, return false
bool Bacteria::tryDamagingSocrates(int dmg)
{
    if(getWorld()->overlapsWithSocrates(this)) {
        getWorld()->damageSocrates(dmg);
        return true;
    }
    return false;
}

//Other bacteria now
//Salmonella base:
Salmonella::Salmonella(double x, double y, StudentWorld *world, int hp) : Bacteria(x, y, world, IID_SALMONELLA, hp)
{
    
}

//all salmonella have the same death sound, so play it when it dies
Salmonella::~Salmonella()
{
    playDeathSound();
}

//plays the hurt sound
void Salmonella::playHurtSound()
{
    getWorld()->playSound(SOUND_SALMONELLA_HURT);
}

//plays the death sound
void Salmonella::playDeathSound()
{
    getWorld()->playSound(SOUND_SALMONELLA_DIE);
}

//tries to move forward if the movement plan is greater than 0.
//if it can't move forwardi n the direction it is facing, it sets its direction to a random angle
//then, it restes its movement plan.
//after, it returns true as long as the movement plan was greater than 0 to begin with
//if not, it returns false
bool Salmonella::tryMoving()
{
    if(getMovementPlan() > 0) {
        decrementMovement();
        if(!getWorld()->canMoveForward(getDirection(), this)) {
            int randAngle = randInt(0, 359);
            setDirection(randAngle);
            resetMovementPlan();
        }
        
        return true;
    }
    return false;
}

//regular salmonella
RegularSalmonella::RegularSalmonella(double x, double y, StudentWorld *world) : Salmonella(x, y, world, 4)
{
    
}

RegularSalmonella::~RegularSalmonella()
{
    
}
 
void RegularSalmonella::doSomething()
{
    //1. check if alive
    if(!isAlive()) {
        return;
    }
    
    bool shouldSkip = false;
    
    //2. check if overlaps w socrates. receive 1 point of dmg and skip to step 5
    if(tryDamagingSocrates(1)) {
        shouldSkip = true;
    }
    
    //3. total 3 food: divide and skip to step 5
    if(!shouldSkip) {
        if(tryDivide()) {
            shouldSkip = true;
        }
    }
    
    //4. food overlap and eat food
    if(!shouldSkip && getWorld()->checkFoodOverlap(this)) {
        eatFood();
    }
    
    //5. movement
    if(tryMoving()) {
        return;
    }
    
    //6. movement plan runs out, looks for food
    lookForFood();

    return;
}

//aggressive salmonella
AggressiveSalmonella::AggressiveSalmonella(double x, double y, StudentWorld *world) : Salmonella(x, y, world, 10)
{
    
}

AggressiveSalmonella::~AggressiveSalmonella()
{
    
}
 
void AggressiveSalmonella::doSomething()
{
    //1. check if alive
    if(!isAlive()) {
        return;
    }
    
    bool targetsSocrates = false;
    
    //2. if <= 72 pixels from Socrates
    if(calculateDistance(getX(), getY(), getWorld()->getSocratesX(), getWorld()->getSocratesY()) <= 72)
    {
        targetsSocrates = true;
        //still 2. try to move forward towards socrates
        //if blocked by dirt, won't move and will get stuck
        //then steps 3-5
        getWorld()->moveTowardsSocrates(this);
    }
    
    //keeps track if we should skip to step 6
    bool shouldSkip = false;
    
    //3. check if overlaps w socrates
    if(tryDamagingSocrates(2)) {
        if(targetsSocrates) return;
        shouldSkip = true;
    }
    
    //4. check food eaten
    if(!shouldSkip) {
        if(tryDivide()) {
            if(targetsSocrates) return;
            shouldSkip = true;
        }
    }
    
    //5.
    if(!shouldSkip && getWorld()->checkFoodOverlap(this)) {
        eatFood();
    }

    //don't go to step 6 if it targeted socrates earlier
    if(targetsSocrates) return;
    
    //6. movementplan > 0, so continue moving in the same direction
    if(tryMoving()) {
        return;
    }
    
    //7. look for food
    lookForFood();
    return;
}

//E coli
EColi::EColi(double x, double y, StudentWorld *world) : Bacteria(x, y, world, IID_ECOLI, 5)
{
    
}

EColi::~EColi()
{
    //plays the death sound when it dies
    playDeathSound();
}
 
void EColi::doSomething()
{
    //1. check if alive
    if(!isAlive()) {
        return;
    }
    
    bool shouldSkip = false;
    
    //2. check if overlaps with socrates
    if(tryDamagingSocrates(4)) {
        shouldSkip = true;
    }
    
    //3. check if it should divide
    if(!shouldSkip) {
        if(tryDivide()) {
            shouldSkip = true;
        }
    }
    
    //4. check if it overlaps w food and eat it
    if(!shouldSkip && getWorld()->checkFoodOverlap(this)) {
        eatFood();
    }
    
    //5. check if <= 256 pixels from Socrates and try to move towards him
    if(calculateDistance(getX(), getY(), getWorld()->getSocratesX(), getWorld()->getSocratesY()) <= 256)
    {
        Direction d = calculateDirection(getX(), getY(), getWorld()->getSocratesX(), getWorld()->getSocratesY());
        setDirection(d);
        for(int i = 0; i < 10; i++) {
            if(getWorld()->canMoveForward(getDirection(), this)) {
                return;
            }
            setDirection((getDirection()+10)%360);
        }
    }
    return;
}

//plays the hurt sound
void EColi::playHurtSound()
{
    getWorld()->playSound(SOUND_ECOLI_HURT);
}

//plays the death sound
void EColi::playDeathSound()
{
    getWorld()->playSound(SOUND_ECOLI_DIE);
}

//The following 3 functions are for spawning new bacteria depending on the type of bacteria.
void RegularSalmonella::spawnNewBacteria(double x, double y, StudentWorld *world)
{
    getWorld()->addActor(new RegularSalmonella(x, y, world));
}
void AggressiveSalmonella::spawnNewBacteria(double x, double y, StudentWorld *world)
{
    getWorld()->addActor(new AggressiveSalmonella(x, y, world));
}
void EColi::spawnNewBacteria(double x, double y, StudentWorld *world)
{
    getWorld()->addActor(new EColi(x, y, world));
}

//GOODIES
//base class first
Goodie::Goodie(double x, double y, StudentWorld *world, int ID) : Activator(x, y, world, ID, 0 /*direction is 0 for all goodies*/)
{
    int level = world->getLevel();
    lifetime = max(randInt(0, 300 - 10 * level - 1), 50);
    duration = 0;
}

Goodie::~Goodie()
{
    
}

//the common things do when a player gets goodies:
//sets to dead, increases score by numpoints
//and, if the goodie isn't a fungus, play the got goodie sound.
void Goodie::commonGoodieCollision(int points)
{
    setDead();
    getWorld()->increaseScore(points);
    if(points > 0) {
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
}

//if the duration reaches its lifetime, the goodie is set to dead. Otherwise, it
//increases the duration it has existed for.
void Goodie::durationCheck()
{
    if(getDuration() >= getLifetime()) {
        setDead();
    } else {
        increaseDuration();
    }
}

//restore health goodie
RestoreHealthGoodie::RestoreHealthGoodie(double x, double y, StudentWorld *world) : Goodie(x, y, world, IID_RESTORE_HEALTH_GOODIE)
{
    
}

RestoreHealthGoodie::~RestoreHealthGoodie()
{
    
}

void RestoreHealthGoodie::doSomething()
{
    //1. checks if alive
    if(!isAlive()) {
        return;
    }
    
    //2. determine if overlaps w/ Socrates
    if(getWorld()->overlapsWithSocrates(this))
    {
        //250 points, kill the goodie, play sound, health is full, return
        commonGoodieCollision(250);
        
        getWorld()->resetSocratesHP();
        return;
    }
    
    //3. check lifetime or increment lifetime thing
    durationCheck();
}

//FlamethrowerGoodie
FlamethrowerGoodie::FlamethrowerGoodie(double x, double y, StudentWorld *world) : Goodie(x, y, world, IID_FLAME_THROWER_GOODIE)
{
    
}

FlamethrowerGoodie::~FlamethrowerGoodie()
{
    
}

void FlamethrowerGoodie::doSomething()
{
    //1. checks if alive
    if(!isAlive()) {
        return;
    }
    
    //2. determine if overlaps w/ Socrates
    if(getWorld()->overlapsWithSocrates(this))
    {
        //300 points, kill the goodie, play sound, 5 more flames, return
        commonGoodieCollision(300);
        
        getWorld()->increaseFlamecount();
        return;
    }
    
    //3. check lifetime or increment lifetime thing
    durationCheck();
}

//ExtraLifeGoodie
ExtraLifeGoodie::ExtraLifeGoodie(double x, double y, StudentWorld *world) : Goodie(x, y, world, IID_EXTRA_LIFE_GOODIE)
{
    
}

ExtraLifeGoodie::~ExtraLifeGoodie()
{
    
}

void ExtraLifeGoodie::doSomething()
{
    //1. checks if alive
    if(!isAlive()) {
        return;
    }
    
    //2. determine if overlaps w/ Socrates
    if(getWorld()->overlapsWithSocrates(this))
    {
        //500 points, kill the goodie, play sound, increase lives, return
        commonGoodieCollision(500);
        
        getWorld()->incLives();
        return;
    }
    
    //3. check lifetime or increment lifetime thing
    durationCheck();
}

//Fungus
Fungus::Fungus(double x, double y, StudentWorld *world) : Goodie(x, y, world, IID_FUNGUS)
{
    
}

Fungus::~Fungus()
{
    
}

void Fungus::doSomething()
{
    //1. checks if alive
    if(!isAlive()) {
        return;
    }
    
    //2. determine if overlaps w/ Socrates
    if(getWorld()->overlapsWithSocrates(this))
    {
        //-50 points, kill the goodie, -20 hp, return
        commonGoodieCollision(-50);
        
        getWorld()->damageSocrates(20);
        return;
    }
    
    //3. check lifetime or increment lifetime thing
    durationCheck();
}
