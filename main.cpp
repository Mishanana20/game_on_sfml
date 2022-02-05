#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
//#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>
#include "mission.h"
#include "LifeBar.h"
#include "iostream"
#include "level.h"
#include <vector>
#include <list>

#include "TinyXML/tinyxml.h"

using namespace sf;
////////////////////////////////////Общий класс-родитель//////////////////////////
class Entity {
public:
	std::vector<Object> obj;//вектор объектов карты
	float dx, dy, x, y, speed, moveTimer,AttackTimer,DeadTimer;
	int w, h, health;
	bool life, isMove, onGround, changeD;
	bool  kickL, kickR, kickUp, kickLB, kickRB, kickUpB;
	bool isAttack;
	Texture texture;
	Sprite sprite;
	String name;
	bool isShootEnemy;
	//bool isShootPlayer;
	float currentFrame;//текущий кадр для анимации
	float currentFrameAttack;
	Entity(Image& image, String Name, float X, float Y, int W, int H) {
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0; AttackTimer = 0; DeadTimer=0;
		speed = 0; health = 500; dx = 0; dy = 0;
		life = true; onGround = false; isMove = false; isShootEnemy = true; changeD = false;
		kickL = false; kickR = false; kickUp = false;
		kickLB = false; kickRB = false; kickUpB = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}

	FloatRect getRect() {//ф-ция получения прямоугольника. его коорд,размеры (шир,высот).
		return FloatRect(x, y, w, h);//эта ф-ция нужна для проверки столкновений 
	}

	virtual void update(float time) = 0;//Виртуальная функция — это функция, объявленная с ключевым словом virtual в базовом классе и переопределенная в одном или в нескольких производных классах.
};
////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;
	int playerScore;
	bool isShootPlayer, isSound2Play,isWin;

	Player(Image& image, String Name, Level& lev, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		playerScore = 0; isSound2Play = false; isWin = true; state = stay; obj = lev.GetAllObjects();//инициализируем.получаем все объекты для взаимодействия персонажа с картой
		if (name == "Player1")
		{
			sprite.setTextureRect(IntRect(0, 254, w, h));
		}
	}

	void control(float &time) {
		moveTimer += time;
		if (Keyboard::isKeyPressed) {
			if (Keyboard::isKeyPressed(Keyboard::Space))
			{
				
				if (moveTimer >= 150)
				{
					moveTimer = 0;
					isShootPlayer = true;
				}
			}
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				state = left; speed = 0.1;
				currentFrame += 0.01 * (time);
				if (currentFrame > 4) currentFrame -= 4;

				if (dx < 0) sprite.setTextureRect(IntRect(40 * int(currentFrame) + 40, 254, -40, 40));
			}
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				state = right; speed = 0.1;
				currentFrame += 0.010 * (time);
				if (currentFrame > 4) currentFrame -= 4;
				if (dx > 0) sprite.setTextureRect(IntRect(40 * int(currentFrame), 254, 40, 40));//будем менять первую координату (координата текстуры сдвигается каждый раз на 40)
			}

			if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {
				if (state == left) 
				{
					state = jump; dy = -0.6; onGround = false;
					state = left;
					
				}
				if (state == right)
				{
					state = jump; dy = -0.6; onGround = false;
					state = right;
					
				}
			}

			if (Keyboard::isKeyPressed(Keyboard::Down)) {
				state = down;
			}
		}
	}



	void checkCollisionWithMap(float Dx, float Dy)
	{
		

		for (int i = 0; i < obj.size(); i++)//проходимся по объектам
			if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
			{
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx > 0) { x = obj[i].rect.left - w; }
					if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width;}
				}
				if (obj[i].name == "thorn")//если встретили препятствие// ЭТО БУДЕТ БАТУТ// Теперь это телепорт
				{
					x = 1672;
					y = 1051;
					//health -= 5;
				}
				if (obj[i].name == "mission")
				{
					isSound2Play = true;
				}
			}
	}

	void update(float time)
	{
		if (health > 0) { life = true; }

		if (life == true)
		{
			control(time);
			switch (state)
			{
			case right:dx = speed; break;
			case left:dx = -speed; break;
			case up: break;
			case down: dx = 0; break;
			case stay: break;
			}

			if (kickUp) { dy = -0.4; kickUp = false; }
			if (kickR) {
				dx = 0.3;
			}
			if (kickL) {
				dx = -0.3;
			}
			if (onGround) { kickR = false; kickL = false; }

			/////тоже самое для босса
			if (kickUpB) { dy = -0.8; kickUpB = false; }
			if (kickRB) {
				dx = 0.6;
			}
			if (kickLB) {
				dx = -0.6;
			}
			if (onGround) { kickRB = false; kickLB = false; }

			x += dx * time;
			checkCollisionWithMap(dx, 0);
			y += dy * time;
			checkCollisionWithMap(0, dy);
			sprite.setPosition(x + w / 2, y + h / 2);
			if (health > 0) { life = true; sprite.setColor(Color::White);}
			if (health <= 0) { life = false; speed = 0; }
			if (!isMove) { speed = 0; }
			setPlayerCoordinateForView(x, y);
			if (life) { setPlayerCoordinateForView(x, y); }
			dy = dy + 0.0015 * time;

		}
		if (life == false)
		{
			DeadTimer += time;
			sprite.setColor(Color::Red);
			if (DeadTimer > 3000)
			{
				if (getplayercoordinateY()<986)
				{ //Если текущая миссия 0, то 
					x = 492; y = 896;
				}
				if (getplayercoordinateY() >=985)
				{ //Если текущая миссия 1, то 
					x = 1690; y = 1087;
				}
				//if ((getCurrentMission(getplayercoordinateX(),getplayercoordinateY())) == 2)
				//{ //Если текущая миссия 2, то 
				//	x = 800; y = 710;//эти координаты должны совпасть с координатами контрольных точек на самой карте
				//}
				health = 500;
				life = true;
				DeadTimer = 0;
			}
		}

	}
	int getHealth() { return health; }
	float getplayercoordinateX() {
		return x;
	}
	float getplayercoordinateY() {
		return y;
	}
};



