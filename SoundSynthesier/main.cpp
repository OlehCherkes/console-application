#include <iostream>
#include "olcNoiseMaker.h"
#include <cmath>
#include <fstream>
#include <cstdlib>

using namespace std;

namespace little_endian_io
{
	template <typename Word>
	std::ostream& write_word(std::ostream& outs, Word value, unsigned size = sizeof(Word))
	{
		for (; size; --size, value >>= 8)
			outs.put(static_cast <char> (value & 0xFF));
		return outs;
	}
}

using namespace little_endian_io;
int choice;
int bit; // ���������
const double pi = 3.14159;
double freq; // �������
int sample_rate; // ������� �������������
double fs;

// ����������� ������
double MakeWave(double dTime) {
		return bit * sin(freq * 2 * pi * dTime);
}

// ����������� ������
double MakeTriangle(double dTime) {
		return bit * (2 / 3.14) * asin(sin(2 * pi * freq * dTime));
}

// �������� (����� ���)
double MakeDither(double dTime) {
	   return 2 * (((rand()/ ((double)RAND_MAX)) - bit ));
}

int main() {
	setlocale(LC_ALL, "Russian");
	
	//Link
	cout << "*****/��������� �������/*****\n" << endl;
	cout << "������� 1 ��� ������ ������������ �������:\n";
	cout << "������� 2 ��� ������ ������������� �������:\n";
	cout << "������� 3 ��� ���������:\n";
	cin >> choice;
	system("cls");
	cout << "*****/��������� �������/*****\n" << endl;
	cout << "������� ������� ������� (Hz): ";
	cin >> freq;
	cout << "������� ��������� �������: ";
	cin >> bit;
	if (bit <= 0)
	{
		cout << "�������� ������ ���� ������ 0!";
	}
	cout << "������� ������� �������������: ";
	cin >> sample_rate;

	vector<wstring> devices = olcNoiseMaker<short>::Enumerate();

	for (auto d : devices) wcout << endl << d << endl; //����� ��������
	olcNoiseMaker<short> sound(devices[0], sample_rate, 1, 8, 512);

	switch (choice){
	case 1: {
		sound.SetUserFunction(MakeWave);
		break;
		}
	case 2: {
		sound.SetUserFunction(MakeTriangle);
		break;
	        }
	case 3: {
		sound.SetUserFunction(MakeDither);
		break;
	}
	}
	////////////////////////// ���������� ���������� ������� � CSV /////////////////////////
	char save_csv;
	cout << "��������� � CSV? [y/n]: ";
	cin >> save_csv;

	if (save_csv == 'y')
	{
		// ������ �������� � ���� csv
		ofstream ocsv; //������� ������ ����� ofstream
		ocsv.open("My_csv.csv"); // �������� ����
		ocsv << freq << ", " << bit << ", " << sample_rate << ", " <<endl; //������ �������� � ���� csv
		ocsv.close(); //�������� �����
		cout << "���� ������� ��������!\n" << endl;
	}
	if (save_csv == 'n')
	{

	}

	////////////////////////// ���������� ���������� ������� � Wav /////////////////////////
	char save_wav;
	cout << "��������� � WAV? [y/n]: ";
	cin >> save_wav;
	if (save_wav == 'y') {
		ofstream f("example.wav", ios::binary);

		// Write the file headers
		f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
		write_word(f, 16, 4);  // no extension data
		write_word(f, 1, 2);  // PCM - integer samples
		write_word(f, 2, 2);  // two channels (stereo file)
		write_word(f, sample_rate, 4);  // samples per second (Hz)
		write_word(f, 176400, 4);  // (Sample Rate * BitsPerSample * Channels) / 8
		write_word(f, 4, 2);  // data block size (size of two integer samples, one for each channel, in bytes)
		write_word(f, 16, 2);  // number of bits per sample (use a multiple of 8)

		// Write the data chunk header
		size_t data_chunk_pos = f.tellp();
		f << "data----";  // (chunk size to be filled in later)

		// Write the audio samples
		// (We'll generate a single C4 note with a sine wave, fading from left to right)
		constexpr double two_pi = 6.283185307179586476925286766559;
		constexpr double max_amplitude = 32760;  // "volume"

		double hz = sample_rate;    // samples per second
		double frequency = freq;  // middle C
		double seconds = 10;      // time

		int N = hz * seconds;  // total number of samples
		for (int n = 0; n < N; n++)
		{
			double amplitude = (double)n / N * max_amplitude;

			switch (choice) {
			case 1: {
				double value = bit * sin((freq * 2 * pi * n) * hz);
				write_word(f, (int)(amplitude * value), 2);
				write_word(f, (int)((max_amplitude - amplitude) * value), 2);
				break;
			    }
			case 2: {
				double value = bit * (2 / 3.14) * asin(sin(2 * pi * freq * n) * hz);
				write_word(f, (int)(amplitude * value), 2);
				write_word(f, (int)((max_amplitude - amplitude) * value), 2);
				break;
			}
			case 3: {
				double value = 2 * (((rand() / ((double)RAND_MAX)) - bit) * n) * hz;
				write_word(f, (int)(amplitude * value), 2);
				write_word(f, (int)((max_amplitude - amplitude) * value), 2);
				break;
			}
			}  
		}

		// (We'll need the final file size to fix the chunk sizes above)
		size_t file_length = f.tellp();

		// Fix the data chunk header to contain the data size
		f.seekp(data_chunk_pos + 4);
		write_word(f, file_length - data_chunk_pos + 8);

		// Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
		f.seekp(0 + 4);
		write_word(f, file_length - 8, 4);
		cout << "���� ������� ��������!\n" << endl;
	}

	if (save_wav == 'n')
	{

	}
	

	
	////////////////////////// ������� ���������� /////////////////////////
	char ex;
	cout << "\n�����? [y/n]: ";
	cin >> ex;
	if (ex == 'y')
	{
		exit(0);
	}

	while (1) {

	}
	return 0;
}

