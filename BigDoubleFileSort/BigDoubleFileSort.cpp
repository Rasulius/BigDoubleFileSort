

#include <tchar.h>
#include <iostream>    
#include <fstream>
#include <limits> 
#include <chrono>
#include <iterator>    
#include <algorithm> 
#include <string>    
#include <vector>   
#include <cstdio>
#define TOTAL_MEMORY_USAGE 90000000 / sizeof(double) // ~ 90 МБ



using namespace std; 

typedef chrono::high_resolution_clock programWorkClock;


/*Функция обьеденияет части*/ 

bool sort(const string &firstFileName, const string &secondFileName, const string &result){

	ifstream firstStream(firstFileName);
	ifstream secondStream(secondFileName);

	if (!firstStream.good() || !secondStream.good())
		return false;

	ofstream destinationFile(result);
	double firstValue, secondValue;

	firstStream.read(reinterpret_cast<char*>(&firstValue), sizeof(firstValue));
	secondStream.read(reinterpret_cast<char*>(&secondValue), sizeof(secondValue));

	while (firstStream.good() || secondStream.good()) {

		if (firstValue <= secondValue) {
			destinationFile.write(reinterpret_cast<char*>(&firstValue), sizeof(firstValue));
			firstStream.read(reinterpret_cast<char*>(&firstValue), sizeof(firstValue));
		}
		else{
			destinationFile.write(reinterpret_cast<char*>(&secondValue), sizeof(secondValue));
			secondStream.read(reinterpret_cast<char*>(&secondValue), sizeof(secondValue));
		}

		if (!firstStream.good()){
			while (secondStream.good()) {
				secondStream.read(reinterpret_cast<char*>(&secondValue), sizeof(secondValue));
				destinationFile.write(reinterpret_cast<char*>(&secondValue), sizeof(secondValue));
			}
			break;
		}

		if (!secondStream.good()){
			while (firstStream.good()) {
				firstStream.read(reinterpret_cast<char*>(&firstValue), sizeof(firstValue));
				destinationFile.write(reinterpret_cast<char*>(&firstValue), sizeof(firstValue));
			}
		}
	}
	destinationFile.close();  // Закрываем файлы и удаляем временные файлы
	firstStream.close();
	secondStream.close();
	remove(firstFileName.c_str());
	remove(secondFileName.c_str());

	return true;
}

bool checkFileFormat(int argc){
	if (argc < 3){
		cout << "please use format: unsorted_filename.txt sorted_filename.txt ";
		return false;
	}
	return true;
}
 
// Сортировка файла, который является частью файла. 

void sortParts(ifstream &unsortedFile, unsigned int &partNumber, programWorkClock::time_point end, programWorkClock::time_point duration){
	std::string EXTENSION = ".tmp";
	vector<double> buffer;

	while (unsortedFile.good()) {
		string line, tempFile;

		for (size_t i = 0; i < TOTAL_MEMORY_USAGE; i++) {
			getline(unsortedFile, line);

			if (!line.empty())
				buffer.push_back(std::stod(line));

			if (!unsortedFile.good()) break;
		}


		std::sort(buffer.begin(), buffer.end());

		tempFile = "part_" + to_string(partNumber) + EXTENSION;
		std::ofstream sorted(tempFile);

		for (auto &i : buffer) {
			sorted.write(reinterpret_cast<char*>(&i), sizeof(i));
		}
		sorted.close();

		duration = programWorkClock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(duration - end);
		end = duration;

		std::cout << "sorting time" << partNumber << " is: " << elapsed_time.count() / 60 << " minutes " << elapsed_time.count() % 60 << " seconds\n";

		partNumber++;
		buffer.clear();

	}
}


int _tmain(int argc, _TCHAR* argv[]){
	
	std::ios::sync_with_stdio(false);
	std::string EXTENSION = ".tmp";

	if (!checkFileFormat(argc))
		exit(0);

	
	unsigned int partNumber = 0;
	unsigned int sortNumber = 0;
	
	ifstream unsortedFile(argv[1]);

	
	programWorkClock::time_point end, duration;	
	auto begin = end = programWorkClock::now();

	// Запускаем сортировку частей
	sortParts(unsortedFile, partNumber, end, duration);
	unsortedFile.close();

	auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(end - begin);
	std::cout << "Parts sorted:" << elapsed_time.count() / 60 << " minutes " << elapsed_time.count() % 60 << " seconds\n"
		<<"\n"<<"";
	

	for (; sortNumber < partNumber; sortNumber += 2, partNumber++) {

		if (!sort("sorted_" + to_string(sortNumber) + EXTENSION,
			"sorted_" + to_string(sortNumber + 1) + EXTENSION,
			"sorted_" + to_string(partNumber) + EXTENSION))
			break;	
	}


	double d_value = 0.0;
	string sorted_bin_file = "bin_sort_" + std::to_string(sortNumber) + EXTENSION;
	ifstream read_bin_file(sorted_bin_file);
	ofstream sorted_file(argv[2]);

	read_bin_file.read(reinterpret_cast<char*>(&d_value), sizeof(d_value));
	while (read_bin_file.good()) {
		sorted_file << d_value << '\n';
		read_bin_file.read(reinterpret_cast<char*>(&d_value), sizeof(d_value));
	}
	read_bin_file.close();
	sorted_file.close();

	remove(sorted_bin_file.c_str());

	end = programWorkClock::now();
	elapsed_time =  chrono::duration_cast<chrono::seconds>(end - begin);
	std::cout << "Sort time: " << elapsed_time.count() / 60 << " minutes " << elapsed_time.count() % 60 << " seconds\n";

	exit(EXIT_SUCCESS);
}

