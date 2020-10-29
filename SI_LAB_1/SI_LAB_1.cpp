#include <iostream>
#include <bitset>
#include <ctime>       
#include <iomanip> 
#include "Massives.h"
#include <sstream>

using namespace std;

string TextToBinaryString(string);
string KeyGeneration();

void BinaryToText(string dec)
{
	stringstream sstream(dec);
	string output;
	while (sstream.good())
	{
		bitset<8> bits;
		sstream >> bits;
		char c = char(bits.to_ulong());
		output += c;
	}
	cout << endl << "Исходный текст: " << output;
}

string Extention_48_bit(string tmp, int expansionDBox[])
{
	string extension;
	for (int i = 0; i < 48; i++)
	{
		extension += (tmp[(int64_t)expansionDBox[i] - 1]);
	}
	return extension;
}

string TextToBinaryString(string words)
{
	string binaryString = "";
	for (char& _char : words)
	{
		binaryString += bitset<8>(_char).to_string();
	}
	while (binaryString.length() < 64)
	{
		binaryString = "0" + binaryString;
	}
	return binaryString;
}

string Permutation(int arr[], string textBIN, int n)
{
	string per;
	for (int i = 0; i < n; i++)
	{
		per += (textBIN[(int64_t)arr[i] - 1]);
	}
	return per;
}

string KeyGeneration()
{
	string key;
	srand((unsigned int)time(NULL));
	for (int i = 0; i < 56; i++)
	{
		int random = rand() % 10 + 1;
		if (random > 5) key += "1";
		else key += "0";
	}
	return key;
}

string Generate_Ki(string key1, int PC1[], int shiftBits[])
{
	string keys;
	string key2;
	int counter = 0;
	string tmpary[8];
	int n = 0, f = 0;
	while (f < 56)
	{
		key2 += key1.substr(f, 7);
		for (int i = f; i < f + 7; i++)
		{
			if (key1[i] == '1') n++;
		}
		if (n % 2 == 0) key2 += "1";
		else key2 += '0';
		f += 7;
		n = 0;
		counter++;
	}
	string Ki = Permutation(PC1, key2, 64);
	string Ci = Ki.substr(0, 32);
	string Di = Ki.substr(32, 32);
	string cShift;
	string dShift;
	for (int global = 0; global < 16; global++)
	{
		if (shiftBits[global] == 1)
		{
			cShift = Ci.substr(1, 27) + Ci.substr(0, 1);
		}
		else
		{
			cShift = Ci.substr(2, 26) + Ci.substr(0, 2);
		}
		if (shiftBits[global] == 1)
		{
			dShift = Di.substr(1, 27) + Di.substr(0, 1);
		}
		else
		{
			dShift = Di.substr(2, 26) + Di.substr(0, 2);
		}
		string CDi = cShift + dShift;
		string Kii = Permutation(PC2, CDi, 48);
		keys = keys + Kii;
		Ci = cShift;
		Di = dShift;
	}
	return keys;
}

string Encrypt(string key1, string text)
{
	string keys = Generate_Ki(key1, PC1, shiftBits);
	string ip1 = Permutation(initialPermutation, text, 64);
	string left = ip1.substr(0, 32);	
	string right = ip1.substr(32);
	for (int d = 0; d < 16; d++)
	{
		string tmp = right;
		tmp = Extention_48_bit(tmp, expansionDBox);
		string KEY[16];
		int i = 0;
		while (i < sizeof(keys))
		{
			for (int j = 0; j < 16; j++)
			{
				KEY[j] = keys.substr(i, 48);
				i += 48;
			}
		}
		string xorSum[48];
		for (int j = 0; j < 48; j++)
		{
			if (tmp[j] == KEY[d][j]) xorSum[j] = "0";
			else xorSum[j] = "1";
		}	
		string bb;
		for (int i = 0; i < 48; i += 6)
		{
			string temp[6] = { xorSum[i],xorSum[i + 1],xorSum[i + 2],xorSum[i + 3],xorSum[i + 4],xorSum[i + 5] };
			int num = i / 6;
			int row = bitset<32>(temp[0] + temp[5]).to_ulong();
			int col = bitset<32>(temp[1] + temp[2] + temp[3] + temp[4]).to_ulong();
			string binary = bitset<4>(sbox[num][row][col]).to_string();
			bb = bb + binary;
		}
		tmp = Permutation(straightPermutation, bb, 32);
		string fin[32];
		string ffffffin;
		for (int i = 0; i < 32; i++)
		{
			if (tmp[i] == left[i]) fin[i] = '0';
			else fin[i] = '1';
		}
		for (int i = 0; i < 32; i++)
		{
			ffffffin += fin[i];
		}	
		left = right;
		right = ffffffin;
		text = left + right;
	}
	text = Permutation(reverseInitialPermutation, text, 64);
	return text;
}

string Decrypt(string key1, string text)
{
	string keys = Generate_Ki(key1, PC1, shiftBits);
	string ip1 = Permutation(initialPermutation, text, 64);
	string left = ip1.substr(0, 32);
	string right = ip1.substr(32);
	for (int d = 15; d >=0; d--)
	{		
		string tmp = left;
		tmp = Extention_48_bit(tmp, expansionDBox);
		string KEY[16];
		int i = 0;
		while (i < sizeof(keys))
		{
			for (int j = 0; j < 16; j++)
			{
				KEY[j] = keys.substr(i, 48);
				i += 48;
			}
		}
		string xorSum[48];
		for (int j = 0; j < 48; j++)
		{
			if (tmp[j] == KEY[d][j]) xorSum[j] = "0";
			else xorSum[j] = "1";
		}
		string bb;
		for (int i = 0; i < 48; i += 6)
		{
			string temp[6] = { xorSum[i],xorSum[i + 1],xorSum[i + 2],xorSum[i + 3],xorSum[i + 4],xorSum[i + 5] };
			int num = i / 6;
			int row = bitset<32>(temp[0] + temp[5]).to_ulong();
			int col = bitset<32>(temp[1] + temp[2] + temp[3] + temp[4]).to_ulong();
			string binary = bitset<4>(sbox[num][row][col]).to_string();
			bb = bb + binary;
		}
		tmp = Permutation(straightPermutation, bb, 32);
		string fin[32];
		string ffffffin;
		for (int i = 0; i < 32; i++)
		{
			if (tmp[i] == right[i]) fin[i] = '0';
			else fin[i] = '1';
		}
		for (int i = 0; i < 32; i++)
		{
			ffffffin += fin[i];
		}		
		right = left;
		left = ffffffin;
		text = left + right;
	}	
	text = Permutation(reverseInitialPermutation, text, 64);
	return text;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	string text;
	cout << "Введите текст: ";
	cin >> text;
	string textBIN = TextToBinaryString(text);
	cout << fixed << left << setw(25) << "Текст в формате BIN: " << textBIN << endl;
	string key1 = KeyGeneration();
	string enc = Encrypt(key1, textBIN);
	string dec = Decrypt(key1, enc);
	cout << fixed << left << setw(25) << "Зашифрованный текст: " << enc << endl;
	cout << fixed << left << setw(25) << "Расшифрованный текст: " << dec;
	BinaryToText(dec);
}