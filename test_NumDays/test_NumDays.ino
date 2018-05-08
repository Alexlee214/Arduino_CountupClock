byte curMonths = 5;
byte curDay = 8;
int curYear = 2018;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int value = (numDaysSince(6, 5, 2018));
  /*
  Serial.print("Should return 2, Returned days is:");
  Serial.println(value);
  Serial.println("");

  value = (numDaysSince(10, 3, 2018));
  Serial.print("Should return 59, Returned days is:");
  Serial.println(value);
  Serial.println("");

  value = (numDaysSince(1, 3, 2018));
  Serial.print("Should return 68, Returned days is:");
  Serial.println(value);
  Serial.println("");

  value = (numDaysSince(8, 3, 2018));
  Serial.print("Should return 61, Returned days is:");
  Serial.println(value);
  Serial.println("");
*/
  Serial.print("Should return 365, Returned days is:");
  Serial.println(numDaysSince(8, 5, 2017));
  Serial.println("");

    Serial.print("Should return 395, Returned days is:");
  Serial.println(numDaysSince(8, 4, 2017));
  Serial.println("");


  Serial.print("Should return 731, Returned days is:");
  Serial.println(numDaysSince(8, 5, 2016));
  Serial.println("");

    Serial.print("Should return 759, Returned days is:");
  Serial.println(numDaysSince(10, 4, 2016));
  Serial.println("");

  Serial.print("Should return 790, Returned days is:");
  Serial.println(numDaysSince(10, 3, 2016));
  Serial.println("");

  
  Serial.print("Should return 789, Returned days is:");
  Serial.println(numDaysSince(11, 3, 2016));
  Serial.println("");

    Serial.print("Should return 788, Returned days is:");
  Serial.println(numDaysSince(12, 3, 2016));
  Serial.println("");

  
    Serial.print("Should return 794, Returned days is:");
  Serial.println(numDaysSince(6, 3, 2016));
  Serial.println("");

  Serial.print("Should return 729, Returned days is:");
  Serial.println(numDaysSince(10, 5, 2016));
  Serial.println("");

   Serial.print("Should return 698, Returned days is:");
  Serial.println(numDaysSince(10, 6, 2016));
  Serial.println("");

  Serial.print("Should return 1096, Returned days is:");
  Serial.println(numDaysSince(8, 5, 2015));
  Serial.println("");

 delay(20000);
}


int numMonthsSince(byte myDays, byte myMonths, int myYears){
  return (numYearsSince(myYears, curYear) * 12) + (12 - myMonths) + curMonths;
}


byte numYearsSince(int myYears, int curYears){
  return curYears - myYears;
}


//calculates and returns the number of days since the a certain date
int numDaysSince(byte myDays, byte myMonths, int myYears){
  int totalDays = 0;

  //handle the year in between the two years only if 
  for(byte yearCount = 1; yearCount <= numYearsSince(myYears, curYear) - 1; yearCount++){
    if (isLeapYear(myYears + yearCount)){ 
       totalDays = totalDays + 366;
       Serial.println("Is leap year");
    }
    else totalDays = totalDays + 365; 
  }


  //if not same year, add all months past in myYear and curYear
  if(myYears != curYear){
    //handles the months before in year
    for(byte countMonth = myMonths; countMonth <= 12; countMonth++)
      totalDays = totalDays + daysInMonth(countMonth, myYears); 
  
    //handles the months in curYear
    for(byte countCurMonth = myMonths - 1; countCurMonth >= 1; countCurMonth--)
      totalDays = totalDays + daysInMonth(countCurMonth, myYears);
  }else{
    //if same year, only add the month in between them
    for(byte countMonth = myMonths + 1; countMonth < curMonths; countMonth++)
      totalDays = totalDays + daysInMonth(countMonth, myYears);
  }    
/*
  //if in different months, add days past in the months
  if(myMonths != curMonths){
    //handles the days in years
    totalDays = totalDays + (daysInMonth(myMonths, myYears) - myDays);
  
    //handles the days in curYear
    totalDays = totalDays + curDay;
  } else{
    //otherwise, only add the difference 
    totalDays = totalDays + (curDay - myDays);
  }
  */
  return totalDays;
}

byte daysInMonth(byte myMonths, int myYears){
  switch(myMonths % 12){
      case 4:
      case 6:
      case 9:
      case 11: return 30;
      case 2: if(isLeapYear(myYears)) return 29;
              else return 28;
      default: return 31;        
  }   
}


//referenced from https://www.programiz.com/c-programming/examples/leap-year
bool isLeapYear(int myYears){
  if(myYears % 4 == 0)
    {
        if( myYears % 100 == 0)
        {
            // year is divisible by 400, hence the year is a leap year
            if ( myYears % 400 == 0){
                return true;
            }   
            else
                return false;
        }
        else{
            return true;
        }    
    }
    else
       return false;
}