class Enemy :public Entity {
public:
	//bool isShootEnemy;
	Enemy(Image& image, String Name, Level& lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		obj = lvl.GetObjects("solid");//инициализируем.получаем нужные объекты для взаимодействия врага с картой
		obj = lvl.GetObjects("FlagLR");
		isAttack=false;
		currentFrameAttack = 0;
		if (name == "EnemyBoss")
		{
			health = 750;
			sprite.setTextureRect(IntRect(0, 0, w, h));
			dx = 0.15;
		}
		if (name == "thorn2")
		{
			health = 10000;
			sprite.setTextureRect(IntRect(0, 0, w, h));
			dx = 0;
		}

		if (name == "easyEnemy") 
		{
			health = 200;
			sprite.setTextureRect(IntRect(0, 0, w, h));
			dx = 0.1;
			//isShootEnemy = false;
		}

		if (name == "snake")
		{
			health = 250;
			sprite.setTextureRect(IntRect(0, 0, w, h));
			dx = 0.15;
			//isShootEnemy = false;
		}

	}

	void checkCollisionWithMap(float Dx, float Dy)
	{


		for (int i = 0; i < obj.size(); i++)//проходимся по объектам
			if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
			{
				if (obj[i].name == "solid")
				{//если встретили препятствие (объект с именем solid)
					if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx > 0) { x = obj[i].rect.left - w;  dx = -0.1; /*sprite.scale(-1, 1);*/ }
					if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width; dx = 0.1; /*sprite.scale(-1, 1);*/ }

				}
				if (obj[i].name == "FlagLR")//флаг смены направления
				{
					if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx > 0) { x = obj[i].rect.left - w;  dx = -0.1; /*sprite.scale(-1, 1);*/ }
					if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width; dx = 0.1; }
				}
			}
	}
	void update(float time)
	{
		//////Яйцо НЕ РАБОТАЕТ
		/*if (name == "eggs")
		{
			checkCollisionWithMap(dx, 0);
			if (life == true) {
				if (health >0)
				{
					currentFrame += 0.01 * (time);
					if (currentFrame > 1) currentFrame -= 1;
					dx = 0;
					sprite.setTextureRect(IntRect(64 * int(currentFrame), 40, 60, 85));
				}
				if (health <= 150)
				{
					dx = 0;
					currentFrame += 0.01 * (time);
					if (currentFrame > 1) currentFrame -= 1;
					sprite.setTextureRect(IntRect(65 * int(currentFrame), 40, 105, 86));
				}
				if (health <= 100)
				{
					dx = 0;
					currentFrame += 0.01 * (time);
					if (currentFrame > 1) currentFrame -= 1;
					sprite.setTextureRect(IntRect(170 * int(currentFrame), 40, 102, 100));
				}
			}
		}*/
		if (name == "thorn2")
		{
			if (life == true)
			{
				checkCollisionWithMap(dx, 0);
				dx = 0;
			}
		}


			if (name=="EnemyBoss")
			  {
					if (life == true)
					{
						checkCollisionWithMap(dx, 0);
						if (health > 50)
						{
							x += dx * time;

							if (health == 750)//когда хп на максимум
							{
								moveTimer += time; if (moveTimer > 3000) { dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
							}

							if (dx < 0)
							{
								currentFrame += 0.01 * (time);
								if (currentFrame >12) currentFrame -= 12;
								sprite.setTextureRect(IntRect(206 * int(currentFrame), 30, 203, 120));
								if (isAttack == true)
								{
									if (currentFrameAttack >12 ) {currentFrameAttack -=7;}
										currentFrameAttack += 0.01 * (time);
										sprite.setTextureRect(IntRect(206 * int(currentFrameAttack), 30, 203, 120));
									isAttack = false;
								}
								/*if (isAttack == true)
								{
									currentFrame = 0; 
									if (currentFrame <7 ) 
									{
									currentFrame += 0.01 * (time);
									if (currentFrame > 6) currentFrame -= 6;
									sprite.setTextureRect(IntRect(192 * int(currentFrame), 160, 192, 100));
									AttackTimer += time; 
									if (AttackTimer > 1000) { isAttack = false; AttackTimer = 0; }
								     }
								}*/
							}
							if (dx > 0)
							{
									currentFrame += 0.01 * (time);
									if (currentFrame > 12) currentFrame -= 12;
									sprite.setTextureRect(IntRect(206 * int(currentFrame) + 206,30, -203, 120));
									if (isAttack == true)
									{
										if (currentFrameAttack >12 )
										{currentFrameAttack -= 7; }
											currentFrameAttack += 0.01 *(time);
											sprite.setTextureRect(IntRect(206 * int(currentFrameAttack) + 206, 30, -203, 120));
									isAttack = false;
								}
							}
						}
						if (health <= 100) //|| health <50) 
						{
							isShootEnemy = false;
							dx = 0;
							currentFrame += 0.004 * (time);
							if (currentFrame > 6) currentFrame = 6;
							sprite.setTextureRect(IntRect(244 * int(currentFrame) + 244, 900, -244, 186));
						}
						sprite.setPosition(x + w / 2, y + h / 2);
						//if (health >=30 || health<=60)// ТУТ ДОБАВЛЮ НОВУЮ АНИМАЦИЮ С РАНЕНЫМ ЧЕЛИКОМ И НОВОЙ СКОРОСТЬ ПЕРЕДВИЖЕНИЯ { life = false; }
						if (health <= 0) { life = false; }
					}
					if (life == false)//ЕЩЕ РАЗ ВЫСТРЕЛИТЬ ЧТОБЫ УДАЛИТЬ ОКОНЧАТЕЛЬНО ВРАГА
					{
						dx = 0;
						currentFrame += 0.007 * (time);
						if (currentFrame > 6) currentFrame *= dx;
						sprite.setTextureRect(IntRect(57.5 * int(currentFrame) + 52, 428, -52, 52));
					}

					if (changeD) {//Это нужно для того, чтобы отразить один раз врага в сторону игрока, если враг в этот момент смотрит в другую сторону.
						sprite.scale(-1, 1);
						changeD = false;
					}

				}
			/////////////////////////////конец змеи Boss////////////////////////////////
		
		if (name == "easyEnemy") {
			
			if (life == true)
			{
				checkCollisionWithMap(dx, 0);
				if (health >= 50 )
				{
					//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
					checkCollisionWithMap(dx, 0);
					x += dx * time;

					if (dx < 0)
					{
						currentFrame += 0.01 * (time);
						if (currentFrame > 6) currentFrame -= 6;

						sprite.setTextureRect(IntRect(57.5 * int(currentFrame), 63, 52, 52));
					}
					if (dx > 0)
					{
						currentFrame += 0.01 * (time);
						if (currentFrame > 6) currentFrame -= 6;

						sprite.setTextureRect(IntRect(57.5 * int(currentFrame) + 52, 63, -52, 52));
					}


					if (!isShootEnemy)
					{
						moveTimer += time;
						if (moveTimer >= 1500)
						{
							isShootEnemy = true;
							moveTimer = 0;
						}
					}
				}
				if (health <=50) //|| health <50) 
				{
					isShootEnemy = false;
					dx = 0;
					currentFrame += 0.007 * (time);
					if (currentFrame > 6) currentFrame *= dx;
					sprite.setTextureRect(IntRect(57.5 * int(currentFrame) + 52, 428, -52, 52));
				}
				sprite.setPosition(x + w / 2, y + h / 2);
				//if (health >=30 || health<=60)// ТУТ ДОБАВЛЮ НОВУЮ АНИМАЦИЮ С РАНЕНЫМ ЧЕЛИКОМ И НОВОЙ СКОРОСТЬ ПЕРЕДВИЖЕНИЯ { life = false; }
				if (health <= 0) { life = false; }
			}
			if (life == false)//ЕЩЕ РАЗ ВЫСТРЕЛИТЬ ЧТОБЫ УДАЛИТЬ ОКОНЧАТЕЛЬНО ВРАГА
			{
				dx = 0;
				currentFrame += 0.007 * (time);
				if (currentFrame > 6) currentFrame *= dx;
				sprite.setTextureRect(IntRect(57.5 * int(currentFrame) + 52, 428, -52, 52));
			}
			if (changeD) {//Это нужно для того, чтобы отразить один раз врага в сторону игрока, если враг в этот момент смотрит в другую сторону.
				sprite.scale(-1, 1);
				changeD = false;
			}

		}

		////////////////////////попробуем добавить змею/////////////////////////////
		if (name == "snake") {

			if (life == true)
			{
				checkCollisionWithMap(dx, 0);
				if (health > 50)
				{
					//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
					
					x += dx * time;

					if (health == 250)//когда хп на максимум
					{
						moveTimer += time; if (moveTimer > 3000) { dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
					}

					if (dx < 0)
					{
						currentFrame += 0.01 * (time);
						if (currentFrame > 6) currentFrame -= 6;

						sprite.setTextureRect(IntRect(52.5 * int(currentFrame), 42, 51, 26));
					}
					if (dx > 0)
					{
						currentFrame += 0.01 * (time);
						if (currentFrame > 6) currentFrame -= 6;

						sprite.setTextureRect(IntRect(52.5 * int(currentFrame) + 52.5,42, -52.5, 26));
					}


					if (!isShootEnemy)
					{
						moveTimer += time;
						if (moveTimer >= 1000)
						{
							isShootEnemy = true;
							moveTimer = 0;
						}
					}
				}
				if (health <= 50) //|| health <50) 
				{
					isShootEnemy = false;
					dx = 0;
					currentFrame += 0.007 * (time);
					if (currentFrame > 6) currentFrame *= dx;
					sprite.setTextureRect(IntRect(63 * int(currentFrame) + 63, 236, -63, 42));
				}
				sprite.setPosition(x + w / 2, y + h / 2);
				//if (health >=30 || health<=60)// ТУТ ДОБАВЛЮ НОВУЮ АНИМАЦИЮ С РАНЕНЫМ ЧЕЛИКОМ И НОВОЙ СКОРОСТЬ ПЕРЕДВИЖЕНИЯ { life = false; }
				if (health <= 0) { life = false; }
			}
			if (life == false)//ЕЩЕ РАЗ ВЫСТРЕЛИТЬ ЧТОБЫ УДАЛИТЬ ОКОНЧАТЕЛЬНО ВРАГА
			{
				dx = 0;
				currentFrame += 0.007 * (time);
				if (currentFrame > 6) currentFrame *= dx;
				sprite.setTextureRect(IntRect(57.5 * int(currentFrame) + 52, 428, -52, 52));
			}

			if (changeD) {//Это нужно для того, чтобы отразить один раз врага в сторону игрока, если враг в этот момент смотрит в другую сторону.
				sprite.scale(-1, 1);
				changeD = false;
			}

		}
		/////////////////////////////конец змеи////////////////////////////////

	}
};

class MovingPlatform : public Entity {//класс движущейся платформы
	public:
		MovingPlatform(Image& image, String Name, Level& lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
			sprite.setTextureRect(IntRect(0, 0, W, H));//прямоугольник 
			obj = lvl.GetObjects("FlagLR");
			//obj = lvl.GetObjects("FlagUp");
			if (name == "MovingPlatform")
			{
				dx = 0.05;//изначальное ускорение по Х
			}
			if (name == "MovingPlatformBoss")
			{
				dx = 0;//изначальное ускорение по Х
			}
		}
		void checkCollisionWithMap(float Dx, float Dy)
		{
			for (int i = 0; i < obj.size(); i++)//проходимся по объектам
				if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
				{
					if (obj[i].name == "FlagLR")
					{//если встретили препятствие (объект с именем solid)
						if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; }
						if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
						if (Dx > 0) { x = obj[i].rect.left - w;  dx*=-1; /*sprite.scale(-1, 1);*/ }
						if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width; dx *= -1; /*sprite.scale(-1, 1);*/ }

					}

				}
		}

		void update(float time)//функция обновления платформы.
		{
			if (name == "MovingPlatform")
			{
				checkCollisionWithMap(dx, 0);
				x += dx * time;//реализация движения по горизонтали
				//y += dy * time;
				//moveTimer += time;//наращиваем таймер
				//if (moveTimer > 2000) { dx *= -1; moveTimer = 0; }//если прошло примерно 2 сек, то меняется направление движения платформы,а таймер обнуляется
				sprite.setPosition(x + w / 2, y + h / 2);//задаем позицию спрайту
			}
			if (name == "MovingPlatform")
			{
				checkCollisionWithMap(dx, 0);
				x += dx * time;//реализация движения по горизонтали
				//y += dy * time;
				//moveTimer += time;//наращиваем таймер
				//if (moveTimer > 2000) { dx *= -1; moveTimer = 0; }//если прошло примерно 2 сек, то меняется направление движения платформы,а таймер обнуляется
				sprite.setPosition(x + w / 2, y + h / 2);//задаем позицию спрайту
			}
		}
};

