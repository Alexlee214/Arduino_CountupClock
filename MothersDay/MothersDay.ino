//everytime it starts up, the value that was last recorded upon set up is stored
//next, keep track of the time in the timer and update hour, day, date accordingly in loop
//set up timer so that the value is stored in EEPROM every once in a while
//backlight auto off after 10 seconds of inactivity
//timer mode?
//countdown to newYears?
//events? (new years)
//Bday?
//closest event?

// include the library code:
#include <LiquidCrystal.h>
#include <EEPROM.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to

const byte joyY = A0;
const byte joyX = A1;
const byte joyClick = A2;

const byte rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4, a = 8;
const byte backlightInterrupt = 2;
const byte modeInterrupt = 3;

short XCenter, YCenter;
volatile int lastModeChange = 0;
volatile int lastLightChange = 0;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const char heart[8] = {
  B00000,
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};


//use button to turn on and off backlight, also auto off after 30 seconds
//use interrupt Service Routine to set the mode
//use button and ISR to set mode, mode automatically returns to 0 after 30 minutes
//mode 0: From: Alex Lee, 2018/5/13
//mode 1: clock
//mode 2: been a mom for ___years
//mode 3: been a mom for ___months
//mode 4: been a mom for ___days
//mode 5: been a mom for ___hours
const byte totalModeNum = 6;
volatile byte mode;
const byte startMode = 0;
const byte clockMode = 1;
const byte momYearsMode = 2;
const byte momMonthsMode = 3;
const byte momDaysMode = 4;
const byte momHoursMode = 5;

byte childrenBDay = 14;
byte childrenBMonth = 2;
int childrenBYear = 1998;

int momMonths = 0;
int momDays = 0;
int momHours = 0;
byte momYears = 0;

//modify later to be set in setup
byte curDays;
byte curMonths;
int curYears;
byte curHour, curMinute, curDayWeek, curSecond;

unsigned long lastMillis = 0;


//---------------------------------------------------------------------------------------------------------------------------------------------function declaration
//function declarations
void timeSet();
void clockDisplay();
void setBacklight();
void setMode();
void momForYears();
void momForMonths();
void momForDays();
void momForHours();
byte numYearsSince(byte day, byte month, int year);
int numMonthsSince(byte day, byte month, int year);
int numDaysSince(byte day, byte month, int year);
int numHoursSince(byte day, byte month, int year);
bool isLeapYear(int year);
void calibrateJoy();
bool nextDateCursor(byte &cursorPosition);
bool nextHourCursor(byte &cursorPosition);
void twoDigitToOne(byte cursorPosition);
byte daysInMonth(byte months, int years);
void printModeNum(byte modeNum);
//---------------------------------------------------------------------------------------------------------------------------------------------function declaration


//---------------------------------------------------------------------------------------------------------------------------------------------setup
void setup() {
  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(joyClick, INPUT_PULLUP);
  pinMode(a, OUTPUT);
  pinMode(modeInterrupt, INPUT_PULLUP);
  pinMode(backlightInterrupt, INPUT_PULLUP);
  calibrateJoy();
  
  //initializeDates();
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  digitalWrite(a, HIGH);
  lcd.createChar(0, heart);
  attachInterrupt(0, setBacklight, FALLING);
  lcd.begin(16, 2);
  timeSet();
  //set the starting time for keeping track of time
  lastMillis = millis();
  storeDates();
  attachInterrupt(1, setMode, FALLING);
  //start from the startup mode
  mode = 0;
  startupScreen();
}
//---------------------------------------------------------------------------------------------------------------------------------------------setup

