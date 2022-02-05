
#include <SFML/Graphics.hpp>
using namespace sf;
sf::View view;//объявили sfml объект "вид", который и является камерой

void setPlayerCoordinateForView(float x, float y)
{//ВНИМАНИЕ! Поскольку у нас новая карта-уделите внимание камере. В моем случае, чтобы не было пустого поля внизу я поправил код для нижней части экрана в файле view.h:
	//view.setCenter(x + 100, y);////следим за игроком, передавая его координаты камере. +100 - сместили камеру по иксу вправо. эксперементируйте
	float tempX = x; float tempY = y;//считываем коорд игрока и проверяем их, чтобы убрать края

	/*if (x < 320) tempX = 320;//убираем из вида левую сторону
	if (y < 240) tempY = 240;//верхнюю сторону
	if (y > 554) tempY = 554;//нижнюю сторону	*/

	//if (y > 624) tempY = 624;//нижнюю стороню.для новой карты

	view.setCenter(tempX, tempY-50); //следим за игроком, передавая его координаты
};

/*(Урок 12) void viewmap(float time) { //функция для перемещения камеры по карте. принимает время sfml


	if (Keyboard::isKeyPressed(Keyboard::D)) {
		view.move(0.1 * time, 0);//скроллим карту вправо (см урок, когда мы двигали героя - всё тоже самое)
	}

	if (Keyboard::isKeyPressed(Keyboard::S)) {
		view.move(0, 0.1 * time);//скроллим карту вниз (см урок, когда мы двигали героя - всё тоже самое)
	}

	if (Keyboard::isKeyPressed(Keyboard::A)) {
		view.move(-0.1 * time, 0);//скроллим карту влево (см урок, когда мы двигали героя - всё тоже самое)
	}
	if (Keyboard::isKeyPressed(Keyboard::W)) {
		view.move(0, -0.1 * time);//скроллим карту вправо (см урок, когда мы двигали героя - всё тоже самое)
	}


}*/
/*void changeview() {


	if (Keyboard::isKeyPressed(Keyboard::U)) {
		view.zoom(1.0100f); //масштабируем, уменьшение
		//view.zoom(1.0006f); //тоже самое помедленнее соответственно
	}

	if (Keyboard::isKeyPressed(Keyboard::R)) {
		//view.setRotation(90);//сразу же задает поворот камере
		view.rotate(1);//постепенно поворачивает камеру (отрицательное значение - в обратную сторону)
	}


	if (Keyboard::isKeyPressed(Keyboard::I)) {
		view.setSize(640, 480);//устанавливает размер камеры (наш исходный)
	}

	if (Keyboard::isKeyPressed(Keyboard::P)) {
		view.setSize(540, 380);//например другой размер
	}


	if (Keyboard::isKeyPressed(Keyboard::Q)) {
		view.setViewport(sf::FloatRect(0, 0, 0.5f, 1));//таким образом делается раздельный экран для игры на двоих. нужно только создать ещё один объект View и привязывать к нему координаты игрока 2.
	}



}*/