#include "PlayingField.h"

PlayingField::PlayingField() {
	srand(time(0));
	short x1 = rand() % 4, y1 = rand() % 4, x2 = -1, y2 = -1;
	while ((x1 == x2 && y1 == y2) || (x2 == -1 && y2 == -1)) {
		x2 = rand() % 4;
		y2 = rand() % 4;
	}
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if ((x1 == i && y1 == j) || (x2 == i && y2 == j)) {
				field[i][j] = 2;
			}
			else field[i][j] = 0;
		}
	}
}

void PlayingField::getField(int(&matrix)[4][4]) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			matrix[i][j] = field[i][j];
		}
	}
}

bool PlayingField::move(char let) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			moveField[i][j] = 0;
			assocField[i][j] = 0;
		}
	}
	if (let == 'w' && tryW()) {
		moveW();
		Plus2();
	}
	else if (let == 'a' && tryA()) {
		moveA();
		Plus2();
	}
	else if (let == 's' && tryS()) {
		moveS();
		Plus2();
	}
	else if (let == 'd' && tryD()) {
		moveD();
		Plus2();
	}
	if (((tryW() || tryA() || tryS() || tryD()) == false) || (find2048())) {
		return false;
	}
	return true;
}

int PlayingField::getNumDig() {
	int num = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (field[i][j] != 0) {
				num++;
			}
		}
	}
	return num;
}

void PlayingField::Plus2() {
	srand(time(0));
	int randNum = rand() % (16 - getNumDig());
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (field[i][j] == 0) {
				randNum--;
			}
			if (randNum == -1) {
				field[i][j] = 2;
				return;
			}
		}
	}
	return;
}

void PlayingField::getMoveField(int(&matrix)[4][4]) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			matrix[i][j] = moveField[i][j];
		}
	}
}

bool PlayingField::tryS() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			if ((field[i + 1][j] == 0 || field[i + 1][j] == field[i][j]) && field[i][j] != 0) {
				return true;
			}
		}
	}
	return false;
}

void PlayingField::moveS() {
	for (int i = 2; i >= 0; i--) {
		for (int j = 0; j < 4; j++) {
			if (field[i][j] != 0) {
				int counter = 0;
				while (i + counter + 1 < 4 && field[i + counter + 1][j] == 0) {
					counter++;
				}
				if (i + counter + 1 == 4) {
					moveField[i][j] = counter;
					field[i + counter][j] = field[i][j];
					field[i][j] = 0;
				}
				else if (field[i + counter + 1][j] == field[i][j] && assocField[i + counter + 1][j] == 0) {
					moveField[i][j] = counter + 1;
					field[i + counter + 1][j] = field[i][j] * 2;
					assocField[i + counter + 1][j] = 1;
					field[i][j] = 0;
				}
				else {
					moveField[i][j] = counter;
					field[i + counter][j] = field[i][j];
					if (counter != 0) {
						field[i][j] = 0;
					}
				}
			}
		}
	}
	return;
}

bool PlayingField::tryW() {
	for (int i = 1; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if ((field[i - 1][j] == 0 || field[i - 1][j] == field[i][j]) && field[i][j] != 0) {
				return true;
			}
		}
	}
	return false;
}

void PlayingField::moveW() {
	for (int i = 1; i <= 3; i++) {
		for (int j = 0; j < 4; j++) {
			if (field[i][j] != 0) {
				int counter = 0;
				while (i - counter - 1 > -1 && field[i - counter - 1][j] == 0) {
					counter++;
				}
				if (i - counter - 1 == -1) {
					moveField[i][j] = counter;
					field[i - counter][j] = field[i][j];
					field[i][j] = 0;
				}
				else if (field[i - counter - 1][j] == field[i][j] && assocField[i - counter - 1][j] == 0) {
					moveField[i][j] = counter + 1;
					field[i - counter - 1][j] = field[i][j] * 2;
					assocField[i - counter - 1][j] = 1;
					field[i][j] = 0;
				}
				else {
					moveField[i][j] = counter;
					field[i - counter][j] = field[i][j];
					if (counter != 0) {
						field[i][j] = 0;
					}
				}
			}
		}
	}
	return;
}

bool PlayingField::tryD() {
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 4; i++) {
			if ((field[i][j + 1] == 0 || field[i][j + 1] == field[i][j]) && field[i][j] != 0) {
				return true;
			}
		}
	}
	return false;
}

void PlayingField::moveD() {
	for (int j = 2; j >= 0; j--) {
		for (int i = 0; i < 4; i++) {
			if (field[i][j] != 0) {
				int counter = 0;
				while (j + counter + 1 < 4 && field[i][j + counter + 1] == 0) {
					counter++;
				}
				if (j + counter + 1 == 4) {
					moveField[i][j] = counter;
					field[i][j + counter] = field[i][j];
					field[i][j] = 0;
				}
				else if (field[i][j + counter + 1] == field[i][j] && assocField[i][j + counter + 1] == 0) {
					moveField[i][j] = counter + 1;
					field[i][j + counter + 1] = field[i][j] * 2;
					assocField[i][j + counter + 1] = 1;
					field[i][j] = 0;
				}
				else {
					moveField[i][j] = counter;
					field[i][j + counter] = field[i][j];
					if (counter != 0) {
						field[i][j] = 0;
					}
				}
			}
		}
	}
	return;
}

bool PlayingField::tryA() {
	for (int j = 1; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			if ((field[i][j - 1] == 0 || field[i][j - 1] == field[i][j]) && field[i][j] != 0) {
				return true;
			}
		}
	}
	return false;
}

void PlayingField::moveA() {
	for (int j = 1; j <= 3; j++) {
		for (int i = 0; i < 4; i++) {
			if (field[i][j] != 0) {
				int counter = 0;
				while (j - counter - 1 > -1 && field[i][j - counter - 1] == 0) {
					counter++;
				}
				if (j - counter - 1 == -1) {
					moveField[i][j] = counter;
					field[i][j - counter] = field[i][j];
					field[i][j] = 0;
				}
				else if (field[i][j - counter - 1] == field[i][j] && assocField[i][j - counter - 1] == 0) {
					moveField[i][j] = counter + 1;
					field[i][j - counter - 1] = field[i][j] * 2;
					assocField[i][j - counter - 1] = 1;
					field[i][j] = 0;
				}
				else {
					moveField[i][j] = counter;
					field[i][j - counter] = field[i][j];
					if (counter != 0) {
						field[i][j] = 0;
					}
				}
			}
		}
	}
	return;
}

bool PlayingField::find2048() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (field[i][j] == 2048) {
				return true;
			}
		}
	}
	return false;
}