//---------------------------------------------------------------------------------------------------------------------------------------------loop
void loop() {
  unsigned long curMillis = millis();

  //after 1 minute, update the time
  //also handles when millis resets, happens approximately every 49.7 days
  if(abs(curMillis - lastMillis) >= 60000){
    //reset the time before all calculations for more accurate time keeping
    lastMillis = millis();

    //increment minute, and update hour if exceeded limit
    curMinute++;
    if(curMinute > 59){
      curMinute = 0;
      curHour++;
    }
    //update the day of week and date if a new day
    if(curHour > 23){
      curHour = 0;
      curDays++;
      curDayWeek++;
    }
    //reset to monday if exceeds sunday
    if(curDayWeek > 7){
      curDayWeek = 1;
    }
    //if month is over, go to the next month
    if(curDays > daysInMonth(curMonths, curYears)){
      curDays = 1;
      curMonths++;
    }
    //go to next year if month exceeds limit
    if(curMonths > 12){
      curMonths = 1;
      curYears++;
    }

    //if screen is displaying clock, update in real-time
    if(mode == clockMode)
      clockDisplay();
      
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------loop


//---------------------------------------------------------------------------------------------------------------------------------------------InitializeEEPROM
//0 marks the beginning of a new date entry
//year, month, day
void initializeDates(){
  //year, month, day
  EEPROM.write(0, 0);
  EEPROM.write(1, 18);
  EEPROM.write(2, 5);
  EEPROM.write(3, 13);

  //hour, minute, day of week
  EEPROM.write(4, 0);
  EEPROM.write(5, 0);
  EEPROM.write(6, 0);
  EEPROM.write(7, 0);

  EEPROM.write(8, 0);
  EEPROM.write(9, 98);
  EEPROM.write(10, 2);
  EEPROM.write(11, 14);
}
//---------------------------------------------------------------------------------------------------------------------------------------------InitializeEEPROM


//---------------------------------------------------------------------------------------------------------------------------------------------storeDates
//stores the value of the date in EEPROMright after it has been set
void storeDates(){
  EEPROM.write(0, 0);
  EEPROM.write(1, curYears - 2000);
  EEPROM.write(2, curMonths);
  EEPROM.write(3, curDays);
  EEPROM.write(4, 0);
  EEPROM.write(5, curHour);
  EEPROM.write(6, curMinute);
  EEPROM.write(7, curDayWeek);
}
//---------------------------------------------------------------------------------------------------------------------------------------------storeDates


//---------------------------------------------------------------------------------------------------------------------------------------------startupScreen
//displays Mother's day message, called by an ISR triggered by interrupt 1 (int 3)
void startupScreen(){
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print(F("Happy MothersDay"));
  lcd.setCursor(0, 1);
  lcd.print(char(0));
  lcd.print(char(0));
  lcd.print(F("Love Alex"));
  lcd.print(char(0));
  lcd.print(char(0));

  printModeNum(mode);
}
//---------------------------------------------------------------------------------------------------------------------------------------------startupScreen


//---------------------------------------------------------------------------------------------------------------------------------------------timeSet
//set up of the time at startup (not necessary if have an RTC)
// cursor position for year: 1,1
// month: 12, 1
// day: 14, 1
// Hour: 1, 1
// Minute: 7, 1
// Day of Week: 13, 1
void timeSet(){
  byte myDays = EEPROM.read(3);
  byte myMonths = EEPROM.read(2);
  int myYears = 2000 + EEPROM.read(1);
  byte cursorPosition = 1;
  bool readySet = false;

  //setting up for setting the date
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(F("Year/Month/Day "));
  lcd.setCursor(1, 1);
  lcd.print(myYears);
  lcd.setCursor(5, 1);
  lcd.print(F("/"));
  lcd.setCursor(8, 1);
  lcd.print(myMonths);
  lcd.setCursor(12, 1);
  lcd.print(F("/"));
  lcd.setCursor(14, 1);
  lcd.print(myDays);
  lcd.setCursor(cursorPosition, 1);
  dateSet(readySet, cursorPosition, myYears, myMonths, myDays);
  cursorPosition = 1;

  //setting up for setting the time
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(F("Hour:Minute|Day"));
  lcd.setCursor(1, 1);
  lcd.print(EEPROM.read(5));
  lcd.setCursor(4, 1);
  lcd.print(F(":"));
  lcd.setCursor(7, 1);
  lcd.print(EEPROM.read(6));
  lcd.setCursor(11, 1);
  lcd.print(F("|"));
  lcd.setCursor(13, 1);
  lcd.print(EEPROM.read(7));
  lcd.setCursor(cursorPosition, 1);
  delay(200);
  hourSet(readySet, cursorPosition);

  //don't display the cursor anymore
  lcd.noBlink();
}
//---------------------------------------------------------------------------------------------------------------------------------------------timeSet


//---------------------------------------------------------------------------------------------------------------------------------------------dateSet
//helper function for timeSet
//set the year, month, day at startup
void dateSet(bool &readySet, byte &cursorPosition, int &myYears, byte &myMonths, byte &myDays){
  while(!readySet){
    lcd.blink();
    if(digitalRead(joyClick) == 0){
      readySet = nextDateCursor(cursorPosition);
      
    //joyStick goes up
    } else if(analogRead(joyY) <= 50){
      //change values according to the currunt position of the cursor
      switch(cursorPosition){
        case 1: myYears++;
                lcd.print(myYears);
                break;
                //provide boundary for month
        case 8: if(myMonths < 12) myMonths++;
                else{ 
                  myMonths = 1;
                  //clears a pixel on the lcd so that the 1 digit value shows up corectly
                  twoDigitToOne(cursorPosition);
                }
                lcd.print(myMonths);
                break;
                //provide boundary for day
        case 14:if(myDays < daysInMonth(myMonths, myYears)) myDays++;
                else{
                  myDays = 1;
                  //clears a pixel on the lcd so that the 1 digit value shows up corectly
                  twoDigitToOne(cursorPosition);
                }
                lcd.print(myDays);
                break;
      }
      lcd.setCursor(cursorPosition, 1);
      delay(200);
      
    //joyStick goes down  
    }else if(analogRead(joyY) >=  1000){
      switch(cursorPosition){
        case 1: myYears--;
                lcd.print(myYears);
                break;
        case 8: if(myMonths > 1){
                  myMonths--;
                  //clears a pixel on the lcd so that the 1 digit value shows up corectly
                  if(myMonths <= 9){
                    twoDigitToOne(cursorPosition);
                  }
                }
                else myMonths = 12;
                lcd.print(myMonths);
                break;
        case 14:if(myDays > 1){
                  myDays--;
                  //clears a pixel on the lcd so that the 1 digit value shows up corectly
                  if(myDays <= 9){
                    twoDigitToOne(cursorPosition);
                  }
                }  
                else myDays = daysInMonth(myMonths, myYears);
                lcd.print(myDays);
                break;
                
      }
        lcd.setCursor(cursorPosition, 1);
        delay(200);
     }   
  }
  curDays = myDays;
  curMonths = myMonths;
  curYears = myYears;
}
//---------------------------------------------------------------------------------------------------------------------------------------------dateSet


//---------------------------------------------------------------------------------------------------------------------------------------------hourSet
//helper function for timeSet
//set the hours, minute and day of week at startup
void hourSet(bool &readySet, byte &cursorPosition){
  lcd.setCursor(1, 1);
  byte myHours = EEPROM.read(5);
  byte myMinutes = EEPROM.read(6);
  byte myDaysWeek = EEPROM.read(7);
  
  while(readySet){
    if(digitalRead(joyClick) == 0){
      readySet = nextHourCursor(cursorPosition);
      
    //joyStick goes up
    } else if(analogRead(joyY) <= 50){
      switch(cursorPosition){
        case 1: if(myHours < 23) myHours++;
                else{ 
                  myHours = 0;
                  twoDigitToOne(cursorPosition);
                }
                lcd.print(myHours);
                break;
        case 7: if(myMinutes < 59) myMinutes++;
                else{ 
                  myMinutes = 0;
                  twoDigitToOne(cursorPosition);
                }
                lcd.print(myMinutes);
                break;
        case 13:if(myDaysWeek < 7) myDaysWeek++;
                else{
                  myDaysWeek = 1;
                  twoDigitToOne(cursorPosition);
                }
                lcd.print(myDaysWeek);
                break;
      }
      lcd.setCursor(cursorPosition, 1);
      delay(200);
      
    //joyStick goes down  
    }else if(analogRead(joyY) >=  1000){
      switch(cursorPosition){
        case 1: if(myHours > 0){
                  myHours--;
                  if(myHours <= 9) 
                    twoDigitToOne(cursorPosition);
                }else myHours = 23;
                lcd.print(myHours);
                break;
        case 7: if(myMinutes > 0){
                  myMinutes--;
                  if(myMinutes <= 9)
                    twoDigitToOne(cursorPosition);
                }
                else myMinutes = 59;
                lcd.print(myMinutes);
                break;
                
        case 13:if(myDaysWeek > 1) myDaysWeek--;
                else myDaysWeek = 7;
                lcd.print(myDaysWeek);
                break;
                
      }
        lcd.setCursor(cursorPosition, 1);
        delay(200);
     }   
  }

  curHour = myHours;
  curMinute = myMinutes;
  curDayWeek = myDaysWeek;
  curSecond = 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------hourSet


//---------------------------------------------------------------------------------------------------------------------------------------------twoDigitToOne
//clears a pixel on the lcd to be able to go from displaying a 2-digit numer to a 1-digit number
void twoDigitToOne(byte cursorPosition){
  lcd.setCursor(cursorPosition+1, 1);
  lcd.print(F(" "));
  lcd.setCursor(cursorPosition, 1);
}
//---------------------------------------------------------------------------------------------------------------------------------------------twoDigitToOne


//---------------------------------------------------------------------------------------------------------------------------------------------nextDateCursor
//helper function for dateSet
//sets the value of the next cursor position and returns if dateSet should terminate
bool nextDateCursor(byte &cursorPosition){
  if(cursorPosition == 1){
    lcd.setCursor(8, 1);
    cursorPosition = 8;
  }
  else if(cursorPosition == 8){
    lcd.setCursor(14, 1);
    cursorPosition = 14;
  }
  else{
    return true;
  }
  delay(300);
  return false;
}
//---------------------------------------------------------------------------------------------------------------------------------------------nextDateCursor


//---------------------------------------------------------------------------------------------------------------------------------------------nextHourCursor
//helper function for hourSet
//sets the value of the next cursor position and returns if hourSet should terminate
bool nextHourCursor(byte &cursorPosition){
  if(cursorPosition == 1){
    lcd.setCursor(7, 1);
    cursorPosition = 7;
  }
  else if(cursorPosition == 7){
    lcd.setCursor(13, 1);
    cursorPosition = 13;
  }
  else{
    lcd.noCursor();
    return false;
  }
  delay(300);
  return true;
}
//---------------------------------------------------------------------------------------------------------------------------------------------nextHourCursor


//---------------------------------------------------------------------------------------------------------------------------------------------calibrateJoy
//calibrates the center position ofthe joysticks
//takes 0.2 seconds upon startup 
void calibrateJoy(){
  short sumX = 0;
  short sumY = 0;
  
  for(int count = 0; count < 20; count++){
    sumX += analogRead(joyX);
    sumY += analogRead(joyY);
    delay(10);
  }

  XCenter = (short)(sumX / 20);
  YCenter = (short)(sumY / 20);
}

//---------------------------------------------------------------------------------------------------------------------------------------------displayClock
//displays the date, time 
void clockDisplay(){
  lcd.clear();
  lcd.setCursor(0, 0);
  printDate(curMonths, curDays, curYears, curHour, curMinute, curDayWeek);
  printModeNum(mode);
}
//---------------------------------------------------------------------------------------------------------------------------------------------displayClock


//prints the current month on the lcd and sets where the next cursor should go
void printDate(byte myMonths, byte myDays, int myYears, byte myHours, byte myMinutes, byte myDaysWeek){
  switch(myMonths){
    case 1: lcd.print(F("JAN"));
            break;
    case 2:lcd.print(F("FEB"));
            break;
    case 3:lcd.print(F("MAR"));
            break;
    case 4:lcd.print(F("APR"));
            break;
    case 5:lcd.print(F("MAY"));
            break;
    case 6:lcd.print(F("JUN"));
            break;
    case 7:lcd.print(F("JUL"));
            break;
    case 8:lcd.print(F("AUG"));
            break;
    case 9:lcd.print(F("SEPT"));
            break;
    case 10:lcd.print(F("OCT"));
            break;
    case 11:lcd.print(F("NOV"));
            break;
    case 12:lcd.print(F("DEC"));
            break;
  }
  lcd.print(F(", "));
  lcd.print(myDays);
  lcd.print(F(", "));
  lcd.print(myYears);

  //convert to PM, AM format
  lcd.setCursor(0, 1);
  if(myHours >= 13) lcd.print(myHours % 12);
  else lcd.print(myHours);
  lcd.print(":");
  lcd.print(myMinutes);
  if(myHours >= 12) lcd.print(F("PM"));
  else lcd.print(F("AM"));

  lcd.print(F(" "));
  switch(myDaysWeek){
    case 1: lcd.print(F("MON"));
            break;
    case 2: lcd.print(F("TUES"));
            break;
    case 3: lcd.print(F("WED"));
            break;
    case 4: lcd.print(F("THURS"));
            break;
    case 5: lcd.print(F("FRI"));
            break;
    case 6: lcd.print(F("SAT"));
            break;
    case 7: lcd.print(F("SUN"));
            break;
  }

    
}


//---------------------------------------------------------------------------------------------------------------------------------------------setBacklight
//triggered by interrupt 0(pin 2)
//turn on and off the backlight of the lcd
void setBacklight(){
  if(millis() - lastLightChange >= 500 ||  -(millis() - lastLightChange) >= 500){
    lastLightChange = millis();
    if(digitalRead(a) == HIGH) 
      digitalWrite(a, LOW);
    else
      digitalWrite(a, HIGH); 
  }    
}
//---------------------------------------------------------------------------------------------------------------------------------------------setBacklight


//---------------------------------------------------------------------------------------------------------------------------------------------seMode
//triggered by interrupt 1 (pin 3)
//iterates throught the modes available
void setMode(){
  //debounce button interrupt
  if(millis() - lastModeChange >= 500 ||  -(millis() - lastModeChange) >= 500){
    lastModeChange = millis();
    //modify here to add mode modes to be called
    if (mode < 5) mode++;
    else mode = 0; 

    //if more modes are added, add another case statement and specify what should happen
    switch (mode){
      case startMode: startupScreen();
              break;
      case clockMode: clockDisplay();
              break;
      case momYearsMode: momForYears();
              break;
      case momMonthsMode: momForMonths();
              break;
      case momDaysMode: momForDays();
              break;
      case momHoursMode: momForHours();  
              break;
      default: startupScreen();
               break;
    }
  }  
}
//---------------------------------------------------------------------------------------------------------------------------------------------seMode


//---------------------------------------------------------------------------------------------------------------------------------------------momForYears
//displays how many years mom has been a mom
void momForYears(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Been a mom for: "));
  lcd.setCursor(0,1);
  lcd.print(momYears);
  lcd.print(F(" "));
  lcd.print(F("Years"));
  printModeNum(mode);
}
//---------------------------------------------------------------------------------------------------------------------------------------------momForYears


//---------------------------------------------------------------------------------------------------------------------------------------------momForMonths
//displays how many months mom has been a mom
void momForMonths(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Been a mom for: "));
  lcd.setCursor(0,1);
  lcd.print(momMonths);
  lcd.print(F(" "));
  lcd.print(F("Months"));
  printModeNum(mode);
}
//---------------------------------------------------------------------------------------------------------------------------------------------momForMonths


//---------------------------------------------------------------------------------------------------------------------------------------------momForDays
//displays how many days mom has been a mom
void momForDays(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Been a mom for: "));
  lcd.setCursor(0,1);
  lcd.print(momDays);
  lcd.print(F(" "));
  lcd.print(F("days"));
  printModeNum(mode);
}
//---------------------------------------------------------------------------------------------------------------------------------------------momForDays


//---------------------------------------------------------------------------------------------------------------------------------------------momForHours
//displays how many hours mom has been a mom
void momForHours(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Been a mom for: "));
  lcd.setCursor(0,1);
  lcd.print(momDays);
  lcd.print(F(" "));
  lcd.print(F("hours"));
  printModeNum(mode);
}
//---------------------------------------------------------------------------------------------------------------------------------------------momForYears



//---------------------------------------------------------------------------------------------------------------------------------------------numYearsSince
//calculates how many years has past been the current year and a given year
//for count year, subtract 1 from this value
byte numYearsSince(byte days, byte months, int years){
  return curYears - years;
}
//---------------------------------------------------------------------------------------------------------------------------------------------numYearsSince


//---------------------------------------------------------------------------------------------------------------------------------------------numMonthsSince
//calculates how many years has past been the current year and a given year
int numMonthsSince(byte days, byte months, int years){
  return (numYearsSince(days, months, years) * 12) + (12 - months) + curMonths;
}
//---------------------------------------------------------------------------------------------------------------------------------------------numMonthsSince


//---------------------------------------------------------------------------------------------------------------------------------------------numDaysSince
//calculates and returns the number of days since the a certain date
int numDaysSince(byte days, byte months, int years){
  int totalDays = 0;

  //handle the year in between the two years
  for(byte yearCount = 1; yearCount <= numYearsSince(days, months, years) - 1; yearCount++){
    if (isLeapYear(years + yearCount)) totalDays = totalDays + 366;
    else totalDays = totalDays + 365; 
  }

  //handles the months before in year
  for(byte countMonth = months + 1; countMonth <= 12; countMonth++)
    totalDays = totalDays + daysInMonth(countMonth, years);

  //handles the months in curYearss
  for(byte countCurMonth = months - 1; countCurMonth >= 1; countCurMonth--)
    totalDays = totalDays + daysInMonth(countCurMonth, years);

  //handles the days in years
  totalDays = totalDays + (daysInMonth(months, years) - days);

  //handles the days in curYearss
  totalDays = totalDays + curDays;
    
  return totalDays;
}
//---------------------------------------------------------------------------------------------------------------------------------------------numDaysSince



//---------------------------------------------------------------------------------------------------------------------------------------------numHoursSince
//calculates and returns the number of hours since the a certain date and time
int numHoursSince(byte days, byte months, int years){
  return (numDaysSince(days, months, years) * 24);
}
//---------------------------------------------------------------------------------------------------------------------------------------------numHoursSince


//---------------------------------------------------------------------------------------------------------------------------------------------daysInMonth
//Returns the number of days in a given month in a given year
byte daysInMonth(byte months, int years){
  switch(months % 12){
      case 4:
      case 6:
      case 9:
      case 11: return 30;
      case 2: if(isLeapYear(years)) return 29;
              else return 28;
      default: return 31;        
  }   
}
//---------------------------------------------------------------------------------------------------------------------------------------------daysInMonth


//prints the mode number on the bottom right corner
void printModeNum(byte modeNum){
  lcd.setCursor(13, 1);
  //mode num starts from 0 as oppose to 1
  lcd.print(modeNum + 1);
  lcd.print(F("/"));
  lcd.print(totalModeNum);
}

//---------------------------------------------------------------------------------------------------------------------------------------------isLeapYear
//Returns whether or not a certain year is leap year
//referenced from https://www.programiz.com/c-programming/examples/leap-year
bool isLeapYear(int years){
  if(years % 4 == 0)
    {
        if( years % 100 == 0)
        {
            // year is divisible by 400, hence the year is a leap year
            if ( years % 400 == 0)
                return true;
            else
                return false;
        }
        else
            return true;
    }
    else
       return false;
}
//---------------------------------------------------------------------------------------------------------------------------------------------isLeapYear
