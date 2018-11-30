#include <AccelStepper.h>

// BLUETOOTH COMMANDO ENABLED on Serial1


// Version
int version = 3;

// Pin Assignments

// Hour Hand Motor Pin Assignments
int HOUR_MOTOR      = 0;
int HOUR_STEP       = 35;
int HOUR_DIRECTION  = 36;
int HOUR_SLEEP      = 37;
int HOUR_INTERRUPT  = 21;

// Min Hand Motor Pin Assignments
int MIN_MOTOR     = 1;
int MIN_STEP      = 22;
int MIN_DIRECTION = 23;
int MIN_SLEEP     = 24;
int MIN_INTERRUPT = 20;

// LED Pin Assignments
int LED_STATUS   = 62;
int LED_WORKING  = 63;
int LED_COMMS    = 64;

// Error Pulses on Status LED For Each Error Case.
int HOUR_ERROR  = 3;
int MIN_ERROR   = 5;
int RTC_ERROR   = 7;
int COMMS_ERROR = 9;

// Pass Pulses on Status LED for each good case
int HOUR_PASS  = 2;
int MIN_PASS   = 4;
int RTC_PASS   = 6;
int COMMS_PASS = 8;

// Animation Poll from Serial1 For Start Animation
int ANIMATE_POLL = 500;

// Animation Parameters
int animate_starthour = 9;
int animate_startmin = 0;
int animate_waittime = 10;   // 0 - indicates wait for interrupt
int animate_endhour = 12;
int animate_endmin = 0;
int animate_spins = 11;
int animate_speed = 800;
int animate_accleration = 800;



// Define 2 Stepper Motors For the Hour and Minuite Hands.
AccelStepper hourstepper(1,HOUR_STEP, HOUR_DIRECTION ) ;  // Hour Hand Stepper
AccelStepper  minstepper(1,MIN_STEP,MIN_DIRECTION );    // Minuite Hand Stepoper

int homecounthour = 0;
int homecountmin = 0;
volatile int lastinthour = 0;
volatile int lastintmin = 0;
unsigned long threshold = 1000; // approximately 1ms since 1st interrupt - Debounce Opto Interrupts


// Assume Both Motors are symmetric and have similar Steps per revolution
//long stepsperrevolution = 200; // Full Setp
//long stepsperrevolution = 400; // Half Setp
long stepsperrevolution = 800; // Quater Setp
//long stepsperrevolution = 1600; // Eighth Setp
//long stepsperrevolution = 1600; // Sixteenth Setp

// Error Adjustment Pending Current Setting on A4988
long hourhomepad = 5; // was 10
long minhomepad = 6; // was 10
long hourerrorsteps = 25; // For Missing Step Compensation.
long minerrorsteps = 40;

// Common Motor Parameters for Speed and Accleration
int homingspeed = 500; /// Motor speed in steps / second for HOMING action
int runningspeed = 400; /// Motor speed in steps / second for RUN action 1.5 RPS
int testspeed = 400; /// Motor speed in steps / second for TESTING

float  homeacceleration = 600;
float  runningacceleration = 600;
float  testacceleration = 600;


void optointerrupthour() // Interupt Handler for Photo Interrupt
{
  unsigned long now;
  if( (now = micros() - lastinthour ) > threshold ) // Soft Debounce Opto Interrupter
      ++homecounthour;
  lastinthour = now;

}

void optointerruptmin() // Interupt Handler for Photo Interrupt
{
  unsigned long now;
  if( (now = micros() - lastintmin ) > threshold ) // Soft Debounce Opto Interrupter
      ++homecountmin;
  lastintmin = now;

}

