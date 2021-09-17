// Corey Fitzpatrick
//Cryptography Project 2 - Vigenere Cipher
//09/14/2021
//I recommend you full-screen your console as there is sometimes a lot of data that gets written at once.

#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <vector>
#include <iomanip>
#include <map>

//=======================================
//==============CONSTANTS================
//=======================================
const int ALPHABET_LENGTH = 26;
//Letter frequencies from http://cs.wellesley.edu/~fturbak/codman/letterfreq.html
const float ALPHABET_FREQ[ALPHABET_LENGTH] = { .082f, .015f, .028f, .043f, .127f, .022f, .02f, .061f, .07f, .002f, .008f, .04f, .024f, .067f, .075f, .019f, .001f, .06f, .063f, .091f, .028f, .001f, .024f, .002f, .02f, 0.1f };
//scalar is used to map frequencies (floats) to alphabet in an ordered std::map.  Converted floats to int with scalar.
const int scalar = 100000;

//=======================================
//==============PROTOTYPES================
//=======================================

std::string toUpperCase(std::string text);
std::string readFile();
std::string getKeywordFromUser();
std::string guessKeyWord(std::string, int keyLength);
std::string adjustKeyword(std::vector<std::map<int, char>>& freqMap, std::string keyword, std::string ciphertext);
int getUserInput(int selection);
int getKeyLengthFromUser();
void outputToFile(std::string, int selection);
void printToScreen(std::string, int selection);
void printKeywordWithIndex(std::string key);
void showTitle();
float indexOfCoincidence(std::string ciphertext);
float friedmanTest(std::string ciphertext);
float averageIOC(std::string ciphertext);

//=======================================
//==============STRUCTS==================
//=======================================
struct Cipher {
	std::string plaintext;
	std::string ciphertext;

	void encrypt(std::string p_text, std::string keyword) {
		plaintext = "";
		plaintext = toUpperCase(p_text);
		keyword = toUpperCase(keyword);
		int length = plaintext.length();

		//encypt
		for (int i = 0; i < length; i++) {
			ciphertext += (plaintext[i] + keyword[i % keyword.length()]) % 26 + 'A';
		}
	}
	void decrypt(std::string c_text, std::string keyword) {
		ciphertext = "";
		ciphertext = toUpperCase(c_text);
		plaintext = "";
		int length = ciphertext.length();

		//decrypt
		for (int i = 0; i < length; i++) {
			plaintext += (ciphertext[i] - keyword[i % keyword.length()] + 26) % 26 + 'A';
		}
		std::cout << "\nUsing the keyword (" << keyword << "), the first 1/4 characters of your plaintext are:\n";
		int displayLength = ciphertext.length() / 4;
		for (int i = 0; i < displayLength; i++)
			std::cout << plaintext[i];
		std::cout << std::endl << std::endl;
	}
};

//=======================================
//==============MAIN==================
//=======================================

int main() {
	std::cout << std::setprecision(5) << std::fixed;

	showTitle();

	int input;
	int keyLength;
	std::string textFromFile;
	std::string keyword;
	std::string ciphertext;
	Cipher cipher;

	input = getUserInput(0);

	while (input != 3) {
		switch (input) {
			//encrypt
		case 1:
			textFromFile = readFile();
			keyword = getKeywordFromUser();
			cipher.encrypt(textFromFile, keyword);
			input = getUserInput(1); // 1 = print to screen, 2 =  output to file
			if (input == 1) {
				printToScreen(cipher.ciphertext, 2);
			}
			else if (input == 2) {
				outputToFile(cipher.ciphertext, 2);
			}
			break;
			//decrypt
		case 2:
			ciphertext = readFile();
			std::cout << "=============================\nThe Friedman Test estimates the key length is: " << floor(friedmanTest(ciphertext)) << " or " << ceil(friedmanTest(ciphertext)) << std::endl;
			averageIOC(ciphertext);
			keyLength = getKeyLengthFromUser();
			keyword = guessKeyWord(ciphertext, keyLength);
			cipher.decrypt(ciphertext, keyword);
			break;
		}
		//reset for next use
		ciphertext = "";
		keyword = "";
		textFromFile = "";
		input = getUserInput(0);
	}
	return 0;
}

