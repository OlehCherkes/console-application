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
int bit; // Амплитуда
const double pi = 3.14159;
double freq; // Частота
int sample_rate; // Частота дискретизации
double fs;

// Синусоидный сигнал
double MakeWave(double dTime) {
		return bit * sin(freq * 2 * pi * dTime);
}

// Треугольный сигнал
double MakeTriangle(double dTime) {
		return bit * (2 / 3.14) * asin(sin(2 * pi * freq * dTime));
}

// Дизеринг (Белый шум)
double MakeDither(double dTime) {
	   return 2 * (((rand()/ ((double)RAND_MAX)) - bit ));
}

int main() {
	setlocale(LC_ALL, "Russian");
	
	//Link
	cout << "*****/Генератор сигнала/*****\n" << endl;
	cout << "Нажмите 1 для выбора синусоидного сигнала:\n";
	cout << "Нажмите 2 для выбора трехугольного сигнала:\n";
	cout << "Нажмите 3 для дизеринга:\n";
	cin >> choice;
	system("cls");
	cout << "*****/Параматры сигнала/*****\n" << endl;
	cout << "Введите частоту сигнала (Hz): ";
	cin >> freq;
	cout << "Введите амплитуду сигнала: ";
	cin >> bit;
	if (bit <= 0)
	{
		cout << "Значение должно быть больше 0!";
	}
	cout << "Введите частоту дискретизации: ";
	cin >> sample_rate;

	vector<wstring> devices = olcNoiseMaker<short>::Enumerate();

	for (auto d : devices) wcout << endl << d << endl; //Вывод драйвера
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
	////////////////////////// Сохранение параметров сигнала в CSV /////////////////////////
	char save_csv;
	cout << "Сохранить в CSV? [y/n]: ";
	cin >> save_csv;

	if (save_csv == 'y')
	{
		// Запись значений в файл csv
		ofstream ocsv; //создаем объект класа ofstream
		ocsv.open("My_csv.csv"); // открытие фала
		ocsv << freq << ", " << bit << ", " << sample_rate << ", " <<endl; //запись значений в файл csv
		ocsv.close(); //закрытие файла
		cout << "Файл успешно сохранен!\n" << endl;
	}
	if (save_csv == 'n')
	{

	}

	////////////////////////// Сохранение параметров сигнала в Wav /////////////////////////
	char save_wav;
	cout << "Сохранить в WAV? [y/n]: ";
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
		cout << "Файл успешно сохранен!\n" << endl;
	}

	if (save_wav == 'n')
	{

	}
	

	
	////////////////////////// Закрыть приложение /////////////////////////
	char ex;
	cout << "\nВыйти? [y/n]: ";
	cin >> ex;
	if (ex == 'y')
	{
		exit(0);
	}

	while (1) {

	}
	return 0;
}