void setup()
{

   // Initialize pins

   pinMode( LED_STATUS, OUTPUT ); // Status LED
   pinMode( LED_WORKING, OUTPUT ); // Status Led
   pinMode( LED_COMMS, OUTPUT );

   pinMode( HOUR_SLEEP, OUTPUT );
   pinMode( MIN_SLEEP,OUTPUT );

   pinMode( HOUR_INTERRUPT, INPUT );
   pinMode( MIN_INTERRUPT, INPUT );

   // Motors All SLEEP
   digitalWrite( HOUR_SLEEP, LOW );  // Motor Sleep Startup
   digitalWrite( MIN_INTERRUPT, LOW );

   // LEDs all OFF
   digitalWrite( LED_STATUS, LOW );
   digitalWrite( LED_WORKING, LOW );
   digitalWrite( LED_COMMS, LOW );

   // Initialize Iterrupt Pins
   digitalWrite( HOUR_INTERRUPT, LOW );
   digitalWrite( MIN_INTERRUPT, LOW );

   testLEDS(); // Test All Status Leds

   lastintmin = 0;
   lastinthour = 0;
   homecountmin = 0;
   homecounthour = 0;

   // Setup Interrupts For Motor Homing
   attachInterrupt( 2, optointerrupthour, RISING );  // Hard Interrupt 0
   attachInterrupt( 3, optointerruptmin, RISING );   // Hard Interrupt 1

   Serial1.begin( 9600 );

   Serial1.println();
   Serial1.println();
   Serial1.println();
   Serial1.println();
   Serial1.println();
   Serial1.println();
   Serial1.println();
   Serial1.println();
   Serial1.println();


   Serial1.println( "(@) Copyright Scott Animators Inc. " );
   Serial1.println( "Starting Clock Animation ..." );
   Serial1.print( "Version : " );
   Serial1.println( version );
   Serial1.print( "Date : " );
   Serial1.println( __DATE__ );
   Serial1.print( "Time : " );
   Serial1.println( __TIME__ );

   int seed = micros() * analogRead(0);
   randomSeed( seed );

   Serial1.print( "Seed " );
   Serial1.println( seed );

   //testMotor( HOUR_MOTOR, hourstepper, &homecounthour ); // Blocking
   //testMotor( MIN_MOTOR, minstepper, &homecountmin ); // Blocking

   //homeMotor( HOUR_MOTOR, 1, hourstepper, &homecounthour );
   //homeMotor( MIN_MOTOR, 1, minstepper, &homecountmin );

   //doSpins( hourstepper, minstepper, 5 );

   homeMotor( HOUR_MOTOR, 1, hourstepper, &homecounthour );
   homeMotor( MIN_MOTOR, 1, minstepper, &homecountmin );

   while( 1 ) // Forever
   {
     //if (getParameters() < 0)
       //break;

     int command = getCommand();

     if (command == 1)
       {
       doAnimation( animate_waittime );
       continue;
       }

     if (command == 6)
       {
       doSpins( hourstepper, minstepper, 10 );
       continue;
       }

     if (command == 7)
       {
       homeMotor( HOUR_MOTOR, 1, hourstepper, &homecounthour );
       homeMotor( MIN_MOTOR, 1, minstepper, &homecountmin );
       continue;
       }

     if (command == 8)
       {
       testMotor( HOUR_MOTOR, hourstepper, &homecounthour ); // Blocking
       continue;
       }

     if (command == 9)
       {
       testMotor( MIN_MOTOR, minstepper, &homecountmin ); // Blocking
        continue;
       }

     if(command == 2)
       break;

   }

   // homeMotor( HOUR_MOTOR, 1, hourstepper, &homecounthour );
   // homeMotor( MIN_MOTOR, 1, minstepper, &homecountmin );

   // Should never hit here

   terminate();

}

// Termination Indication
void terminate()
{

    Serial1.println( "System Terminated ... " );

    while(1)
    {
      digitalWrite( LED_STATUS, HIGH );
      digitalWrite( LED_WORKING, HIGH );
      digitalWrite( LED_COMMS, HIGH );
      delay(100);
      digitalWrite( LED_STATUS, LOW );
      digitalWrite( LED_WORKING, LOW  );
      digitalWrite( LED_COMMS, LOW );
      delay(300);
    }
}

int getCommand()
{

      // Input Animation Parameters over Serial1.
      // Start Time Hour 1-12
      // Start Min Position in 5 Min Increments - possible values are 00, 05, 10 , .... 55
      // Default is 9:00
      //
      // End Time Always Assumed 12:00 Default
      // Number of Spins
      // Speed ( Steps per second assumed at 800 steps per revolution
      //

   digitalWrite( LED_COMMS, HIGH );

   int command = 7; // Default = Test
   int prompt = 0;

   while(1) // Action Big Loop
   {

     if (!prompt)
     {
       Serial1.println( "Menu Enter Any One .. ");
       Serial1.println( "(1)Animate " );
       Serial1.println( "(2)Terminate" );
       Serial1.println( "(6)Spin Ten" );
       Serial1.println( "(7)Home Motors" );
       Serial1.println( "(8)Test Hour" );
       Serial1.println( "(9) Test Minuite " );
       prompt = 1;
     }

     int available = Serial1.available();
     if ( available <= 0 )
       {
       delay(ANIMATE_POLL);
       continue;
       }

       command = Serial1.parseInt();
       while(Serial1.available() > 0 ) // Gobble Any Shit on the Line Till nothing left
       {
          char crap  = Serial1.read();
       }

       break;
   }

  return command;

}

