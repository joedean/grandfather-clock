                    \           
//
// The Scott Clock Animator
//


#include <AccelStepper.h>

AccelStepper hourstepper(1,12,13) ;  // Hour Hand Stepper
//AccelStepper minstepper(1,14,15);    // Minuite Hand Stepoper

int homecounthour = 0;
volatile int lastinthour = 0;

//volatile int homecountmin = 0;
//volatile int lastintmin = 0;
   
int stepsperrevolution = 200; // Full Setp
//int stepsperrevolution = 400; // Half Setp
//int stepsperrevolution = 800; // Quater Setp
//int stepsperrevolution = 1600; // Eighth Setp
//int stepsperrevolution = 1600; // Sixteenth Setp


int revolutions = 0;

volatile unsigned long lastint = 0;
unsigned long threshold = 1000; // approximately 1ms since 1st interrupt

int homingspeed = 75; /// Motor speed in steps / second for HOMING action
int runningspeed = 200; /// Motor speed in steps / second for RUN action
int testspeed = 200; /// Motor speed in steps / second for TESTING


void optointerrupthour() // Interupt Handler for Photo Interrupt 
{
  unsigned long now;
  if( (now = micros() - lastint ) > threshold ) // Soft Debounce Opto Interrupter
      ++homecounthour;
  lastint = now;
  
}

void optointerruptmin() // Interupt Handler for Photo Interrupt 
{
  unsigned long now;
  if( (now = micros() - lastint ) > threshold ) // Soft Debounce Opto Interrupter
      ++homecounthour;
  lastint = now;
  
}

void setup()
{  
  
   // Initialize pins 
   
   pinMode( 4, OUTPUT ); // Work In Progress LED
   pinMode( 5, OUTPUT ); // Status Led
   pinMode( 2, INPUT );  // Opto Interrupt 
   pinMode( 6, OUTPUT ); // Sleep Motor

   digitalWrite( 4, LOW );
   digitalWrite( 5, LOW );
   digitalWrite( 6, LOW );
   
   randomSeed( micros() );
   
   //homecountmin = 0;
   homecounthour = 0;
   
   attachInterrupt( 0, optointerrupthour, RISING );
   //attachInterrupt( 0, optointerruptmin, RISING );

   Serial.begin( 115200 );
   
   Serial.println();
   Serial.println();
   Serial.println( "Starting Scott Clock Animation" );
   
   testMotor( hourstepper, &homecounthour ); // Blocking
   //testMotor( mintepper, &homecountmin ); // Blocking
   
   homeMotor( hourstepper, &homecounthour );
   //homeMotor( minstepper, &homecountmin );
   
   // SetTime( hourstepper, minstepper ); // Blocking

   // Wait For Animation Start Signal
   
   // doAnimation( hourstepper, minstepper );
   
   // Wait For Animation Stop Signal
   

   //homeMotor( minstepper,  &homecountermin );
   
   clock2position( hourstepper, &homecounthour, 0 ,7, 10 );
   
   good2go();
   delay(10);
   
   homeMotor( hourstepper, &homecounthour );
   
   
   while(1)   // Done Just Sit Tight For Now
      good2go();
   
}

// Fast Clock work to a given hour position from a given position in hours

void clock2position( AccelStepper stepper, int *homecount, int starthour, int hourtarget, int revolutions )
{
   if (!hourtarget)
     return;
   
   if (!starthour)
     homeMotor( stepper, homecount );

   digitalWrite( 4, HIGH );
    
   float stepsperdegree = stepsperrevolution / 360.0;
   Serial.print( "Steps Per degree " );
   Serial.println( stepsperdegree,3 );
   
   int steps2go = (int)(hourtarget * 30 * stepsperdegree); // Degrees per hour = 30 , steps per degree.
   
   Serial.print( "Hour Position " );
   Serial.println( hourtarget );
   Serial.print( "Steps2go " );
   Serial.println( steps2go );
   
   digitalWrite( 6, HIGH ); // Wakeup Motor from Sleep
   
   stepper.setMaxSpeed( runningspeed );  
   stepper.setSpeed( runningspeed );
   stepper.setAcceleration( 3000 ); 
   
   // Do Revolutions if there are any
   
   if (revolutions)
   {
       int revsteps  = revolutions * stepsperrevolution; 
       stepper.setCurrentPosition( 0 );
       stepper.move( revsteps );
       stepper.runToPosition();
   }
   
   
   stepper.setCurrentPosition( 0 );
   stepper.move( steps2go );  // Run to computed position
   stepper.runToPosition();   // Blocking
   
   digitalWrite( 6, LOW ); // Sleep Motor from Sleep

   digitalWrite( 4, LOW );
   
}
  
  
void doAnimation( AccelStepper hourstepper, AccelStepper minstepper )
{
  
}

