// millisDelay.h
#ifndef MILLIS_DELAY_H
#define MILLIS_DELAY_H

/*
 * (c)2018 Forward Computing and Control Pty. Ltd.
 * NSW Australia, www.forward.com.au
 * This code is not warranted to be fit for any purpose. You may only use it at your own risk.
 * This generated code may be freely used for both private and commercial use
 * provided this copyright is maintained.
 */
 
class millisDelay {
  public:

    millisDelay();

    /**
      Start a delay of this many milliseconds
      @param delay in millisconds, 0 means ifFinished() return true on first call
    */
    void start(unsigned long delay);

    /**
       Stop the delay
       justFinished() will now never return true
       until after start(),restart() or repeat() called again
    */
    void stop();

    /**
      repeat()
      Do same delay again but allow for a possible delay in calling justFinished()
    */
    void repeat();

    /**
      restart()
      Start the same delay again starting from now
      Note: use repeat() when justFinished() returns true, if you want a regular repeating delay
    */
    void restart();

    /**
       Force delay to end now
    */
    void finish();

    /**
      Has the delay ended/expired or has finish() been called?
      justFinished() returns true just once when delay first exceeded or the first time it is called after finish() called
    */
    bool justFinished();

    /**
      Is the delay running, i.e. justFinished() will return true at some time in the future
    */
    bool isRunning();

    /**
      Returns the last time this delay was started, in mS, by calling start(), repeat() or restart()
      Returns 0 if it has never been started
    */
    unsigned long getStartTime();

    /**
      How many mS remaining until delay finishes
      Returns 0 if finished or stopped
    */
    unsigned long remaining();

    /**
      The delay set in start
    */
    unsigned long delay();

  private:
    unsigned long mS_delay;
    unsigned long startTime;
    bool running; // true if delay running false when ended
    bool finishNow; // true if finish() called to finish delay early, false after justFinished() returns true
};
#endif