//=======================================
//==============FUNCTIONS==================
//=======================================

std::string toUpperCase(std::string text) {
	std::string formattedText;
	int length = text.length();
	for (int i = 0; i < length; i++) {
		if (isalpha(text[i])) {
			formattedText += toupper(text[i]);
		}
	}

	return formattedText;
}
//==========================================
//==========================================
int getUserInput(int selection) {
	int input;

	if (selection == 0)
		std::cout << "\nEnter 1 to encrypt a plaintext file: \nEnter 2 to decrypt a ciphertext file\nEnter 3 to terminate program\n";
	else if (selection == 1)
		std::cout << "Do you want to print out the ciphertext to screen or output to a file?\nEnter 1 to print to screen\nEnter 2 to output to file";
	else if (selection == 2)
		std::cout << "Do you want to print out the plaintext to screen or output to a file?";

	std::cin >> input;
	std::cin.ignore();
	return input;
}
//==========================================
//==========================================
std::string readFile() {
	std::string text;
	std::ifstream inFile;
	std::string filename;
	std::string line;

	std::cout << "\nEnter filename: \n";
	std::getline(std::cin, filename);

	if (filename.find(".txt") == std::string::npos) {
		filename += ".txt";
	}

	inFile.open(filename);

	if (inFile.fail()) {
		std::cout << "Error opening file.  Exiting Program";
		exit(1);
	}
	if (inFile.is_open()) {
		while (inFile.peek() != EOF) {
			getline(inFile, line);
			text += line;
		}
	}
	inFile.close();
	return toUpperCase(text);
}
//==========================================
//==========================================
std::string getKeywordFromUser() {
	std::string keyword;
	std::cout << "\nEnter keyword: \n";
	std::getline(std::cin, keyword);
	return keyword;
}
//==========================================
//==========================================
void printToScreen(std::string text, int selection) {
	//handle printing plaintext
	if (selection == 1) {
		std::cout << "\n=== YOUR PLAINTEXT ===\n" << text;
	}
	//handle printing ciphertext
	else if (selection == 2) {
		std::cout << "\n=== YOUR CIPHERTEXT ===\n";
		int length = text.length();
		for (int i = 0; i < length; i++) {
			std::cout << text[i];
			if ((i + 1) % 5 == 0) {
				std::cout << " ";
			}
		}
	}
}
//==========================================
//==========================================
void outputToFile(std::string text, int selection) {
	std::ofstream outFile;
	std::string filename;
	int length = text.length();

	std::cout << "\nEnter filename for export:\n";
	std::getline(std::cin, filename);

	if (filename.find(".txt") == std::string::npos) {
		filename += ".txt";
	}
	outFile.open(filename);

	std::cout << "\n=== Dumping to file ===\n";

	//handle writing plaintext to file
	if (selection == 1) {
		outFile << text;
	}
	//handle writing ciphertext to file
	else if (selection == 2) {
		for (int i = 0; i < length; i++) {
			outFile << text[i];
		}
	}
	std::cout << "\n=== File writing successful ===\n\n";
}
//==========================================
//==========================================
float indexOfCoincidence(std::string ciphertext) {
	int frequencies[ALPHABET_LENGTH] = { 0 };
	int  totalCharacters = ciphertext.length();
	int summation = 0;
	float index;

	for (int i = 0; i < totalCharacters; i++) {
		frequencies[ciphertext[i] - 'A']++;
	}

	for (int i = 0; i < ALPHABET_LENGTH; i++) {
		summation = summation + (frequencies[i] * (frequencies[i] - 1));
	}
	index = (float)summation / (totalCharacters * (totalCharacters - 1.0f));
	return index;
}