int getParameters()
{

      // Input Animation Parameters over Serial1.
      // Start Time Hour 1-12
      // Start Min Position in 5 Min Increments - possible values are 00, 05, 10 , .... 55
      // Default is 9:00
      //
      // End Time Always Assumed 12:00 Default
      // Number of Spins
      // Speed ( Steps per second assumed at 800 steps per revolution
      //

   digitalWrite( LED_COMMS, HIGH );

   int ask = 0;

   while(1) // Action Big Loop
   {
     if (!ask)
     {
       Serial1.println( "Whats is the Deleo Scott HH MM W HH MM Spped Accleration Turns ? " );
       ask = 1;
     }

     int available = Serial1.available();
     if ( available <= 0 )
       {
       delay(ANIMATE_POLL);
       continue;
       }

     // Got somthing , read string to new line and convert
     // Expect single digit number 0 - 9 \n

     while(available)
     {
       animate_starthour = 0;
       animate_startmin = 0;
       animate_waittime = 0;
       animate_endhour = 0;
       animate_endmin = 0;
       animate_speed = 0;
       animate_accleration = 0;
       animate_spins = 0;

       animate_starthour = Serial1.parseInt();
       if (animate_starthour < 0)
       {
         digitalWrite( LED_COMMS, LOW );
         return -1;
       }

       animate_startmin = Serial1.parseInt();
       animate_waittime = Serial1.parseInt();
       animate_endhour = Serial1.parseInt();
       animate_endmin = Serial1.parseInt();
       animate_speed = Serial1.parseInt();
       animate_accleration = Serial1.parseInt();
       animate_spins = Serial1.parseInt();


       // Consume byles till newline igonoring anything else

       while(1) // Sender Alwasy Terminates Transmission with New Line
       {
          char inc = Serial1.read();
          if (inc == '\n')
              break;
       }

       dumpAnimationParameters();

       if (animate_starthour > 0 &&
           animate_endhour > 0 &&
           animate_waittime >= 0 &&
           animate_startmin >= 0 &&
           animate_endmin >= 0 &&
           animate_speed > 0 &&
           animate_accleration > 0 &&
           animate_spins >= 0 )
         break;
        else
          ask = 1;


     }

     animate_starthour = constrain( animate_starthour, 1, 12 );
     animate_startmin = constrain( animate_startmin, 0, 55 );
     animate_waittime = constrain( animate_waittime, 0, 60 );
     animate_endhour = constrain( animate_endhour, 1, 12 );
     animate_endmin = constrain( animate_endmin, 0, 55 );
     animate_spins = constrain( animate_spins, 0, 12 );
     animate_accleration = constrain( animate_accleration, 100,400 );
     animate_speed = constrain( animate_speed, 100,400 );

     if (animate_startmin % 5)
       animate_startmin = (animate_startmin / 5) * 5;

     if (animate_endmin % 5)
       animate_endmin = (animate_endmin / 5) * 5;

     digitalWrite( LED_COMMS, LOW );

     break;

  }

}

void dumpAnimationParameters()
{

     Serial1.print( "Start Time " );
     Serial1.print( animate_starthour );
     Serial1.print( ":" );
     Serial1.println( animate_startmin );

     Serial1.print( "End Time " );
     Serial1.print( animate_endhour );
     Serial1.print( ":" );
     Serial1.println( animate_endmin );

     Serial1.print( "Wait Time ");
     Serial1.println( animate_waittime );

     Serial1.print( "Speed " );
     Serial1.println( animate_speed );

     Serial1.print( "Accleration " );
     Serial1.println( animate_accleration );

     Serial1.print( "Spins " );
     Serial1.println( animate_spins );
}


