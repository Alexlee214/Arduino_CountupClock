//everytime it starts up, the value that was last recorded upon set up is stored
//next, keep track of the time in the timer and update hour, day, date accordingly in loop
//set up timer so that the value is stored in EEPROM every once in a while

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
//mode 2: Reminders of the Day:
//mode 3: been a mom for ___years
//mode 4: been a mom for ___months
//mode 5: been a mom for ___days
//mode 6: been a mom for ___hours
volatile byte mode = 0;


byte childrenBDay = 14;
byte childrenBMonth = 2;
int childrenBYear = 1998;

int momMonths = 243;
int momDays = 0;
int momHours = 0;
byte momYears = 0;

//modify later to be set in setup
byte curDays;
byte curMonths;
int curYears;
byte curHour, curMinute, curDayWeek;


//function declarations
void setTime();
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
  attachInterrupt(0, setBacklight, FALLING);
  attachInterrupt(1, setMode, FALLING);
  lcd.createChar(0, heart);
  lcd.begin(16, 2);
  timeSet();
  storeDates();
  startupScreen();
}

void loop() {
}



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

void startupScreen(){
  Serial.println("called start");
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("Happy MothersDay");
  lcd.setCursor(0, 1);
  lcd.print(char(0));
  lcd.setCursor(1, 1);
  lcd.print(char(0));
  lcd.setCursor(2, 1);
  lcd.print(char(0));
  lcd.print("Love Alex");
  lcd.setCursor(12, 1);
  lcd.print(char(0));
  lcd.setCursor(13, 1);
  lcd.print(char(0));
  lcd.setCursor(14, 1);
  lcd.print(char(0));
  lcd.setCursor(15, 1);
  lcd.print(char(0));
}


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
  lcd.print("Year/Month/Day ");
  lcd.setCursor(1, 1);
  lcd.print(myYears);
  lcd.setCursor(5, 1);
  lcd.print("/");
  lcd.setCursor(9, 1);
  lcd.print(myMonths);
  lcd.setCursor(12, 1);
  lcd.print("/");
  lcd.setCursor(14, 1);
  lcd.print(myDays);
  lcd.setCursor(cursorPosition, 1);
  dateSet(readySet, cursorPosition, myYears, myMonths, myDays);
  cursorPosition = 1;

  //setting up for setting the time
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Hour/Minute/Day");
  lcd.setCursor(1, 1);
  lcd.print(EEPROM.read(5));
  lcd.setCursor(4, 1);
  lcd.print(":");
  lcd.setCursor(7, 1);
  lcd.print(EEPROM.read(6));
  lcd.setCursor(11, 1);
  lcd.print("|");
  lcd.setCursor(13, 1);
  lcd.print(EEPROM.read(7));
  lcd.setCursor(cursorPosition, 1);
  delay(200);
  hourSet(readySet, cursorPosition);
}

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
        case 9: if(myMonths < 12) myMonths++;
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
        case 9: if(myMonths > 1){
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

//set the hours, minute and day of week at startup
void hourSet(bool &readySet, byte &cursorPosition){
  Serial.println(cursorPosition);
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
}


void twoDigitToOne(byte cursorPosition){
  lcd.setCursor(cursorPosition+1, 1);
  lcd.print(" ");
  lcd.setCursor(cursorPosition, 1);
}


bool nextDateCursor(byte &cursorPosition){
  if(cursorPosition == 1){
    lcd.setCursor(9, 1);
    cursorPosition = 9;
  }
  else if(cursorPosition == 9){
    lcd.setCursor(14, 1);
    cursorPosition = 14;
  }
  else{
    return true;
  }
  delay(300);
  return false;
}

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

void calibrateJoy(){
  short sumX = 0;
  short sumY = 0;
  
  for(int count = 0; count < 20; count++){
    sumX += analogRead(joyX);
    sumY += analogRead(joyY);
  }

  XCenter = (short)(sumX / 20);
  YCenter = (short)(sumY / 20);
}


void clockDisplay(){
  lcd.clear();
  delay(100);
  lcd.setCursor(1, 0);
}


void setBacklight(){
  Serial.println("backlight");
  if(digitalRead(a) == HIGH) 
    digitalWrite(a, LOW);
  else
    digitalWrite(a, HIGH);  
}


void setMode(){
  if (mode < 5) mode++;
  else mode = 0; 

  Serial.println(mode);
  switch (mode){
    case 0: startupScreen();
            break;
    case 1: clockDisplay();
            break;
    case 2: momForYears();
            break;
    case 3: momForMonths();
            break;
    case 4: momForDays();
            break;
    case 5: momForHours();  
            break;
    default: startupScreen();
             break;
  }
  
}


void momForYears(){
  Serial.println("called years");
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("Been a mom for: ");
  lcd.setCursor(0,1);
  lcd.print(momYears);
  lcd.setCursor(11, 1);
  lcd.print("Years");
}


void momForMonths(){
  Serial.println("called months");
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("Been a mom for: ");
  lcd.setCursor(0,1);
  lcd.print(momMonths);
  lcd.setCursor(10, 1);
  lcd.print("Months");
}


void momForDays(){
  Serial.println("called days");
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("Been a mom for: ");
  lcd.setCursor(0,1);
  lcd.print(momDays);
  lcd.setCursor(12, 1);
  lcd.print("days");
}


void momForHours(){
  Serial.println("called hours");
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("Been a mom for: ");
  lcd.setCursor(0,1);
  lcd.print(momDays);
  lcd.setCursor(11, 1);
  lcd.print("hours");
}


//for count year, subtract 1 from this value
byte numYearsSince(byte days, byte months, int years){
  return curYears - years;
}


int numMonthsSince(byte days, byte months, int years){
  return (numYearsSince(days, months, years) * 12) + (12 - months) + curMonths;
}


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


int numHoursSince(byte days, byte months, int years){
  return (numDaysSince(days, months, years) * 24);
}


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

