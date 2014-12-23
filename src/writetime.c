#include <stdio.h>
#include "writetime.h"

enum precision { exactly, nearly, after };

const char * const ONE = "one";
const char * const TWO = "two";
const char * const THREE = "three";
const char * const FOUR = "four";
const char * const FIVE = "five";
const char * const SIX = "six";
const char * const SEVEN = "seven";
const char * const EIGHT = "eight";
const char * const NINE = "nine";
const char * const TEN = "ten";
const char * const ELEVEN = "eleven";
const char * const TWELVE = "twelve";

const char * const TWENTY = "twenty";
const char * const TWENTY_FIVE = "twenty five";

const char * const EMPTY = "";
const char * const QUARTER = "quarter";
const char * const HALF = "half";
const char * const O_CLOCK = " o'clock";

const char * const PAST = " past ";
const char * const SPACE = " ";
const char * const TO = " to ";

const char * const EXACT = "bang on";
const char * const ALMOST = "almost";
const char * const JUST_GONE = "just after";

const char * const BORKED = "BORKED";

void write_time_buffer(int hours, int minutes, char *main_time_dest, char *precision_dest, size_t msize) {
  const char * m_str;
  const char *dir_str;
  enum precision prec;

  if (minutes < 3) {
    dir_str = EMPTY;
  } else if (minutes < 28) {
    dir_str = PAST;
  } else if (minutes < 33) {
    dir_str = SPACE;
  } else if (minutes > 57) {
    dir_str = EMPTY;
    hours++;
  } else {
    dir_str = TO;
    hours++;
  }

  // 12-hour clock only!
  if (hours > 12) hours -= 12;

  if      (minutes == 0)  { prec = exactly; m_str = EMPTY; }
  else if (minutes < 3 )  { prec = after;   m_str = EMPTY; }
  else if (minutes < 5)   { prec = nearly;  m_str = FIVE; }
  else if (minutes == 5)  { prec = exactly; m_str = FIVE; }
  else if (minutes < 8)   { prec = after;   m_str = FIVE; }
  else if (minutes < 10)  { prec = nearly;  m_str = TEN; }
  else if (minutes == 10) { prec = exactly; m_str = TEN; }
  else if (minutes < 13)  { prec = after;   m_str = TEN; }
  else if (minutes < 15)  { prec = nearly;  m_str = QUARTER; }
  else if (minutes == 15) { prec = exactly; m_str = QUARTER; }
  else if (minutes < 18)  { prec = after;   m_str = QUARTER; }
  else if (minutes < 20)  { prec = nearly;  m_str = TWENTY; }
  else if (minutes == 20) { prec = exactly; m_str = TWENTY; }
  else if (minutes < 23)  { prec = after;   m_str = TWENTY; }
  else if (minutes < 25)  { prec = nearly;  m_str = TWENTY_FIVE; }
  else if (minutes == 25) { prec = exactly; m_str = TWENTY_FIVE; }
  else if (minutes < 28)  { prec = after;   m_str = TWENTY_FIVE; }
  else if (minutes < 30)  { prec = nearly;  m_str = HALF; }
  else if (minutes == 30) { prec = exactly; m_str = HALF; }
  else if (minutes < 33)  { prec = after;   m_str = HALF; }
  else if (minutes < 35)  { prec = nearly;  m_str = TWENTY_FIVE; }
  else if (minutes == 35) { prec = exactly; m_str = TWENTY_FIVE; }
  else if (minutes < 38)  { prec = after;   m_str = TWENTY_FIVE; }
  else if (minutes < 40)  { prec = nearly;  m_str = TWENTY; }
  else if (minutes == 40) { prec = exactly; m_str = TWENTY; }
  else if (minutes < 43)  { prec = after;   m_str = TWENTY; }
  else if (minutes < 45)  { prec = nearly;  m_str = QUARTER; }
  else if (minutes == 45) { prec = exactly; m_str = QUARTER; }
  else if (minutes < 48)  { prec = after;   m_str = QUARTER; }
  else if (minutes < 50)  { prec = nearly;  m_str = TEN; }
  else if (minutes == 50) { prec = exactly; m_str = TEN; }
  else if (minutes < 53)  { prec = after;   m_str = TEN; }
  else if (minutes < 55)  { prec = nearly;  m_str = FIVE; }
  else if (minutes == 55) { prec = exactly; m_str = FIVE; }
  else if (minutes < 58)  { prec = after;   m_str = FIVE; }
  else                    { prec = nearly;  m_str = EMPTY; }

  const char *prec_str;
  switch (prec) {
    case exactly: prec_str = EXACT; break;
    case nearly:  prec_str = ALMOST; break;
    case after:   prec_str = JUST_GONE; break;
  }

  const char *hr_str;
  switch (hours) {
    case 0:
    case 12: hr_str = TWELVE; break;
    case 1:  hr_str = ONE; break;
    case 2:  hr_str = TWO; break;
    case 3:  hr_str = THREE; break;
    case 4:  hr_str = FOUR; break;
    case 5:  hr_str = FIVE; break;
    case 6:  hr_str = SIX; break;
    case 7:  hr_str = SEVEN; break;
    case 8:  hr_str = EIGHT; break;
    case 9:  hr_str = NINE; break;
    case 10: hr_str = TEN; break;
    case 11: hr_str = ELEVEN; break;
    default: hr_str = BORKED; break;
  }

  const char *oclock = minutes < 3 || minutes > 57 ? O_CLOCK : EMPTY;

  snprintf(main_time_dest, msize, "%s%s%s%s", m_str, dir_str, hr_str, oclock);
  snprintf(precision_dest, msize, "%s", prec_str);
}

/*
int main(void) {
  int hour, minute = 0;
  char buf[256];
  for (hour = 0; hour < 24; hour++) {
    for (minute = 0; minute < 60; minute++) {
      write_time_buffer(hour, minute, buf, 256);
      printf("%d:%d = %s\n", hour, minute, buf);
    }
  }
  return 0;
}
*/