void doAnimation( int waittime )
{

    // Assume Motors are at HOME Position To Start
    // Compute Steps needed to get to start position

   dumpAnimationParameters();

   float stepsperdegree = stepsperrevolution / 360.0;

   digitalWrite( LED_STATUS, HIGH );

   Serial1.print( "Steps Per degree " );
   Serial1.println( stepsperdegree,3 );


   long hsteps2go = (long)(animate_starthour * 30 * stepsperdegree); // Degrees per hour = 30 , steps per degree.
   long msteps2go = (long)(animate_startmin * 6 * stepsperdegree); // Degrees per min = 6 , steps per degree.


   // Compensate for Missteps at the error rate specified steps every revolution
   hsteps2go = (long)( hsteps2go + ( hsteps2go * (long)hourerrorsteps / stepsperrevolution ) );

   // Compensate for Missteps at the error rate specified steps every revolution
   msteps2go = (long)( msteps2go + ( msteps2go * (long)minerrorsteps / stepsperrevolution ) );

   Serial1.println( "Animation Started ... " );

   Serial1.print( "Hour Position " );
   Serial1.print( animate_starthour );
   Serial1.print( " Steps " );
   Serial1.println( hsteps2go );

   Serial1.print( "Min Position " );
   Serial1.print( animate_startmin );
   Serial1.print( " Steps " );
   Serial1.println( msteps2go );

   moveMotors( animate_speed, animate_accleration, hsteps2go, msteps2go );

   digitalWrite( LED_STATUS, LOW );

    // Go to Start Time and if asked Wait for Interrupt to finish rest of animation steps
    // I waittime = 0 then wait for interrupt otherwise wait the quantity of wait time in seconds.

   if (!waittime)
      wait4Comms();
   else
      delay( waittime * 1000 );

   long antiphasesteps  = animate_spins * stepsperrevolution;

  // Add Ending Time Steps
  // Add 16 Mins to Hour Hand
  long padhoursteps = 26 * 6 * stepsperdegree;  // Compensation for Lost Steps
  long padminsteps = 7 * 6 * stepsperdegree;

   moveMotors( animate_speed, animate_accleration, antiphasesteps + padhoursteps , -(antiphasesteps + padminsteps) );

   good2go( 10 );

}

void wait4Comms()
{

   digitalWrite( LED_COMMS, HIGH );

   Serial1.println( "Start Animation ? " );

   while( 1 )
   {
     int available = Serial1.available();

     if ( available <= 0 )
        {
        delay(ANIMATE_POLL);
        continue;
        }
     else
       {
         /*
         while(1)
         {
         char junk = Serial1,read();
         if ( Serial1.available() == 0 )
           break;
         }
         */
         break;
       }
   }

  Serial1.println( "Starting Animation... " );

  digitalWrite( LED_COMMS, LOW );

}

// Move Hour and Min Motoros from Current Position
void moveMotors( int mspeed, int maccleration , long hoursteps, long minsteps )
{

      AccelStepper hstepper = hourstepper;
      AccelStepper mstepper = minstepper;

       digitalWrite( LED_STATUS, HIGH );

       // Wake Motors
       digitalWrite( HOUR_SLEEP, HIGH );
       digitalWrite( MIN_SLEEP,  HIGH );

       hstepper.setMaxSpeed( mspeed );
       hstepper.setAcceleration( maccleration );
       hstepper.setCurrentPosition( 0 );
       hstepper.moveTo( hoursteps );
       hstepper.setSpeed( mspeed );

       mstepper.setMaxSpeed( mspeed );
       mstepper.setAcceleration( maccleration );
       mstepper.setCurrentPosition( 0 );
       mstepper.moveTo( minsteps );
       mstepper.setSpeed( mspeed );

       int donemin = 0;
       int donehour = 0;

       while(1)
       {

          if (!donehour)
          {
            if(hstepper.distanceToGo())
               hstepper.runSpeedToPosition();
            else
              donehour++;
          }
          if (!donemin)
          {
            if( mstepper.distanceToGo())
               mstepper.runSpeedToPosition();
            else
              donemin++;
          }

          if(donemin && donehour)
          {
            mstepper.runSpeedToPosition();
            hstepper.runSpeedToPosition();
            break;
          }

       }

       Serial1.print( "Move Done MIN " );
       Serial1.print( donemin );
       Serial1.print( " HOUR " );
       Serial1.println( donehour );

       // Sleep Motors
       digitalWrite( HOUR_SLEEP, LOW );
       digitalWrite( MIN_SLEEP,  LOW );

       digitalWrite( LED_STATUS, LOW );


}

