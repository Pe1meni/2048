#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>

class PlayingField {
private:
	int field[4][4];
	int moveField[4][4];
public:
	PlayingField();

	//перемещения
	bool move(char);
	bool tryW();
	void moveW();
	bool tryA();
	void moveA();
	bool tryS();
	void moveS();
	bool tryD();
	void moveD();
	
	//перенос поля
	void getField(int(&matrix)[4][4]); //взять поле плиток после перемещения
	void getMoveField(int (&matrix)[4][4]); //взять поле, отображающее на сколько ->
	//-> плиток в выбранном направлении сдвинулась плитка стоящаяя на этом месте

	//бесполезная фигня
	void Plus2();
	int getNumDig();
	bool find2048();
};