class Bullet :public Entity {//класс пули
public:
	int direction;//направление пули
	Bullet(Image& image, String Name, Level& lvl, float X, float Y, int W, int H, int dir) :Entity(image, Name, X, Y, W, H) {//всё так же, только взяли в конце состояние игрока (int dir)
		obj = lvl.GetObjects("solid");//инициализируем .получаем нужные объекты для взаимодействия пули с картой
		x = X;
		y = Y;
		direction = dir;
		speed = 0.1;
		w = h = 36;
		life = true;
		//выше инициализация в конструкторе
	}


	void update(float time)
	{
		if (name == "Bullet")
		{
			speed = 0.3;
			switch (direction)
			{
			case 0: dx = -speed; dy = 0;   break;//интовое значение state = left
			case 1: dx = speed; dy = 0;   break;//интовое значение state = right
			case 2: life = false; dx = 0; dy = -speed;   break;//интовое значение state = up
			case 3: life = false; dx = 0; dy = -speed;   break;//интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
			case 4: life = false; dx = 0; dy = -speed;   break;//интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
			case 5:life = false; dx = 0; dy = -speed;   break;//интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
			}
			x += dx * time;//само движение пули по х
			y += dy * time;//по у
			if (x <= 0) x = 1;// задержка пули в левой стене, чтобы при проседании кадров она случайно не вылетела за предел карты и не было ошибки
			if (y <= 0) y = 1;
			for (int i = 0; i < obj.size(); i++) {//проход по объектам solid
				if (getRect().intersects(obj[i].rect)) //если этот объект столкнулся с пулей,
				{
					life = false;// то пуля умирает

				}
			}
			sprite.setPosition(x + w / 2, y + h / 2);//задается позицию пуле
		}

		if (name == "BulletEnemy")
		{
			speed = 0.2;
			switch (direction)
			{
			case 0: dx = -speed; dy = 0;   break;//интовое значение state = left
			case 1: dx = speed; dy = 0;   break;//интовое значение state = right
			case 2: dx = 0; dy = -speed;   break;//интовое значение state = up
			case 3: dx = 0; dy = -speed;   break;//интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
			case 4: dx = 0; dy = -speed;   break;//интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
			case 5: dx = 0; dy = -speed;   break;//интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
			}
			x += dx * time;//само движение пули по х
			y += dy * time;//по у
			if (x <= 0) x = 1;// задержка пули в левой стене, чтобы при проседании кадров она случайно не вылетела за предел карты и не было ошибки
			if (y <= 0) y = 1;
			for (int i = 0; i < obj.size(); i++) {//проход по объектам solid
				if (getRect().intersects(obj[i].rect)) //если этот объект столкнулся с пулей,
				{
					life = false;// то пуля умирает
				}
			}
			



			sprite.setPosition(x + w / 2, y + h / 2);//задается позицию пуле
		}

	}



};