// AntiPhase Spins Hour and Min Hands
void doSpins( AccelStepper hstepper, AccelStepper mstepper, int count )
{

   if (count)
   {
       long revsteps  = count * stepsperrevolution;

       Serial1.print( "Antiphase Revolution Test " );
       Serial1.println( revsteps );

       digitalWrite( LED_STATUS, HIGH );

       // Wake Motors
       digitalWrite( HOUR_SLEEP, HIGH );
       digitalWrite( MIN_SLEEP,  HIGH );

       hstepper.setMaxSpeed( runningspeed );
       hstepper.setAcceleration( runningacceleration );
       hstepper.setCurrentPosition( 0 );
       hstepper.moveTo( revsteps );
       hstepper.setSpeed( runningspeed );

       mstepper.setMaxSpeed( runningspeed );
       mstepper.setAcceleration( runningacceleration );
       mstepper.setCurrentPosition( 0 );
       mstepper.moveTo( revsteps * -1 );
       mstepper.setSpeed( runningspeed );

       int donehour = 0;
       int donemin = 0;
       while(1)
       {
          if(!donehour && hstepper.distanceToGo())
             hstepper.runSpeedToPosition();
          else
            donehour++;
          if(!donemin && mstepper.distanceToGo())
             mstepper.runSpeedToPosition();
          else
            donemin++;

          if(donehour && donemin)
            break;
       }

       // Sleep Motors
       digitalWrite( HOUR_SLEEP, LOW );
       digitalWrite( MIN_SLEEP,  LOW );

       digitalWrite( LED_STATUS, LOW );

       good2go( 10 );
       delay(1000);

       Serial1.println( "Antiphase Revolution Test Complete " );
       Serial1.print( "Hour Interrupts " );
       Serial1.println( homecounthour );
       Serial1.print( "Minute Interrupts " );
       Serial1.println( homecountmin );

   }

}

void testLEDS()
{

   digitalWrite( LED_STATUS, LOW );
   digitalWrite( LED_WORKING, LOW );
   digitalWrite( LED_COMMS, LOW );


   cycleLED( LED_STATUS, 2 );
   cycleLED( LED_WORKING, 2 );
   cycleLED( LED_COMMS, 2 );


}

void cycleLED( int led, int count )
{

  for( int i=0;i<count;i++ )
        {
        digitalWrite( led, HIGH );
        delay(30);
        digitalWrite( led, LOW );
        delay(200);
        }

   delay(1000);

}



// System Panic Indicator
// Number Of Flashed indicate subsystem Error

void panic( int count )
{

   Serial1.println( "Panic ..." );

   while(1) // Never Return till Power Down or Resst
      {
      for( int i=0;i<count;i++ )
        {
        digitalWrite( LED_STATUS, HIGH );
        delay(30);
        digitalWrite( LED_STATUS, LOW );
        delay(200);
        }
      delay(2000);
      }

}

// System Good Indicator

void good2go( int count )
{

  for( int i=0;i<count;i++ )
      {
      digitalWrite( LED_STATUS,HIGH );
      delay( 200 );
      digitalWrite( LED_STATUS,LOW );
      delay(1000);
      }

}

