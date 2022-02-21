#include <string>

#include "format.h"

using std::string;

// helper function to convert a Long int measuring seconds
// into output format HH:MM:SS as string
string Format::ElapsedTime(long seconds) { 
  string hstr, mstr, sstr = "";
    long hour = seconds/3600;
    if (hour<10){hstr = "0" + std::to_string(hour);}
    else {hstr = std::to_string(hour);}
    long rest = seconds - hour*3600;
    long minute = rest/60;
    if (minute<10){mstr = "0" + std::to_string(minute);}
    else {mstr = std::to_string(minute);}
    long second = seconds - hour*3600 - minute*60;
    if (second<10){sstr = "0" + std::to_string(second);}
    else {sstr = std::to_string(second);}
    string result = hstr+":"+mstr+":"+sstr;
    return result;}