void menu(RenderWindow& window) {
	Music music;//создаем объект музыки
	music.openFromFile("sounds/люб тема.ogg");//загружаем файл
	music.play();//воспроизводим музыку
	music.setLoop(true);

	Texture menuTexture1, menuTexture2, menuTexture3, aboutTexture, menuBackground;
	menuTexture1.loadFromFile("images/новая игра.png");
	menuTexture2.loadFromFile("images/о программе.png");
	menuTexture3.loadFromFile("images/выход.png");
	aboutTexture.loadFromFile("images/about.png");
	menuBackground.loadFromFile("images/Хогвартс.jpg");
	Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), about(aboutTexture), menuBg(menuBackground);
	bool isMenu = 1;
	int menuNum = 0;
	menu1.setPosition(100, 30);
	menu2.setPosition(100, 90);
	menu3.setPosition(100, 150);
	menuBg.setPosition(0, 0);

	//////////////////////////////МЕНЮ///////////////////
///////////////////СДЕЛАЛ МЕНЮ!! НЕОБХОДИМО ЗАДАТЬ ФОНОВУЮ МУЗЫКУ И ДОБАВИТЬ УПРАВЛЕНИЕ В "О ПРОГРАММЕ"////////////////
	/////////////////////////НАВЕРНОЕ ЭТО БУДЕТ ЧТО-ТО ПО ТИПУ "С ЛЮБОВЬЮ ДЛЯ ПОЛЕЧКИ"//////////////
	
	while (isMenu)
	{
		
		Event event;
		while (window.pollEvent(event))
		{
			
			if (event.type == Event::Closed)
				window.close();
			
		}
		menu1.setColor(Color::White);
		menu2.setColor(Color::White);
		menu3.setColor(Color::White);
		menuNum = 0;
		window.clear(Color(129, 181, 221));

		if (IntRect(100, 30, 300, 50).contains(Mouse::getPosition(window))) { menu1.setColor(Color::Blue); menuNum = 1; }
		if (IntRect(100, 90, 300, 50).contains(Mouse::getPosition(window))) { menu2.setColor(Color::Blue); menuNum = 2; }
		if (IntRect(100, 150, 300, 50).contains(Mouse::getPosition(window))) { menu3.setColor(Color::Blue); menuNum = 3; }

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) isMenu = false;//если нажали первую кнопку, то выходим из меню 
			//if (menuNum == 2) { window.draw(about); window.display(); while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
			if (menuNum == 2) { do { window.draw(about); window.display(); } while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
			if (menuNum == 3) { window.close(); isMenu = false; }

		}


		window.draw(menuBg);
		window.draw(menu1);
		window.draw(menu2);
		window.draw(menu3);

		window.display();
	}
	////////////////////////////////////////////////////
}

