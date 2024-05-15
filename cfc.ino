#include <TFT_eSPI.h>
#include <SPI.h>

// include .h files for image
#include "cat.h"
#include "fish1.h"
#include "water.h"
#include "gamelogo.h"
#include "catnom.h"
#include "catsad.h"

 
// define screen specs
#define SCREENWIDTH 320
#define SCREENHEIGHT 480
#define HEIGHT_2  240 //half screen height
#define WIDTH_2   160 //half screen width

// define joystick pin specifications
#define JOYSTICK_X_PIN A7
#define JOYSTICK_Y_PIN A6
#define JOYSTICK_BUTTON_PIN 34

// set on/off switch pin
const int switchPin = 47;

// define fish dimensions
int fishWidth = 27;
int fishHeight = 19;

// define water dimensions
int waterWidth = 18;
int waterHeight = 18;


// number of images on the screen
const int numImages = 5;
int xPos[numImages];
int yPos[numImages];
int fallSpeed[numImages];
int limitY = 393;

const uint16_t images[] = {fish, water};

unsigned long lastImageTime = 0;
unsigned long nextImageTime = 0;
const unsigned long timeDelay = 500;
long prevMillis = 0;
int minutes = 0;
int seconds = 0;


// initialize cursor location
int cursorX = 0;
int cursorY = 392;

int rectX = 0;
int fishY = 0;
int waterY = 0;
int fishCount = 0;

// declare variables for x, y coordinates
// array for setting up the screen in a grid-like fashion
const int xSize = 7; // size of x array
int fishX[xSize] = {9, 55, 101, 147, 193, 239, 285};
int catX[xSize] = {0, 46, 92, 138, 184, 230, 276};


// set up intervals for when an object is printed on screen
const int amountOfIntervals = 2;
unsigned long currentMillis = 0;
int intervals[amountOfIntervals] = {1000, 3000};

int blinkInterval[2] = {800, 1600};
unsigned long prevJoy[2] = {0, 0};

int elementX = 0;

// determine if joystick is to right or left
bool left = false;
bool right = false;

bool found = false;

bool firstFish = true;
bool firstWater = true;
bool isFish = true;
bool isWater = false;
bool checkWater = false;
bool checkFish = false;

bool gameGo = false;
bool gameOver = false;

int previousFishCount = 0;

static short score;
static short tmpx, tmpy;

TFT_eSPI tft = TFT_eSPI();

int xGridValue[7] = {9, 55, 101, 147, 193, 239, 285};

class Object
{
  public:
    int x;
    int y; 
    int speed;

    Object update(int x, int y, int speed) {
      this->x = x;
      this->y = y;
      this->speed = speed;
    }

    void resetObject() {
      this->x = random(0, 7);
      this->y = random(-100, 0);
      this->speed = random(3, 5);
    }

    void moveObject() {
      this->y += this->speed;     

      if(this->y > SCREENHEIGHT+4) {
        resetObject();
      }

    }

    int getX() {
      return this->x;
    }

    int getY() {
      return this->y;
    }

    void showObject(bool isFish) {
      moveObject();
      if(isFish) {
        tft.fillRect(xGridValue[this->x], this->y-fishHeight, fishWidth, fishHeight, TFT_BLACK);
        tft.pushImage(xGridValue[this->x], this->y, fishWidth, fishHeight, fish);
      }
      else {
        tft.fillRect(xGridValue[this->x], this->y-waterHeight, waterWidth, waterHeight, TFT_BLACK);
        tft.pushImage(xGridValue[this->x], this->y, waterWidth, waterHeight, water);        
      }
    }

    void deleteObject(bool check) {
      if(check) {
        tft.fillRect(xGridValue[this->x], this->y - fishHeight, fishWidth, fishHeight, TFT_BLACK);
      }
    }

};


// create Object instances
Object fishObject;
Object waterObject1;
Object waterObject2;
Object waterObject3;
Object waterObject4;
Object waterObject5;
int numObjects = 0; // keep track of objects on screen



