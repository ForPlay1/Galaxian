
#include <iostream>
#include "sqlite3.h"
#include <cmath>
#include <vector>

using namespace std;

// g++ -c main.cpp -I"G:\SFML-2.6.2\include"
// g++ main.o -o main -L"G:\SFML-2.6.2\lib" -lsfml-graphics -lsfml-window -lsfml-system
// ./main

int callbackVectorPair(void* Used, int argc, char** argv, char** azColName)
{
	std::vector<std::pair<std::string, std::string>>* data = (std::vector< std::pair<std::string, std::string>>*)Used;
	for (int i = 0; i < argc; i++)
		data->emplace_back(azColName[i], argv[i]);
	return 0;
}

int main()
{
    // sqlite3* DB;
	// std::vector<std::pair<std::string , std::string>> data;
	// char* messageError;
	// std::string sql = "SELECT * FROM Scores;";
	
	// data.clear();
	// int exit = sqlite3_open("data.db", &DB);
	
	// exit = sqlite3_exec(DB, sql.c_str(), callbackVectorPair, &data, &messageError);

	// if (exit != SQLITE_OK) {
	// 	std::cerr << "Error selecting data: " << messageError << std::endl;
	// 	sqlite3_free(messageError);
	// }
	// else
	// 	std::cout << "Data selected successfully!" << std::endl;

	// sqlite3_close(DB);
	// for(auto i: data){
	// 	cout << i.first << " ";
	// 	cout << i.second << endl;
	// }

	// sqlite3* DB;
	// std::vector<std::pair<std::string , std::string>> data;
	// char* messageError;
	// std::string sql = "SELECT MAX(score) FROM Scores;";
	
	// data.clear();
	// int exit = sqlite3_open("data.db", &DB);
	
	// exit = sqlite3_exec(DB, sql.c_str(), callbackVectorPair, &data, &messageError);

	// if (exit != SQLITE_OK) {
	// 	std::cerr << "Error selecting data: " << messageError << std::endl;
	// 	sqlite3_free(messageError);
	// }
	// else
	// 	std::cout << "Data selected successfully!" << std::endl;

	// sqlite3_close(DB);
	// cout << data[0].second;

	// вставить значения
	// sqlite3* DB;
	// char* messageError;

	// std::string sql = "INSERT INTO Scores VALUES(0, 5000, 'Close', 5);";

	// int exit = sqlite3_open("data.db", &DB);

	// exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
	// if (exit != SQLITE_OK) {
	// 	std::cerr << "Error inserting records: " << messageError << std::endl;
	// 	sqlite3_free(messageError);
	// }
	// else
	// 	std::cout << "Data inserted successfully" << std::endl;

	// sqlite3_close(DB);



	// Удалить значения
	// sqlite3* DB;
	// char* messageError;

	// std::string sql = "DELETE FROM Scores WHERE id=2;";

	// int exit = sqlite3_open("data.db", &DB);

	// exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
	// if (exit != SQLITE_OK)
	// {
	// 	std::cerr << "Error deleting records: " << messageError << std::endl;
	// 	sqlite3_free(messageError);
	// }
	// else
	// 	std::cout << "Data deleted successfully" << std::endl;

	// sqlite3_close(DB);


	// sqlite3 *DB;
	// char* messageError;

	// try
	// {
	// 	int exit = 0;
	// 	exit = sqlite3_open("data.db", &DB);
	// 	std::string sql = "CREATE TABLE Scores(id INTEGER, score INTEGER, howquit TEXT, levels INTEGER);";

	// 	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
	// 	if (exit != SQLITE_OK)
	// 	{
	// 		std::cerr << "Error creating table: " << messageError << std::endl;
	// 		sqlite3_free(messageError);
	// 	}
	// 	else
	// 		std::cout << "Table created successfully" << std::endl;
	// 	sqlite3_close(DB);
	// }
	// catch (const std::exception& e)
	// {
	// 	std::cerr << e.what();
	// }

}