//==========================================
//==========================================
float friedmanTest(std::string ciphertext) {
	float keyLength;
	float ioc;
	int totalCharacters;

	totalCharacters = ciphertext.length();
	ioc = indexOfCoincidence(ciphertext);
	keyLength = (0.027f * totalCharacters) / ((totalCharacters - 1.0f) * ioc - (0.038f * totalCharacters) + 0.065f);
	return keyLength;
}
//==========================================
//==========================================
void displayFrequencies(float frequencies[]) {
	std::cout << "=== FREQUENCIES OF EACH LETTER IN CIPHERTEXT ===" << std::endl;
	for (int i = 0; i < ALPHABET_LENGTH; i++) {
		std::cout << (char)(i + 'A') << " : " << frequencies[i] << std::endl;
	}
}
//==========================================
//==========================================
float averageIOC(std::string ciphertext) {
	std::vector<float> averages(50);
	int length = ciphertext.length();
	std::string star;
	float maxioc = 0;

	std::cout << "==============================\n"
		<< "Below is the average index of coincidences for key length 1 - 25.\n"
		<< "The likely keylength is the highest average observed near the friedman estimate.\n"
		<< "Values marked with an * are very near the IoC of english frequencies (higher than 0.06) and are more likely to be correct.\n"
		<< "==============================\n";

	for (int i = 1; i < 25; i++) {
		std::string substring = "";
		std::vector<std::string> cosets(i);
		for (int j = 0; j < length; j++) {
			cosets[j % i] += ciphertext[j];
		}
		float sum = 0;
		int cosetSize = cosets.size();
		for (int z = 0; z < cosetSize; z++) {
			sum += indexOfCoincidence(cosets[z]);
		}
		averages[i] = sum / (float)i;
		if (averages[i] > 0.06)
			star = '*';
		else star = "";

		std::cout << "keylength: " << i << " average ioc: " << averages[i] << " " << star << std::endl;
	}
	return 1;
}
//==========================================
//==========================================
int getKeyLengthFromUser() {
	int keyLength;

	std::cout << "\nEnter the keylength based on the information above: \n";
	std::cin >> keyLength;
	std::cin.ignore();

	return keyLength;
}
//==========================================
//==========================================
std::string guessKeyWord(std::string ciphertext, int keyLength) {
	std::vector<std::string> substrings(keyLength, "");
	std::vector<std::vector<float>> frequencies(keyLength, std::vector<float>(ALPHABET_LENGTH, 0.0));
	int length = ciphertext.length();

	//splits ciphertext in keyLength number of subtrings, each beginning at the keyLength[i] position
	for (int i = 0; i < length; i++) {
		substrings[i % keyLength] += ciphertext[i];
	}

	// gets frequency letter counts for each substring
	for (int i = 0; i < keyLength; i++) {
		float letterCount[ALPHABET_LENGTH] = { 0 };
		std::cout << "===THE FREQUENCIES FOR THE " << i << "TH CHARACTER OF THE KEYWORD ARE: ===\n";
		for (int k = 0; k < substrings.at(i).length(); k++) {
			letterCount[substrings.at(i)[k] - 'A']++;
		}
		for (int j = 0; j < ALPHABET_LENGTH; j++) {
			frequencies[i][j] = letterCount[j] / substrings[i].length();
			std::cout << (char)(j + 'A') << ": " << frequencies[i][j] << "\n";
		}
		std::cout << "========================\n";
	}

	//begin shifting characters in each substring, we find the most likely number of shifts by multiplying the frequencies of each letter found in the substring by the actual english letter
	//frequencies.  The shift with the largest sum should tell us what the actual character should be in the key.
	//for example, MAX(∑k=0 to 26 (k[i] * α[i])) , whichever k index is highest, will tell us the number of shifts it to match our ciphertext with true alphabet

	std::vector < std::map<int, char>> freqMaps(keyLength);
	std::string keyword = "";
	for (int i = 0; i < keyLength; i++) {
		int numberOfShifts = 0;
		float max = 0;
		for (int shift = 0; shift < ALPHABET_LENGTH; shift++) {
			float sum = 0;
			for (int j = 1; j < ALPHABET_LENGTH; j++) {
				sum += frequencies[i][(j + shift) % 26] * ALPHABET_FREQ[j];
			}
			//inserts summations into map, with floats converted to ints by scalar.  This is used to rank-order the most likely characters for each index of keyword.
			freqMaps[i].insert(std::pair<int, char>(sum * scalar, shift + 'A'));
			if (max < sum) {
				max = sum;
				numberOfShifts = shift;
			}
		}
		keyword += (numberOfShifts + 'A');
		std::cout << "Number of shifts for keyword index " << i << " is: " << numberOfShifts;
		std::cout << "\nkeyword: " << keyword << std::endl;
	}
	std::cout << "==============================\n"
		<< "==============================\n"
		<< "Press <Enter> to display the first several lines of the plaintext using the most likely keyword.";
	std::cin.get();

	Cipher cipher;
	cipher.decrypt(ciphertext, keyword);

	keyword = adjustKeyword(freqMaps, keyword, ciphertext);

	return keyword;
}
//==========================================
//==========================================
std::string adjustKeyword(std::vector<std::map<int, char>>& freqMap, std::string keyword, std::string ciphertext) {
	std::string input;
	int index;
	char newCharacter;

	std::cout << "\nI estimated the keyword to be: " << keyword << ". Is this keyword correct?\nEnter Y for yes.  Enter N for no.\n";
	std::getline(std::cin, input);

	while (tolower(input[0]) != 'y') {
		std::cout << "\nEnter the index of the position (zero indexed) of the keyword that is incorrect:\n";
		printKeywordWithIndex(keyword);
		std::cin >> index;
		std::cin.ignore();

		std::cout << "The most likely characters (listed from most likely to least likely) for the index you selected are as follows: \n";
		std::cout << "(The numbers represent the summation of the product of the frequencies found in the text and the frequencies of english alphabet) \n";

		int i = 0;
		for (std::map<int, char>::reverse_iterator it = freqMap[index].rbegin(); it != freqMap[index].rend(); ++it) {
			std::cout << it->first << " => " << it->second << '\n';
			i++;
		}

		std::cout << "Enter the replacement character for index: " << index << ".\n";
		std::cin >> newCharacter;
		newCharacter = toupper(newCharacter);
		std::cin.ignore();
		keyword[index] = newCharacter;

		Cipher cipher;
		cipher.decrypt(ciphertext, keyword);

		std::cout << "\nI estimated the keyword to be: " << keyword << ". Is this keyword correct?\nEnter Y for yes.  Enter N for no.\n";
		std::getline(std::cin, input);
	}

	return keyword;
}
//==========================================
//==========================================
void printKeywordWithIndex(std::string key) {
	std::cout << std::endl << std::endl;
	for (int i = 0; i < key.length(); i++) {
		std::cout << key[i] << " ";
	}
	std::cout << std::endl;
	for (int i = 0; i < key.length(); i++) {
		std::cout << i << " ";
	}
	std::cout << std::endl << std::endl;
}
//==========================================
//==========================================
void showTitle() {
	for (int i = 0; i < 60; i++) {
		std::cout << (char)205;
	}
	std::cout << std::endl;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 61; j++) {
			if (j % 60 == 0)
				std::cout << (char)124;
			else if (j == 15) {
				std::cout << "====== Vigenere Cipher =======";
				j += 30;
			}
			else
				std::cout << " ";
		}
		std::cout << std::endl;
	}
	for (int i = 0; i < 60; i++) {
		std::cout << (char)205;
	}
}