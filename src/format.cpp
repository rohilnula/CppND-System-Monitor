#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    string hours_str;
	string min_str;
    string sec_str;

    sec_str = std::to_string(seconds%60);
	min_str = std::to_string((seconds/60)%60);
	hours_str = std::to_string(seconds/3600);

	return hours_str.append(":").append(min_str).append(":").append(sec_str);
}