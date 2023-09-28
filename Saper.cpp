#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <ctime>

// field: 0-8 - количество мин вокруг, -1 - мина в клетке, -2 - попадание в клетку с миной
// shown_or_hidden: 0 - скрыта, 1 - открыта, 2 - флаг
// check: 1 - продолжается, 0 - конец

bool check(std::vector<std::vector<int>>& shown_or_hidden, const int& length) {
	for (int i = 1; i < shown_or_hidden.size(); i++) {
		for (int j = 1; j < shown_or_hidden[i].size(); j++) {
			if (!shown_or_hidden[i][j]) {
				return 1;
			}
		}
	}
	return 0;
}

std::string alphabet = " ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz"; // вспомогательный алфавит для перевода буквенных значений в числовые
const int moves_x[8]{ -1, -1, -1, 0, 0, 1, 1, 1 },
	moves_y[8]{ -1, 0, 1, -1, 1, -1, 0, 1 }; // сдвиги по осям x и y соответственно

void build(std::vector<std::vector<int>>& field, std::vector<std::vector<int>>& shown_or_hidden, const int& len_y, const int& len_x) {
	system("cls"); // очистка консоли для удаления прошлого поля
	std::cout << "   ";
	for (int i = 1; i <= len_y; i++) {
		std::cout << alphabet[i] << ' ';
	}
	std::cout << std::endl;
	for (int x = 1; x <= len_x; x++) {
		if (x < 10) {
			std::cout << ' ' << x << ' ';
		}
		else {
			std::cout << x << ' ';
		}
		for (int y = 1; y <= len_y; y++) {
			if (shown_or_hidden[x][y] == 1) {
				if (field[x][y] == -1) {
					std::cout << "x ";
				}
				else if (field[x][y] == -2) {
					std::cout << "X ";
				}
				else if (field[x][y]) {
					std::cout << field[x][y] << ' ';
				}
				else {
					std::cout << "  ";
				}
			}
			else if (shown_or_hidden[x][y] == 2) {
				std::cout << "f ";
			}
			else {
				std::cout << ". ";
			}
		}
		std::cout << std::endl;
	}
}

int move_conv(std::string& move) { // перевод строкового значения числа в целочисленный ( "1234" -> 1234 )
	const std::string s = "0123456789";
	int n = 0;
	for (auto c : move) {
		if ('0' <= c and c <= '9') {
			n *= 10;
			n += s.find(c);
		}
	}
	return n;
}

void fill_with_ones(std::vector<std::vector<int>>& s) { // заполнение массива единицами (используется при проигрыше для открытия всех клеток)
	for (int i = 0; i < s.size(); i++) {
		for (int j = 0; j < s[i].size(); j++) {
			s[i][j] = 1;
		}
	}
}

int main() {
	srand(time(0)); // Создание отправной точки для того, чтобы координаты не повторялись при повторном запуске программы
	const int len_xM[3]{ 13, 20, 27 }, len_yM[3]{ 6, 9, 13 }, number_of_minesM[3]{ 10, 35, 75 }; // константные значения по уровням сложности
	int difficulty;
	std::cout << "Choose your difficulty:\n1 - easy\n2 - medium\n3 - hard\n";
	std::cin >> difficulty;
	difficulty--;
	const int len_x = len_xM[difficulty], len_y = len_yM[difficulty], number_of_mines = number_of_minesM[difficulty];
	int left_to_open = len_x * len_y - number_of_mines;
	std::set<int> mine_coordinates;
	while (mine_coordinates.size() < number_of_mines) { // генерация случайных координат для мин
		int x = rand() % len_x + 1, y = rand() % len_y + 1;
		mine_coordinates.insert(x * 100 + y);
	}
	bool game_ongoing = true;
	std::vector<std::vector<int>> field(len_x + 2, std::vector<int>(len_y + 2)); // само поле
	std::vector<std::vector<int>> shown_or_hidden(len_x + 2, std::vector<int>(len_y + 2)); // отображение каждой клетки
	for (auto c : mine_coordinates) {
		int x = c / 100, y = c % 100;
		field[x][y] = -1; // мина
		for (int i = 0; i < 8; i++) {
			if (field[x + moves_x[i]][y + moves_y[i]] != -1) {
				field[x + moves_x[i]][y + moves_y[i]]++;
			}
		}
	}
	build(field, shown_or_hidden, len_y, len_x);
	const int length = len_x * len_y;
	bool chck = 1;
	while (game_ongoing and chck) {
		std::cout << "Your move(e.g. a2 / a2f): ";
		std::string move;
		std::cin >> move;
		int y = alphabet.find(move[0]) % 27, x = move_conv(move);
		if (move[move.size() - 1] == 'f') {
			shown_or_hidden[x][y] = 2;
		}
		else {
			if (field[x][y] != -1 and shown_or_hidden[x][y] != 1) {
				shown_or_hidden[x][y] = 1;
				left_to_open--;
			}
			else {
				field[x][y] = -2;
				fill_with_ones(shown_or_hidden);
				game_ongoing = false;
			}
		}
		// !!!! починить счётчик !!!!
		int change = 1, prev = 0, now;
		while (change) {
			now = 0;
			for (int i = 1; i <= len_x; i++) {
				for (int j = 1; j <= len_y; j++) {
					if (!field[i][j] and shown_or_hidden[i][j]) {
						for (int lft = 0; lft < 8; lft++) {
							if (!shown_or_hidden[i + moves_x[lft]][j + moves_y[lft]]) {
								shown_or_hidden[i + moves_x[lft]][j + moves_y[lft]] = 1;
								now++;
								left_to_open--;
							}
						}
					}
				}
			}
			change = prev - now;
			prev = now;
		}
		// !!!! починить счётчик !!!!
		build(field, shown_or_hidden, len_y, len_x);
		chck = check(shown_or_hidden, length);
	}
	if (!game_ongoing) {
		std::cout << "You lose\n";
	}
	else {
		std::cout << "You win!\n";
	}
	return 0;
}