int main()
{
	RenderWindow window(VideoMode(1080, 720), "Harry");
	menu(window);//вызов меню
	view.reset(FloatRect(0, 0, 640, 480));
	
	Music music;//создаем объект музыки
	music.openFromFile("начало гарри.ogg");//загружаем файл
	music.play();//воспроизводим музыку
	music.setLoop(true);//повтор музыки

	SoundBuffer buffer;
	buffer.loadFromFile("sounds/змея шипит.ogg");
	Sound sound(buffer);
	SoundBuffer buffer1;
	buffer1.loadFromFile("sounds/змея атака.ogg");
	Sound sound1(buffer1);
	SoundBuffer buffer3;
	buffer3.loadFromFile("sounds/свиток.ogg");
	Sound sound3(buffer3);
	sound3.setVolume(10);//громкость звука
	sound1.setVolume(15);
	sound.setVolume(15);

	SoundBuffer buffer2;
	buffer2.loadFromFile("sounds/маг пауза.ogg");
	Sound sound2(buffer2);
	sound2.setVolume(15);

	//Text Для МИССИИЙ
	Font font;
	font.loadFromFile("Caslon.ttf");
	Text text("", font, 15);
	text.setFillColor(Color(0,0,0));
	
	Image quest_image;
	quest_image.loadFromFile("images/missionbg1.jpg");
	quest_image.createMaskFromColor(Color(0, 0, 0));
	Texture quest_texture;
	quest_texture.loadFromImage(quest_image);
	Sprite s_quest;
	s_quest.setTexture(quest_texture);
	s_quest.setTextureRect(IntRect(0, 0, 340, 510));  //приведение типов, размеры картинки исходные
	s_quest.setScale(0.6f, 0.6f);//чуть уменьшили картинку, => размер стал меньше
	bool showMissionText = true;
	//Text end


	Level lvl;//создали экземпляр класса уровень
	lvl.LoadFromFile("map.tmx");//загрузили в него карту, внутри класса с помощью методов он ее обработает.


	std::list<Entity*>  entities;//создаю список, сюда буду кидать объекты.например врагов.
	std::list<Entity*>::iterator it;//итератор чтобы проходить по эл-там списка
	std::vector<Object> e = lvl.GetObjects("easyEnemy");//все объекты врага на tmx карте хранятся в этом векторе
	//std::vector<Object> e = lvl.GetObjects("snake");//все объекты врага на tmx карте хранятся в этом векторе
	


	LifeBar lifeBarPlayer;
	bool isWiner = false;
	bool isWin=false;



	Image heroImage;
	heroImage.loadFromFile("images/fang1.png");

	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images/дементор2.png");
	easyEnemyImage.createMaskFromColor(Color(255, 0, 0));\
		for (int i = 0; i < e.size(); i++)//проходимся по элементам этого вектора(а именно по врагам)
			entities.push_back(new Enemy(easyEnemyImage, "easyEnemy", lvl, e[i].rect.left, e[i].rect.top, 52, 52));//и закидываем в список всех наших врагов с карты

	std::vector<Object> eSnake = lvl.GetObjects("snake");//все объекты врага на tmx карте хранятся в этом векторе
	Image snakeImage;
	snakeImage.loadFromFile("images/змея.png");
	snakeImage.createMaskFromColor(Color(0, 248, 0)); \
		snakeImage.createMaskFromColor(Color(0, 255, 255)); \
		for (int i = 0; i < eSnake.size(); i++)//проходимся по элементам этого вектора(а именно по врагам)
			entities.push_back(new Enemy(snakeImage, "snake", lvl, eSnake[i].rect.left, eSnake[i].rect.top, 52, 26));//и закидываем в список всех наших врагов с карты

	std::vector<Object> eSnakeBoss = lvl.GetObjects("EnemyBoss");//все объекты врага на tmx карте хранятся в этом векторе
	Image snakeBossImage;
	snakeBossImage.loadFromFile("images/EnemyBoss1.png");
	snakeBossImage.createMaskFromColor(Color(0, 248, 0)); \
		snakeBossImage.createMaskFromColor(Color(0, 0, 0)); \
		for (int i = 0; i < eSnakeBoss.size(); i++)//проходимся по элементам этого вектора(а именно по врагам)
			entities.push_back(new Enemy(snakeBossImage, "EnemyBoss", lvl, eSnakeBoss[i].rect.left, eSnakeBoss[i].rect.top, 185, 126));//и закидываем в список всех наших врагов с карты


	std::vector<Object> eThorn2 = lvl.GetObjects("thorn2");
	Image Thorn2;
	Thorn2.loadFromFile("images/Thorn2.png");
	for (int i = 0; i < eThorn2.size(); i++)//проходимся по элементам этого вектора(а именно по врагам)
		entities.push_back(new Enemy(Thorn2, "thorn2", lvl, eThorn2[i].rect.left, eThorn2[i].rect.top, 31, 41));//и закидываем в список всех наших врагов с карты

	///////ЯЙЦО!!!! (НЕ работает)
	/*std::vector<Object> eGG = lvl.GetObjects("eggs");//все объекты врага на tmx карте хранятся в этом векторе
	Image eggImage;
	eggImage.loadFromFile("images/яйцо.png");
	for (int i = 0; i < eGG.size(); i++)//проходимся по элементам этого вектора(а именно по врагам)
		entities.push_back(new Enemy(easyEnemyImage, "eggs", lvl, eGG[i].rect.left, eGG[i].rect.top, 52, 52));*/


	Image BulletImage;//изображение для пули
	BulletImage.loadFromFile("images/син.png");//загрузили картинку в объект изображения
	BulletImage.createMaskFromColor(Color(0, 0, 0));//маска для пули по черному цвету

	Image BulletEnemyImage;//изображение для пули
	BulletEnemyImage.loadFromFile("images/красный.png");//загрузили картинку в объект изображения
	BulletEnemyImage.createMaskFromColor(Color(0, 0, 0));//маска для пули по черному цвету


	Image movePlatformImage;
	movePlatformImage.loadFromFile("images/MovingPlatform3.png");

	Image movePlatformImage1;
	movePlatformImage1.loadFromFile("images/MovingPlatform3.png");
	//std::vector<Object> player = lvl.GetObject("player");
	//for (int i = 0; i < eSnake.size(); i++)//проходимся по элементам этого вектора(а именно по врагам)
		//entities.push_back(new Enemy(heroImage, "snake", lvl, eSnake[i].rect.left, eSnake[i].rect.top, 52, 40));//и закидываем в список всех наших врагов с карты
	/*Image EnemyBossImage;
	EnemyBossImage.loadFromFile("images/EnemyBoss.png");
	Object EnemyBoss = lvl.GetObject("EnemyBoss");
	Enemy B(EnemyBossImage, "EnemyBoss", lvl, EnemyBoss.rect.left, EnemyBoss.rect.top, 200, 153);//передаем координаты прямоугольника player из карты в координаты нашего игрока*/


	Object player = lvl.GetObject("player");//объект игрока на нашей карте.задаем координаты игроку в начале при помощи него
	//Object easyEnemyObject = lvl.GetObject("easyEnemy");//объект легкого врага на нашей карте.задаем координаты игроку в начале при помощи него

	Player p(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 32, 40);//передаем координаты прямоугольника player из карты в координаты нашего игрока
	//Enemy easyEnemy(easyEnemyImage, "EasyEnemy", lvl, easyEnemyObject.rect.left, easyEnemyObject.rect.top, 200, 97);//передаем координаты прямоугольника easyEnemy из карты в координаты нашего врага

	e = lvl.GetObjects("MovingPlatform");//забираем все платформы в вектор 
	for (int i = 0; i < e.size(); i++) entities.push_back(new MovingPlatform(movePlatformImage, "MovingPlatform", lvl, e[i].rect.left, e[i].rect.top,96, 32));//закидываем платформу в список.передаем изображение имя уровень координаты появления (взяли из tmx карты), а так же размеры

	e = lvl.GetObjects("MovingPlatform1");//забираем все платформы в вектор 
	for (int i = 0; i < e.size(); i++) entities.push_back(new MovingPlatform(movePlatformImage, "MovingPlatform1", lvl, e[i].rect.left, e[i].rect.top, 96, 32));

	Clock clock;
	while (window.isOpen())
	{

		float time = clock.getElapsedTime().asMicroseconds();

		clock.restart();
		time = time / 800;

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			////////////////пуля событие////////////
			if (p.isShootPlayer == true)
			{
				p.isShootPlayer = false;
				entities.push_back(new Bullet(BulletImage, "Bullet", lvl, p.x, p.y,36,36,p.state));//если выстрелили, то появляется пуля, unam передаем как int
			}
			
			
			if (p.isSound2Play == true)
			{
				sound2.play();
				p.isSound2Play = false;
				
			}



			/*for (it = entities.begin(); it != entities.end(); it++) {
				if ((*it)->name == "easyEnemy")
				{
					if (p.state == p.right)
					{
						if (FloatRect(p.x, p.y, p.w * 3, p.h).intersects((*it)->getRect()))
							//if(((*it)->getRect()).intersects(IntRect(p.x, p.y, p.w * 3, p.h)))
						{
							if ((*it)->isShootEnemy == true) { (*it)->isShootEnemy = false; entities.push_back(new Bullet(BulletImage, "Bullet", lvl, (*it)->x, (*it)->y, 36, 36, p.left)); }
						}
					}
					else if (p.state == p.left)
					{
						//if (((*it)->getRect()).intersects(IntRect(p.x, p.y, p.w * 3, p.h)))
						if (FloatRect(p.x - p.w * 3, p.y, p.w * 3, p.h).intersects((*it)->getRect()))
							if ((*it)->isShootEnemy == true) { (*it)->isShootEnemy = false; entities.push_back(new Bullet(BulletImage, "Bullet", lvl, (*it)->x, (*it)->y, 36, 36, p.right)); }
					}
				}
			}*/
			///////////////Mission
			if (event.type == Event::KeyPressed)//событие нажатия клавиши
				if ((event.key.code == Keyboard::Tab)) {//если клавиша ТАБ


					switch (showMissionText) {//переключатель, реагирующий на логическую переменную showMissionText

					case true: {
						sound3.play();
						std::ostringstream playerHealthString;//строка здоровья игрока
						playerHealthString << p.health; //заносим в строку здоровье 
						std::ostringstream task;//строка текста миссии
						task << getTextMission(getCurrentMission(p.getplayercoordinateX(), p.getplayercoordinateY()));//вызывается функция getTextMission (она возвращает текст миссии), которая принимает в качестве аргумента функцию getCurrentMission(возвращающую номер миссии), а уже эта ф-ция принимает в качестве аргумента функцию p.getplayercoordinateX() (эта ф-ция возвращает Икс координату игрока)
						text.setString("\nПолечка\n Здоровье=" + playerHealthString.str() + "" + task.str());
						showMissionText = false;//эта строка позволяет убрать все что мы вывели на экране
						break;//выходим , чтобы не выполнить условие "false" (которое ниже)
					}
					case false: {
						text.setString("");//если не нажата клавиша таб, то весь этот текст пустой
						showMissionText = true;// а эта строка позволяет снова нажать клавишу таб и получить вывод на экран
						break;
					}
					}
				}//////Mission end
		}


		lifeBarPlayer.update(p.getHealth());//сюда передаем значение, которое надо нарисовать. Можно передать здоровья игрока тогда будет lifeBarPlayer.update(player.getHealth()); так


		p.update(time);
		for (it = entities.begin(); it != entities.end(); it++) { (*it)->update(time); }//для всех элементов списка(пока это только враги,но могут быть и пули к примеру) активируем ф-цию update
		window.setView(view);
		window.clear(Color(77, 83, 140));
		lvl.Draw(window);//рисуем новую карту

	
	/////////////////////Если я захочу удалять ВСЕ объекты при их смерти
	for (it = entities.begin(); it != entities.end();)//говорим что проходимся от начала до конца
		{
			Entity* b = *it;//для удобства, чтобы не писать (*it)->
			//b->update(time);//вызываем ф-цию update для всех объектов (по сути для тех, кто жив)
			if (b->life == false) { it = entities.erase(it); delete b; }// если этот объект мертв, то удаляем его
			else it++;//и идем курсором (итератором) к след объекту. так делаем со всеми объектами списка
		//////////////////////////////////
		}

			
			for (it = entities.begin(); it != entities.end(); it++) {
			

			////////////////////////////////////////какой-то код про убийство врага
			//////////////////// ПОКА НЕ РАБОТАЕТ///// РАБОТАЕТ!!!!!
			if ((*it)->name == "easyEnemy")
			{

				Entity* enemy = *it;

				for (std::list<Entity*>::iterator it2 = entities.begin(); it2 != entities.end(); it2++)
				{
					Entity* bullet = *it2;
					if (bullet->name == "Bullet")

						if (bullet->life == true)
						{

							if (bullet->getRect().intersects(enemy->getRect()))
							{
								bullet->life = false;
								std::cout << bullet->life << std::endl;
								enemy->  /*life = false;*/ health -= 50;
							}
						}
				}
			}
			if ((*it)->name == "snake")
			{

				Entity* enemy = *it;
				for (std::list<Entity*>::iterator it2 = entities.begin(); it2 != entities.end(); it2++)
				{
					Entity* bullet = *it2;
					if (bullet->name == "Bullet")

						if (bullet->life == true)
						{

							if (bullet->getRect().intersects(enemy->getRect()))
							{
								bullet->life = false;
								std::cout << bullet->life << std::endl;
								enemy->  /*life = false;*/ health -= 50;
							}
						}
				}
			}

			if ((*it)->name == "EnemyBoss")
			{

				Entity* enemy = *it;
				for (std::list<Entity*>::iterator it2 = entities.begin(); it2 != entities.end(); it2++)
				{
					Entity* bullet = *it2;
					if (bullet->name == "Bullet")

						if (bullet->life == true)
						{

							if (bullet->getRect().intersects(enemy->getRect()))
							{
								bullet->life = false;
								std::cout << bullet->life << std::endl;
								enemy->  /*life = false;*/ health -= 50;
							}
						}
				}
			}
			if ((*it)->name == "EnemyBoss")
			{
				Entity* enemy = *it;
			
				if (enemy->health > 100)
				{
					p.isWin = false;
					
				}
				else
				{
					p.isWin = true;
				}

			}
			if (p.isWin == true)
			{
				for (std::list<Entity*>::iterator it2 = entities.begin(); it2 != entities.end(); it2++)
				{
					Entity* thorn2 = *it2;
					if (thorn2->name == "thorn2")

							if (thorn2->getRect().intersects(p.getRect()))
							{
								p.x = 5454;
								p.y = 2001;
							}

				}
			}
			///// яйцо (НЕ РАБОТАЕТ!!!)
			/*if ((*it)->name == "eggs")
			{

				Entity* enemy = *it;
				for (std::list<Entity*>::iterator it2 = entities.begin(); it2 != entities.end(); it2++)
				{
					Entity* bullet = *it2;
					if (bullet->name == "Bullet")

						if (bullet->life == true)
						{

							if (bullet->getRect().intersects(enemy->getRect()))
							{
								bullet->life = false;
								std::cout << bullet->life << std::endl;
								enemy-> health -= 50;
							}
						}
				}
			}*/

			///////////////////////////////////НОВОЕ!!!!!!!!!!!
			if ((*it)->name == "BulletEnemy")
			{
				Entity* BulletEnemy = *it;

				////////////пули врезались в героя //////////////////
						if (BulletEnemy->life == true)
						{

							if (BulletEnemy->getRect().intersects(p.getRect()))
							{
								BulletEnemy->life = false;
								std::cout << BulletEnemy->life << std::endl;
								 p.health -= 50;
							}
						}
				}
				//////////////////////////////
			
			///////////////////////////////////////////////////////

			//////////////Старая реализация нахождения персонажа врагом
			/*if ((*it)->name == "easyEnemy")
			{
				if (p.state == p.right)// && p.state == p.stay)//Не работает!! все время стреляют вправо!!! и пропал один враг!
				{
					if (FloatRect(p.x, p.y, p.w * 5, p.h*2).intersects((*it)->getRect())) 
						//if(((*it)->getRect()).intersects(IntRect(p.x, p.y, p.w * 3, p.h)))
					{
						if ((*it)->isShootEnemy == true) { (*it)->isShootEnemy = false; entities.push_back(new Bullet(BulletEnemyImage, "BulletEnemy", lvl, (*it)->x, (*it)->y, 36, 36, p.left)); }
					}
				}
				else if (p.state == p.left)
				{
					//if (((*it)->getRect()).intersects(IntRect(p.x, p.y, p.w * 3, p.h)))
					if (FloatRect(p.x - p.w , p.y, p.w * 5, p.h*2).intersects((*it)->getRect()))
						if ((*it)->isShootEnemy == true) { (*it)->isShootEnemy = false; entities.push_back(new Bullet(BulletEnemyImage, "BulletEnemy", lvl, (*it)->x, (*it)->y, 36, 36, p.right)); }
				}
			}*/
			//////////////////////РЕАЛИЗУЕМ ДРУГОЕ ДВИЖЕНИЕ ДЕМЕНТОРА (все равно стреляет спиной)
			if ((*it)->name == "easyEnemy")
			{
				//if (p.state == p.right)// && p.state == p.stay)//Не работает!! все время стреляют вправо!!! и пропал один враг!
				{
					if (((*it)->x - p.x < 300) && ((*it)->x - p.x > 0) && ((*it)->y - p.y > -20) && ((*it)->y - p.y < 40))
					{
						if ((*it)->isShootEnemy == true) { (*it)->isShootEnemy = false; entities.push_back(new Bullet(BulletEnemyImage, "BulletEnemy", lvl, (*it)->x, (*it)->y, 36, 36, p.left)); }
					}
				}
				if ((p.x - (*it)->x < 300) && (p.x - (*it)->x > 0) && ((*it)->y - p.y > -20) && ((*it)->y - p.y < 40))
				{
					
						if ((*it)->isShootEnemy == true) { (*it)->isShootEnemy = false; entities.push_back(new Bullet(BulletEnemyImage, "BulletEnemy", lvl, (*it)->x, (*it)->y, 36, 36, p.right)); }
				}
			}
			////////////////////////////////////////КОНЕЦ ДЕМЕНТОРА
			///////////////////для змеи (новое взаимодействие)////////////
			if ((*it)->name == "snake") //если имя эл-та списка "EasyEnemy" (причём эл-т списка не обязан пересекаться с игроком),
			{

				if (((*it)->x - p.x < 300) && ((*it)->x - p.x > 0) && ((*it)->y - p.y > 0) && ((*it)->y - p.y < 25)) { //и если игрок находится ниже врага и находится на расстояние 400 слева от него, то...
					
					if ((*it)->dx > 0) //{ (*it)->changeD = true; } //если при этом враг движется вправо, changeD = true.
					{
						(*it)->dx = -0.15; //задаём скорость, немного больше обычной влево
						sound.play();
					}
				}
				if ((p.x - (*it)->x < 300) && (p.x - (*it)->x > 0) && ((*it)->y - p.y > 0)&&((*it)->y - p.y <25)) { //аналогично для случая, когда игрок правее врага
					
					if ((*it)->dx < 0) //{ (*it)->changeD = true; }
					{
						(*it)->dx = 0.15;
						sound.play();
					}
				}
				if ((*it)->getRect().intersects(p.getRect()))
				{
					if (p.dy > 0)// && (p.onGround == false)) 
					{
						//(*it)->dx = 0;
						p.dy = -0.3; 

						//(*it)->health = 0;
					}
					if ((*it)->dx > 0) {
						p.kickR = true; p.kickUp = true; //p.onGround = false;
						p.health -= 1;
						sound1.play();
					}
					if ((*it)->dx < 0) {
						p.kickL = true; p.kickUp = true;// p.onGround = false;
						p.health -= 1;
						sound1.play();
					}
				}
			}

			if ((*it)->name == "EnemyBoss") //если имя эл-та списка "EasyEnemy" (причём эл-т списка не обязан пересекаться с игроком),
			{

				if (((*it)->x - p.x < 500) && ((*it)->x - p.x > 0) && ((*it)->y - p.y > -100) && ((*it)->y - p.y < 100)) { //и если игрок находится ниже врага и находится на расстояние 400 слева от него, то...

					if ((*it)->dx > 0) //{ (*it)->changeD = true; } //если при этом враг движется вправо, changeD = true.
					{
						(*it)->dx = -0.25; //задаём скорость, немного больше обычной влево
					
						sound.play();
					}
				}
				if ((p.x - (*it)->x < 500) && (p.x - (*it)->x > 0) && ((*it)->y - p.y > -100) && ((*it)->y - p.y < 100)) { //аналогично для случая, когда игрок правее врага

					if ((*it)->dx < 0) //{ (*it)->changeD = true; }
					{
						(*it)->dx = 0.25;
						
						sound.play();
					}
				}
				if ((*it)->health > 100)
				{
					if ((*it)->getRect().intersects(p.getRect()))
					{
						if (p.dy > 0)// && (p.onGround == false)) 
						{
							//(*it)->dx = 0;
							p.dy = -0.8;

							//(*it)->health = 0;
						}
						if ((*it)->dx > 0) {
							p.kickRB = true; p.kickUpB = true; //p.onGround = false;
							p.health -= 20;
							(*it)->isAttack = true;
							sound1.play();
						}
						if ((*it)->dx < 0) {
							p.kickLB = true; p.kickUpB = true;// p.onGround = false;
							p.health -= 20;
							(*it)->isAttack = true;
							sound1.play();
						}
					}
				}
			}
				/////////////////при виде поблизости игрока (ну скажем в пределах 400 координат) враг должен резко ускориться в его сторону и постараться задавить игрока//////////////////////////
			
				
					
				

			
			///////////////////////////конец нового ///////////////////////////////

			if (((*it)->name == "MovingPlatform") && ((*it)->getRect().intersects(p.getRect())))//если игрок столкнулся с объектом списка и имя этого объекта movingplatform
			{
				Entity* movPlat = *it;
				if ((p.dy > 0) || (p.onGround == false))//при этом игрок находится в состоянии после прыжка, т.е падает вниз
					if (p.y + p.h < movPlat->y + movPlat->h)//если игрок находится выше платформы, т.е это его ноги минимум (тк мы уже проверяли что он столкнулся с платформой)
					{
						p.y = movPlat->y - p.h +1; p.x += movPlat->dx * time; p.dy = 0; p.onGround = true; // то выталкиваем игрока так, чтобы он как бы стоял на платформе
					}
			}
			if (((*it)->name == "MovingPlatform1") && ((*it)->getRect().intersects(p.getRect())))//если игрок столкнулся с объектом списка и имя этого объекта movingplatform
			{
				Entity* movPlat = *it;
				if ((p.dy > 0) || (p.onGround == false))//при этом игрок находится в состоянии после прыжка, т.е падает вниз
					if (p.y + p.h < movPlat->y + movPlat->h)//если игрок находится выше платформы, т.е это его ноги минимум (тк мы уже проверяли что он столкнулся с платформой)
					{
						p.y = movPlat->y - p.h + 1; p.x += movPlat->dx * time; p.dy = 0; p.onGround = true; // то выталкиваем игрока так, чтобы он как бы стоял на платформе
					}
			}


			window.draw((*it)->sprite); //рисуем entities объекты (сейчас это только враги)
		}



		lifeBarPlayer.draw(window);//рисуем полоску здоровья (РАБОТАЕТ)
		//////Тоже для отображения Миссий
		if (!showMissionText) {
			text.setPosition(view.getCenter().x + 125, view.getCenter().y - 130);//позиция всего этого текстового блока
			s_quest.setPosition(view.getCenter().x + 115, view.getCenter().y - 130);//позиция фона для блока			
			window.draw(s_quest); window.draw(text); //рисуем спрайт свитка (фон для текста миссии). а затем и текст. все это завязано на логическую переменную, которая меняет свое состояние от нажатия клавиши ТАБ
		}
		/////////
		//window.draw(B.sprite);
		window.draw(p.sprite);
		window.display();
	}
	return 0;
}