/*

  Test Specific Motor
  Engage TEST LED in each case
  Make 2 Revolutions and Count HOME INTERRUPTS
  IF GOOD THEN 4 LONG LED FLASH
  IF BAD  THEN FATAL FLASH

*/
void testMotor( int motor, AccelStepper stepper , int *homecount )
{

  char *motorstring;
  int motorsleeppin = 0;
  int paniccount = 0;
  int goodcount = 0;

  if (motor == HOUR_MOTOR )
  {
     motorstring = "HOUR";
     motorsleeppin = HOUR_SLEEP;
     paniccount = HOUR_ERROR;
     goodcount = HOUR_PASS;
  }

  if (motor == MIN_MOTOR)
  {
     motorstring = "MIN";
     motorsleeppin = MIN_SLEEP;
     paniccount = MIN_ERROR;
     goodcount = MIN_PASS;
  }

  Serial1.print( "Self Testing Motor and Interrupter For " );
  Serial1.println( motorstring );


  digitalWrite( LED_STATUS, HIGH ); // Set Working LED ON

  digitalWrite( motorsleeppin, HIGH ); // Wakeup Motor

  stepper.setMaxSpeed( testspeed );
  stepper.setSpeed( testspeed );
  stepper.setAcceleration( testacceleration );

  long randomposition = random( 10,stepsperrevolution-30 ); /// random position set to avoid false count;

  Serial1.print( "Random position " );
  Serial1.println( randomposition );

  stepper.setCurrentPosition( 0 );
  stepper.moveTo( (long)randomposition );  // Make 1 Revolution
  stepper.runToPosition();   // Blocking

  delay(1000);

  *homecount = 0; // Initialze Interrupted Home Count

  long position = stepsperrevolution;

  for( int i=0; i<2; i++ )
    {
    stepper.setCurrentPosition( 0 );
    stepper.moveTo( position );  // Make 1 Revolution
    stepper.runToPosition();   // Blocking
    }

  position = -1 * position;

  delay(1000);

  for( int i=0; i<2; i++ )
    {
    stepper.setCurrentPosition( 0 );
    stepper.moveTo( position  );  // Make 1 Revolution
    stepper.runToPosition();   // Blocking
    }


  stepper.stop();
  stepper.setCurrentPosition(0);

  digitalWrite( motorsleeppin, LOW ); // Sleep Motors

  digitalWrite( LED_STATUS , LOW );

  if ( *homecount != 4 )
    {
    Serial1.print( "Test Error Interrupts " );
    Serial1.print( motorstring );
    Serial1.print( " " );
    Serial1.println( (int)*homecount );
    panic( paniccount );
    }
  else
    good2go( goodcount  );

  Serial1.print( "Testing Passed " );
  Serial1.println( motorstring );

}

/*

  Home Specific Motor
  Engage TEST LED in each case
  Make

*/

void homeMotor(  int motor, int dorandom, AccelStepper stepper, int *homecount )
{

  char *motorstring;
  int motorsleeppin = 0;
  int paniccount = 0;
  int goodcount = 0;
  long homepad = 0;

  if (motor == HOUR_MOTOR)
  {
     motorstring = "HOUR";
     motorsleeppin = HOUR_SLEEP;
     paniccount = HOUR_ERROR;
     goodcount = HOUR_PASS;
     homepad = hourhomepad;
  }

  if (motor == MIN_MOTOR)
  {
     motorstring = "MIN";
     motorsleeppin = MIN_SLEEP;
     paniccount = MIN_ERROR;
     goodcount = MIN_PASS;
     homepad = minhomepad;
  }

  Serial1.print( "Homing Motor " );
  Serial1.println( motorstring );

  digitalWrite( LED_STATUS, HIGH );

  digitalWrite( motorsleeppin, HIGH ); // Wakeup Motor from Sleep

  stepper.setMaxSpeed( homingspeed );
  stepper.setAcceleration( homeacceleration );

  if (dorandom)
  {
     long randomposition = random( 10,stepsperrevolution-30 ); /// random position set to avoid false count;

     Serial1.print( "Random position " );
     Serial1.println( randomposition );

     stepper.setCurrentPosition( 0 );
     stepper.moveTo( (long)randomposition );  // Make 1 Revolution
     stepper.runToPosition();   // Blocking
     delay(1000);
  }

  stepper.setCurrentPosition( 0 );
  stepper.moveTo( stepsperrevolution+stepsperrevolution/2 );  // Make 1 Revolution
  stepper.setSpeed( homingspeed );

  int localhomecount = 0;
  *homecount = 0;

  while(1)
    {
    if (stepper.distanceToGo() == 0)
      break;
    stepper.runSpeedToPosition();
    delay(0);
    if ( *homecount == 1 )
      break;

    }

  localhomecount = *homecount;

  // Pad Forward Home Position 12 O'Clock

  stepper.setCurrentPosition(0);
  stepper.moveTo(homepad);
  stepper.setSpeed(homingspeed);
  stepper.runToPosition();

  stepper.stop();
  stepper.setCurrentPosition(0);

  digitalWrite( motorsleeppin, LOW ); // Sleep Motors

  digitalWrite( LED_STATUS, LOW );

  if (localhomecount != 1) // Failed To Home This Stepper.
  {
    Serial1.print( "Homing Error Interrupts " );
    Serial1.println( (int)*homecount );
    panic( paniccount );
  }
  else
    good2go( goodcount );


  *homecount = 0;

  Serial1.print( "Homing Complete " );
  Serial1.println( motorstring );

   // TODO , May have to Retrace a few Steps

}

void loop()
{

}
