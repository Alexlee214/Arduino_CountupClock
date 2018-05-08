// include the library code:
#include <LiquidCrystal.h>
#include <TimeLib.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const byte rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4, a = 8;
const byte backlightInterrupt = 2;
const byte modeInterrupt = 3;

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
byte curDay = 13;
byte curMonths= 5;
int curYear = 2018;


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


void setup() {
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  pinMode(a, OUTPUT);
  pinMode(modeInterrupt, INPUT_PULLUP);
  pinMode(backlightInterrupt, INPUT_PULLUP);
  digitalWrite(a, HIGH);
  attachInterrupt(0, setBacklight, FALLING);
  attachInterrupt(1, setMode, FALLING);
  lcd.createChar(0, heart);
  lcd.begin(16, 2);
  startupScreen();
}

void loop() {
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


void setTime(){
  
}


void clockDisplay(){
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 0);
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
byte numYearsSince(byte day, byte month, int year){
  return curYear - year;
}


int numMonthsSince(byte day, byte month, int year){
  return (numYearsSince(day, month, year) * 12) + (12 - month) + curMonths;
}


int numDaysSince(byte day, byte month, int year){
  int days = 0;

  //handle the year in between the two years
  for(byte yearCount = 1; yearCount <= numYearsSince(day, month, year) - 1; yearCount++){
    if (isLeapYear(year + yearCount)) days = days + 366;
    else days = days + 365; 
  }


  //handles the months before in year
  for(byte countMonth = month + 1; countMonth <= 12; countMonth++){
    
  }

  //handles the months in curYear
  for(byte countCurMonth = month - 1; countCurMonth >= 1; countCurMonth--){
    
  }
  
/*
  byte monthsCount;
  //different year
  if(curYear - year > 0){
      if(curMonth - month > 0)
        monthsCount = (11 + curMonth - month);
      else if(curMonth - month == 0)
        monthsCount = 11;
      else monthsCount = (11 - (month - curMonth))
  }
  //same year
  else if (curYear - year = 0){
    if(curMonth - month > 0)
      monthsCount = curMonth = month;
    else if(curMonths)
  }                                                
                
  
  //handle the month
  for(byte monthCount = 1; monthCount < months - curMonth; monthCount++){
    switch(months % 12):
      case 1, 3, 5, 7, 8, 10, 12: days = days + 31;
      case 4, 6, 9, 11: days = days + 30;
      case: 2: if()
  }
  */

  return 0;
}


int numHoursSince(byte day, byte month, int year){
  return (numDaysSince(day, month, year) * 24);
}


byte daysInMonth(byte month, int year){
  switch(month % 12){
      case 4:
      case 6:
      case 9:
      case 11: return 30;
      case 2: if(isLeapYear(year)) return 29;
              else return 28;
      default: return 31;        
  }   
}

//referenced from https://www.programiz.com/c-programming/examples/leap-year
bool isLeapYear(int year){
  if(year % 4 == 0)
    {
        if( year % 100 == 0)
        {
            // year is divisible by 400, hence the year is a leap year
            if ( year % 400 == 0)
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
