#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <windows.h>
#include <shlwapi.h>
#include <limits>
#include <thread>
#include <conio.h>
#include <stdlib.h>
#include <chrono>
#include <cmath>

using namespace std;

string fileName = "Words.txt";
int sentenceCount = 50;
int prevWidth = 0;

bool debug = false;

string test;

vector<string> getWords() {
	string path = __FILE__;
	path = path.substr(0, 1 + path.find_last_of('\\')); //removes file name
	path += fileName;
	vector<string> vec;
	ifstream file_in(path);
	if (!file_in) { cout << "\"" << path << "\" does not exist!" << endl; return vec; }

	string line;
	while (getline(file_in, line)) {
		vec.push_back(line);
	}

	return vec;
}

string getRandom(vector<string> words) {
	return words[rand() % words.size()];
}

vector<string> getSentence(vector<string> words) {
	vector<string> sentence;
	for (int i = 0; i < sentenceCount; i++) {
		int index = rand() % words.size();
		sentence.push_back(words[index]);
	}
	return sentence;
}

string toStrSentence(vector<string> sentence) {
	string str = "";
	for (int i = 0; i < sentence.size(); i++) {
		str += sentence[i];
		if (i != sentence.size() - 1)
			str += ' ';
	}
	return str;
}

int getTerminalWidth() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
		// an error occurred
		cerr << "Cannot determine console size." << endl;
	}
	// cout << "The console is " << csbi.srWindow.Right - csbi.srWindow.Left << " wide." << endl;
	int width = csbi.srWindow.Right - csbi.srWindow.Left;
	return width;
}

void typeRunner(int x, int y, int index, HANDLE hConsole, string strSentence) {
	char c;
	auto start = chrono::high_resolution_clock::now();
	while (1) {
		int width = getTerminalWidth();

		if (width != prevWidth) {
			prevWidth = width;
			x = index;
			y = 0;
			while (x > width) {
				x -= width;
				y++;
			}
		}

		if (x > width) {
			x = 0;
			y++;
		}

		if (index >= strSentence.size()) {
			if (debug) {
				cout << "\nQUITING\n";
				printf("Index: %i Size: %i", index, strSentence.size());
			}
			break;
		}

		COORD pos = { x, y };

		c = _getch();
		if (c == '.') {
			if (debug) {
				cout << "\nQUITING\n";
				printf("C: %c", c);
			}
			break;
		}

		if (c != strSentence.at(index)) {
			continue;
		}

		// cout << "\033[32;40m" << strSentence.at(index) << "\033[0m";
		SetConsoleTextAttribute(hConsole, 2);
		cout << strSentence.at(index);

		SetConsoleCursorPosition(hConsole, pos);

		x++;
		index++;
	}
	auto finish = std::chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = finish - start;
	float wpm = (strSentence.size() / 5) / (elapsed.count() / 60);

	cout << "\n\nYour net WPM was " << wpm << " words per minute!\n";
	return;
}

int main() {
// 	cout << "\x1B[32;40m" << "Testing" << "\033[0m";
	system("cls");
	srand(time(0));
	prevWidth = getTerminalWidth();
	vector<string> words = getWords();
	vector<string> sentence = getSentence(words);
	string strSentence = toStrSentence(sentence);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { 0, 0 };
	string typed = "";

	cout << strSentence << endl;

	SetConsoleCursorPosition(hConsole, pos);

	int x = 1, y = 0, index = 0;

	thread typer(typeRunner, x, y, index, hConsole, strSentence);
	typer.join();
	return 0;
}
