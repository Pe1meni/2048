#include "PlayingField.h"

//оазмеры окна sfml
#define RESOLUTION_X 1080
#define RESOLUTION_Y 720

//функция для визуала в консоли (можно убрать)
void printMatrix(int(&matrix)[4][4]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
	return;
}

//получить длину стороны поля (плитка очевидно в 4 раза меньше)
int getSizeField() {
	int min = RESOLUTION_X > RESOLUTION_Y ? RESOLUTION_Y : RESOLUTION_X;
	return ((int)(min * 0.9 / 4)) * 4;
}


int main(void) {
	//просто для наглядности (можно убрать)
	PlayingField field; //создание рандомного поля через конструктор класса
	int Field[4][4]/*матрица с конечным положением плиток*/, moveField[4][4]/*матрица с перемещением плиток в выбранном направлении*/;
	char let; //вводимая с клавиатуры буква (управление wasd)
	field.getField(Field); //перенести данные из класса в матрицу
	printMatrix(Field);
	std::cin >> let;
	while (field.move(let)) {
		field.getMoveField(moveField); //перенести данные из класса в матрицу
		printMatrix(moveField);
		std::cout << std::endl;
		field.getField(Field);
		printMatrix(Field);
		std::cout << "-----------------------" << std::endl;
		std::cin >> let;
	}
	if (field.find2048()) {
		std::cout << "You won!" << std::endl;
	}
	else {
		std::cout << "You lost!" << std::endl;
	}
	return 0;
}