/*----------------------
------ FUNCTIONS -------
---------------------- */
void game_start() {
  gameOver = false;
  fishCount = 0;
  
  if (!gameGo) {
    
    tft.fillRect(10, HEIGHT_2 - 102, SCREENWIDTH - 20, 1, TFT_WHITE); //OMG!
          //note: (x,y,x2,y2,colour)
    tft.fillRect(10, HEIGHT_2 - 8, SCREENWIDTH - 20, 1, TFT_WHITE); //OMG! make top and bottom lines
      //CHANGE THESE COORDINATES ONCE TESTING
    tft.pushImage(WIDTH_2-90, HEIGHT_2-100, 180, 90, gamelogo);
    tft.pushImage(WIDTH_2+92, HEIGHT_2-152, 68, 49, catnom);

    currentMillis = millis();
    for(int k = 0; k < 2; k++) {
      if(currentMillis - prevJoy[k] >= blinkInterval[k]) {
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2); //OMG!
        tft.setCursor(WIDTH_2-76, HEIGHT_2 + 34);
        if (k == 0) {
          tft.fillRect(WIDTH_2-76, HEIGHT_2 + 34 , 200, 30, TFT_BLACK);
        }
        if (k == 1) {
          tft.setCursor(WIDTH_2-76, HEIGHT_2 + 34);
          tft.println("press joystick");
        }
        
        prevJoy[k] = currentMillis;
      }
      
    }
  }

  if (digitalRead(JOYSTICK_BUTTON_PIN) == 0) {
    gameGo = true;
    game_init();
  }

}

void game_init() {
  tft.fillScreen(TFT_BLACK);
  tft.pushImage(cursorX, cursorY, 46, 88, cat);
}

void game_over(){
  tft.fillRect(10, HEIGHT_2 - 55, SCREENWIDTH - 20, 1, TFT_WHITE);
  tft.fillRect(10, HEIGHT_2 - 8, SCREENWIDTH - 20, 1, TFT_WHITE);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(4); 
  tft.setCursor(WIDTH_2-100, HEIGHT_2 - 45);
  tft.println("GAME OVER");

  tft.setTextSize(2);
  tft.setCursor(WIDTH_2-50, HEIGHT_2+10);
  tft.print("score: ");
  tft.print(fishCount);

  currentMillis = millis();
  for(int k = 0; k < 2; k++) {
    if(currentMillis - prevJoy[k] >= blinkInterval[k]) {
      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(2); //OMG!
      tft.setCursor(WIDTH_2-76, HEIGHT_2 + 34);
      if (k == 0) {
        tft.fillRect(WIDTH_2-76, HEIGHT_2 + 50 , 200, 30, TFT_BLACK);
      }
      if (k == 1) {
        tft.setCursor(WIDTH_2-76, HEIGHT_2 + 50);
        tft.println("press joystick");
      }
      
      prevJoy[k] = currentMillis;
    }
  }
  if (digitalRead(JOYSTICK_BUTTON_PIN) == 0) {
    gameGo = false;
    tft.fillScreen(TFT_BLACK);
    game_start();
  }
  
  
} 




/*---------------
----- MAIN ------
-------------- */

void setup(void) {
  // put your setup code here, to run once:
  tft.init();
  tft.setRotation(0); // set screen portrait
  tft.fillScreen(TFT_BLACK);

  // generate new "random" order each time
  randomSeed(analogRead(0));

  // setup switch
  pinMode(switchPin, INPUT_PULLUP);


  // setup joystick
  pinMode(JOYSTICK_BUTTON_PIN, INPUT);
  digitalWrite(JOYSTICK_BUTTON_PIN, HIGH);

  Serial.begin(9600);
}