void SetTime( AccelStepper hoursteper, AccelStepper minstepper )
{
  
  
}


// System Panic Indicator

void panic()
{
  
   while(1) // Never Return till Power Down or Resst
      {
      digitalWrite( 5, HIGH );
      delay(50);
      digitalWrite( 5, LOW );
      delay(100); 
      digitalWrite( 5, HIGH );
      delay(50);
      digitalWrite( 5, LOW );
      delay(1000);
      }
      
}

// System Good Indicator

void good2go()
{
  
  for( int i=0;i<4;i++ )
      {
      digitalWrite( 5,HIGH );
      delay( 200 );
      digitalWrite( 5,LOW );
      delay(1000);
      }
 
}

/*

  Test Motors Sequentially
  Engage TEST LED in each case
  Make 2 Revolutions and Count HOME INTERRUPTS
  IF GOOD THEN 4 LONG LED FLASH
  IF BAD  THEN FATAL FLASH
  
*/
void testMotor( AccelStepper stepper , int *homecount )
{
  

  Serial.println();
  Serial.println( "Test" );
  
  digitalWrite( 4, HIGH );
  
  digitalWrite( 6, HIGH ); // Wakeup Motor from Sleep
  
  stepper.setMaxSpeed( testspeed );  
  stepper.setSpeed( testspeed );
  stepper.setAcceleration( 3000 );

  long randomposition = random( 30,stepsperrevolution-30 ); /// random position set to avoid false count;
  
  Serial.println( "Random position " );
  Serial.print( randomposition );
  
  stepper.setCurrentPosition( 0 );
  stepper.move( (int)randomposition );  // Make 1 Revolution
  stepper.runToPosition();   // Blocking

  delay(1000);

  *homecount = 0; // Initialze Interrupted Home Count
  
  int position = stepsperrevolution + 10;

  for( int i=0; i<2; i++ )
    {        
    stepper.setCurrentPosition( 0 );
    stepper.move( position );  // Make 1 Revolution
    stepper.runToPosition();   // Blocking
    }
   
  position = -1 * position;
  delay(1000);
  
  for( int i=0; i<2; i++ )
    {        
    stepper.setCurrentPosition( 0 );
    stepper.move( position  );  // Make 1 Revolution
    stepper.runToPosition();   // Blocking
    }
    
  stepper.stop();
  stepper.setCurrentPosition(0);
  
  digitalWrite( 6, LOW ); // Sleep Motors  
  
  digitalWrite( 4, LOW );
  
  if ( *homecount != 4 )
    panic();
  else  
    good2go();


}

/*

  Home Motors Sequentially
  Engage TEST LED in each case
  Make 
  
*/

void homeMotor(  AccelStepper stepper, int *homecount )
{
  
  Serial.println();
  Serial.println( "HOME" );
  
  
  digitalWrite( 4, HIGH );

  digitalWrite( 6, HIGH ); // Wakeup Motor from Sleep
  
  stepper.setMaxSpeed( homingspeed );  
  stepper.setSpeed( homingspeed );
  stepper.setAcceleration( 10000 );
  
  stepper.setCurrentPosition( 0 );
  stepper.moveTo( stepsperrevolution+10 );  // Make 1 Revolution
  
  int localhomecount = 0;
  *homecount = 0;
  
  while(1)
    {        
    stepper.run();
    if ( *homecount == 1 )
      break;
    if (stepper.distanceToGo() == 0)
      break;
    }
   
  stepper.stop();
  stepper.setCurrentPosition(0);
    
  digitalWrite( 6, LOW ); // Sleep Motors
  
  digitalWrite( 4, LOW );
    
  if (*homecount != 1) // Failed To Home This Stepper.
    panic();
  else
    good2go();
   
   
   // TODO , May have to Retrace a few Steps

}

void loop()
{  
 
}