#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

#define HOURS_IN_MILLIS (60 * 60 * 1000)
#define MINUTES_IN_MILLIS (60 * 1000)
#define SECONDS_IN_MILLIS (1000)

unsigned int convertToMillis(string time) {
	//format
	//00:00:00,000
	unsigned int millis = 0;
	millis += HOURS_IN_MILLIS * std::stoul(time.substr(0,2), nullptr); // HOURS
	millis += MINUTES_IN_MILLIS * std::stoul(time.substr(3, 2), nullptr); // MINUTES
	millis += SECONDS_IN_MILLIS * std::stoul(time.substr(6, 2), nullptr); // SECONDS
	millis += std::stoul(time.substr(9), nullptr); // MILLIS
	return millis;
}

string convertToTimeString(unsigned int millis) {
	unsigned int hours = millis / HOURS_IN_MILLIS;
	millis -= HOURS_IN_MILLIS * hours;
	unsigned int minutes = millis / MINUTES_IN_MILLIS;
	millis -= MINUTES_IN_MILLIS * minutes;
	unsigned int seconds = millis / SECONDS_IN_MILLIS;
	millis -= SECONDS_IN_MILLIS * seconds;

	char buff[16];
	buff[15] = '\0';
	snprintf(buff, sizeof(buff) - 1, "%02u:%02u:%02u,%03u", hours, minutes, seconds, millis);
	return buff;
}

string syncLine(string line, unsigned int sync) {
	int index;
	if ((index = line.find(" --> ")) == -1)
		return 0;
	string first = line.substr(0, index);
	string second = line.substr(index + 5);
	if (first.length() != 12 || second.length() != 12)
		return 0;
	unsigned int from = convertToMillis(first) + sync;
	unsigned int to = convertToMillis(second) + sync;
	return convertToTimeString(from) + " --> " + convertToTimeString(to);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "Usage [.SRT_FILE] (OPTIONAL)[milliseconds e.g. 1000 or -1000]" << endl;
		getchar();
		return 0;
	}
	string path = (argv[1]);
	string dst = (argv[1]);
	transform(dst.begin(), dst.end(), dst.begin(), ::tolower);
	if (dst.find(".srt") != dst.length() - 4) {
		cout << "The input files needs to end with .srt" << endl;
		getchar();
		return 0;
	}
	dst = dst.substr(0, dst.length() - 4) + "_synced.srt";
	unsigned int millis;
	if (argc > 2) {
		millis = std::stoul(argv[2], nullptr);
	}
	else {
		cout << "Input the amount to synchronize in milliseconds (e.g. 1000 or -1000): ";
		cin >> millis;
	}
	string line;
	ifstream srtFile(path);
	ofstream synced(dst);
	bool lastEmpty = true;
	bool error = false;
	if (srtFile.is_open() && synced.is_open())
	{
		while (getline(srtFile, line)) {
			if (lastEmpty) {
				synced << line << "\n";
				//Read the time line
				if (getline(srtFile, line)) {
					line = syncLine(line, millis);
				} else {
					//Failed to read the time line
					error = true;
					break;
				}
				lastEmpty = false;
			}
			else {
				if (line.length() == 0)
					lastEmpty = true;
			}
			synced << line << "\n";
		}
		synced.close();
		srtFile.close();
		if (error) {
			cout << "An error occurred during the process!" << endl;
		}
		else {
			cout << "Completed!" << endl;
		}
	} else{
		cout << "Failed to open the required files!" << endl;
	}
	getchar();
    return 0;
}