void loop() {
  // setup a on/off switch


    
  //game_over();
  if (!gameGo && !gameOver) {
    game_start();
    previousFishCount = fishCount; // reset fishCount
  }
  if (!gameOver && gameGo) {
    /*-------------------------------------
    -------------- TIMER ------------------
    --------------------------------------*/
    unsigned long currentMillis = millis();
    if ((currentMillis - prevMillis) > 1000) {
      prevMillis = currentMillis;
      seconds += 1;

      if(seconds == 60) {
        seconds = 0;
        minutes += 1;
      }
  }



    /*-------------------------------------
    ---------- SCORE DISPLAY --------------
    --------------------------------------*/
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0,0);
    tft.print("score: ");
    tft.setCursor(80,0);
    tft.print(previousFishCount);
    if (previousFishCount < fishCount) {
      tft.setTextColor(TFT_BLACK);
      tft.setCursor(80,0);
      tft.print(previousFishCount);
      tft.print(fishCount);
      previousFishCount = fishCount;
    }




    /*--------------------------------------
    ----------- FALLING OBJECTS ------------
    --------------------------------------*/
    if(currentMillis - lastImageTime >= 1000 && numObjects < numImages) {
      lastImageTime = currentMillis;
      
      int x = random(7);
      int y = random(-100, 0);
      int speed = random(3, 5);
      
      bool overlap = false;
        
      // update fish objects
      fishObject = fishObject.update(x, y, speed);

      // update water objects
      waterObject1 = waterObject1.update(x, y, speed);
      waterObject2 = waterObject2.update(x, y, speed);
      waterObject3 = waterObject3.update(x, y, speed);
      waterObject4 = waterObject4.update(x, y, speed);
      waterObject5 = waterObject5.update(x, y, speed);
        


      numObjects++; // update number of objects

    }
      
    // display objects
    fishObject.showObject(isFish);
    waterObject1.showObject(isWater);
    waterObject2.showObject(isWater);
    waterObject3.showObject(isWater);
    waterObject4.showObject(isWater);
    waterObject5.showObject(isWater);
      


          
    /*--------------------
    ----- CAT MOVING ----- 
    ----------------------*/
    int currentJoyX = 512; // base joystick value
    int newJoyX = currentJoyX + map(analogRead(JOYSTICK_X_PIN), 0, 1023, -1000, 1000)/1000; // joystick value from 0 - 1023

    // determine if joystick is pressed left or right
    if (currentJoyX > newJoyX) { //left
      left = true;
      right = false;
    }
    else if (currentJoyX < newJoyX) { // right
      right = true;
      left = false;
    }
    else {
      right = false;
      left = false;
    }

    // limit cursor to bound of the screen
    cursorX = constrain(cursorX, 0, 276);
    cursorY = constrain(cursorY, 392, 392);

    // limit element value
    elementX = constrain(elementX, 0, 6);

    rectX = constrain(rectX, 0, 276);

    // if joystick is pressed right
    if (right) {
      cursorX = cursorX + 46; // update cursor to new cursor

      // find array element for x position
      for(int i = 0; i < xSize; i++) {
        if (catX[i] == cursorX) {
          found = true;
          elementX = i; // update x value of cat
          break;
        }
      }
    }

    // if joystick is pressed left
    if (left) {
      cursorX = cursorX - 46;

      for(int i = 0; i < xSize; i++) {
        if (catX[i] == cursorX) {
          found = true;
          elementX = i; // update x value of cat
          break;
        }
      }
    }

    // if joystick is moved
    if (found) { // determine direction
      if (left) {
        // "delete" previous image of cat
        rectX = catX[elementX + 1];
        tft.fillRect(rectX, cursorY, 46, 88, TFT_BLACK);
        tft.pushImage(cursorX, cursorY, 46, 88, cat);
      }

      if (right) {
        // "delete" previous image of cat
        rectX = catX[elementX - 1];
        tft.fillRect(rectX, cursorY, 46, 88, TFT_BLACK);
        tft.pushImage(cursorX, cursorY, 46, 88, cat);
      }
    }

      


    /*---------------------
    --- COLLISION CHECK----
    -------------------- */
    // check if collision with fish

    /*if (checkFish) {
      fishCount++;
      fishObject.deleteObject(checkFish);
      fishObject.resetObject();      
    }*/

    int checkFishX = fishObject.getX();    
    int checkFishY = fishObject.getY()+fishHeight;
    bool checkFishCollision = false;
        
      /*if (checkFishY >= limitY && checkFishX == elementX) {
        tft.fillRect(checkFishX, checkFishY-fishHeight, fishWidth, fishHeight, TFT_BLACK);
        tft.pushImage(cursorX, cursorY, 46, 88, cat);
        fishObject.resetObject();      
        fishCount++;
      }*/

    // do check if fishY reaches the limitY value and if fishX is same x as fish
    if (checkFishY >= limitY) {
      if (checkFishX == elementX) {
        tft.fillRect(xGridValue[elementX], limitY-fishHeight, fishWidth, fishHeight, TFT_BLACK);
        tft.pushImage(cursorX, cursorY, 46, 88, cat);
        fishObject.resetObject();      
        fishCount++;
      }
    }    

    // get all waterObject x, y values
    int checkWater1X = waterObject1.getX();
    int checkWater1Y = waterObject1.getY()+waterHeight;
    bool checkWater1Collision = false;


    int checkWater2X = waterObject2.getX();
    int checkWater2Y = waterObject2.getY()+waterHeight;
    bool checkWater2Collision = false;


    int checkWater3X = waterObject3.getX();
    int checkWater3Y = waterObject3.getY()+waterHeight;
    bool checkWater3Collision = false;


    int checkWater4X = waterObject4.getX();
    int checkWater4Y = waterObject4.getY()+waterHeight;
    bool checkWater4Collision = false;


    int checkWater5X = waterObject5.getX();
    int checkWater5Y = waterObject5.getY()+waterHeight;
    bool checkWater5Collision = false;


    // do actual check for waterObjects
    if(checkWater1Y>limitY) {
      if(checkWater1X==elementX) {
        tft.fillScreen(TFT_BLACK); // make screen black
        gameOver = true; // change bool to transition screen to game_over
        gameGo = false;

        fishObject.resetObject();
        waterObject1.resetObject();
        waterObject2.resetObject();
        waterObject3.resetObject();
        waterObject4.resetObject();
        waterObject5.resetObject();
      }
    }

    if(checkWater2Y>limitY) {
      if(checkWater2X==elementX) {
        tft.fillScreen(TFT_BLACK); // make screen black
        gameOver = true; // change bool to transition screen to game_over
        gameGo = false;

        fishObject.resetObject();
        waterObject1.resetObject();
        waterObject2.resetObject();
        waterObject3.resetObject();
        waterObject4.resetObject();
        waterObject5.resetObject();
      }
    }

    if(checkWater3Y>limitY) {
      if(checkWater3X==elementX) {
        tft.fillScreen(TFT_BLACK); // make screen black
        gameOver = true; // change bool to transition screen to game_over
        gameGo = false;

        fishObject.resetObject();
        waterObject1.resetObject();
        waterObject2.resetObject();
        waterObject3.resetObject();
        waterObject4.resetObject();
        waterObject5.resetObject();
      }
    }

    if(checkWater4Y>limitY) {
      if(checkWater4X==elementX) {
        tft.fillScreen(TFT_BLACK); // make screen black
        gameOver = true; // change bool to transition screen to game_over
        gameGo = false;

        fishObject.resetObject();
        waterObject1.resetObject();
        waterObject2.resetObject();
        waterObject3.resetObject();
        waterObject4.resetObject();
        waterObject5.resetObject();
      }
    }

    if(checkWater5Y>limitY) {
      if(checkWater5X==elementX) {
        tft.fillScreen(TFT_BLACK); // make screen black
        gameOver = true; // change bool to transition screen to game_over
        gameGo = false;

        fishObject.resetObject();
        waterObject1.resetObject();
        waterObject2.resetObject();
        waterObject3.resetObject();
        waterObject4.resetObject();
        waterObject5.resetObject();
      }
    }

      // check if collision with water
      /*int checkWaterX[numImages];
      int checkWaterY[numImages];

      for (int w = 0; w < numImages; w++) {
        checkWaterX[w] = waterObject[w].getX();
        checkWaterY[w] = waterObject[w].getY() + waterHeight;

        if (checkWaterY[w] > limitY) {
          if (checkWaterX[w] == elementX) {
            tft.fillScreen(TFT_BLACK);
            gameOver = true;
            gameGo = false;
          }      
        }
      }*/

    Serial.print("Cat X value: ");
    Serial.println(elementX);
    Serial.print("Fish X value: ");
    Serial.println(checkFishX);

  }

  if (gameOver && !gameGo) {
    game_over();
  }


}

