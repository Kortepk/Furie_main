#include <SFML/Graphics.hpp>  //Библиотека для отрисовки
#include "set.h"
#include <iostream>
#include <complex>
#include <fstream>
#include <vector>
#include <algorithm>

#define ANTI_ALIASING 8
#define MAX_N 3

using namespace std;
using namespace sf;

Image IMAGE;
Image IMAGE_RAINBOW;
Font MAIN_FONT;
Text TEXT_WHITE_ALPHA;
Text TEXT_BLACK;

fstream fout;

RenderWindow window(VideoMode(WIDTH*1.5, HEIGHT), "Circle paint", Style::Close, ContextSettings(0,0,ANTI_ALIASING));

vector <vector <float>> SHADOW_DOT;
vector <complex <double>> PIXELS_POSITION;
vector <complex <double> > CONSTANTS;

complex <double> TEMP_COMPLEX;
complex <double> imaginary_i(0, 1);
//vector <complex <double> > RADIUS_VECTOR;

CircleShape WHITE_CIRCLE(1, 60);
RectangleShape WHITE_BUTTON(Vector2f(99.f, 50.f));
RectangleShape LINE_POLZ(Vector2f(WIDTH - 266, 2.f));  
RectangleShape VERT_POLZ(Vector2f(10, 30)); 
 
RectangleShape BUTTON_1(Vector2f(23, 23));  

RectangleShape OSI_LINE(Vector2f(260, 2));  

Vertex LINE_DOT[] =
	{
		Vertex(Vector2f(0, 0)),
		Vertex(Vector2f(0, 0))  
	};	
	
Vertex line_red[] =
	{
		Vertex(Vector2f(390.f, 240.f), Color::Red),
		Vertex(Vector2f(470.f, 150.f), Color::Red)  
	};	

Vertex line_pink[] = 
{
	Vertex(Vector2f(390.f, 240.f), Color(255, 186, 232)),
	Vertex(Vector2f(470.f, 150.f), Color(255, 186, 232))  
};	

Vertex vert_stick[] =
	{
		Vertex(Vector2f(WIDTH-2, 0.f), Color(146, 164, 204)),
		Vertex(Vector2f(WIDTH, 0.f), Color(146, 164, 204)),
		Vertex(Vector2f(WIDTH, HEIGHT), Color(146, 164, 204)),
		Vertex(Vector2f(WIDTH-2, HEIGHT), Color(146, 164, 204))  
	};	

Vertex line_t[] ={
	Vertex(Vector2f(WIDTH, 0.f), Color::White),
	Vertex(Vector2f(WIDTH, 600), Color::White)
};

Vertex line_horiz[] =	{
		Vertex(Vector2f(WIDTH, 150.f), Color::Red),
		Vertex(Vector2f(WIDTH+400, 150.f), Color::Red)
};	

Vertex arrow_up[] =
{
	Vertex(Vector2f(WIDTH - 7.5, 0), Color(146, 164, 204)),
	Vertex(Vector2f(WIDTH - 0.5, 0), Color(146, 164, 204)),
	Vertex(Vector2f(WIDTH + 6.5, 0), Color(146, 164, 204)),
};

Vertex arrow_right[] =
{
	Vertex(Vector2f(1185, 0), Color(188, 99, 52)),
	Vertex(Vector2f(1200, 0), Color(188, 99, 52)),
	Vertex(Vector2f(1185, 0), Color(188, 99, 52))
};

CircleShape TRIANGLE_BUT(10, 3);
CircleShape RED_DOTE(2);

void Start(){
	window.setVisible(false);
	
	fout.open("pixel position.txt");
	IMAGE.loadFromFile("data.png");
	Vector2u vect = IMAGE.getSize();
	IMAGE_RAINBOW.create(vect.x, vect.y, Color::White);
	
	MAIN_FONT.loadFromFile("arkhip.ttf");
	
	TEXT_WHITE_ALPHA.setFont(MAIN_FONT);
	TEXT_WHITE_ALPHA.setFillColor(Color(255,255,255,100));
	TEXT_WHITE_ALPHA.setCharacterSize(20);
	
	TEXT_BLACK.setFont(MAIN_FONT);
	TEXT_BLACK.setFillColor(Color::Black);
	TEXT_BLACK.setCharacterSize(20);

	WHITE_CIRCLE.setFillColor(Color(0, 0, 0, 0));  
	WHITE_CIRCLE.setOutlineColor(Color(153, 217, 205, 120)); // 120
	WHITE_CIRCLE.setOutlineThickness(1.f);  
	
	WHITE_BUTTON.setOutlineColor(Color(255, 255, 255, 128));
	WHITE_BUTTON.setOutlineThickness(1.f); 
	
	//LINE_POLZ.setFillColor(Color(15, 180, 140)); 
	LINE_POLZ.setPosition(133, HEIGHT + 25);
	
	VERT_POLZ.setFillColor(Color(153, 217, 205)); 
	
	BUTTON_1.setFillColor(Color(113, 127, 159));
	BUTTON_1.setOutlineColor(Color(146, 164, 204));
	BUTTON_1.setOutlineThickness(2.f); 
	
	TRIANGLE_BUT.setFillColor(Color(128, 93, 116));
	
	RED_DOTE.setFillColor(Color::Red);
	
	OSI_LINE.setFillColor(Color(146, 164, 204));
	OSI_LINE.setPosition(Vector2f(400, 300));
}

Color GetRainbow(int x){
	x %= 1530; //ограничиваем
	
	int layer = x/255; // узнаём какой сектор
	
	x %= 255; // делаем до цветового диапазона
	
	switch(layer){
		case 0:{
			return Color(255, x, 0);
		}
		case 1:{
			return Color(255 - x, 255, 0);
		}
		case 2:{
			return Color(0, 255, x);
		}
		case 3:{
			return Color(0, 255 - x, 255);
		}
		case 4:{
			return Color(x, 0, 255);
		}
		case 5:{
			return Color(255, 0, 255-x);
		}
	}
	
}

void Start_search(int XX, int YY){ // двигаемся направо от x+1
	int x = XX+1, y = YY, next_x = 1, next_y = 0, kol = 0; // относительно текущих коорд
	int bgn = -1, total = -1, save = -1; // положение относительно похода движения, т.е. проходиться по часовой стрелке и проверяется пиксель
	Color clr, clr_blc = Color::Black;
	bool flag = false;
	int wd2 = WIDTH/2, hg2 = HEIGHT/2;
	
	clr = IMAGE.getPixel(XX, YY);
	if(clr != clr_blc){
		return;
	}
	
	fout<<XX<<" "<<YY<<endl;
	if(OTLADKA)
		cout<<XX<<" "<<YY<<endl;
	PIXELS_POSITION.push_back(complex <double>(XX - wd2, YY - hg2));
	
	while(x != XX or y != YY){
		PIXELS_POSITION.push_back(complex <double>(x - wd2, y - hg2));
		fout<<x<<" "<<y<<endl;
		if(OTLADKA){
			cout<<x<<" "<<y<<endl;
			if(kol % 500 == 0){
				IMAGE_RAINBOW.setPixel(x, y, Color(255, 99, 99));
				IMAGE_RAINBOW.saveToFile("Error.png");
			}
		}
		/*if(kol % 500 == 0){
			IMAGE_RAINBOW.setPixel(x, y, Color(255, 99, 99));
			IMAGE_RAINBOW.saveToFile("Error.png");
		}*/
		clr = IMAGE.getPixel(x + next_x, y + next_y);
		if(clr == clr_blc){
			IMAGE_RAINBOW.setPixel(x, y, GetRainbow(kol));
			x += next_x;
			y += next_y;
		}
		else{
			flag = true;
			if(next_x == 1 and next_y == 0)		bgn = 0;
			if(next_x == 1 and next_y == 1)		bgn = 1;
			if(next_x == 0 and next_y == 1)		bgn = 2;
			if(next_x == -1 and next_y == 1)	bgn = 3;
			if(next_x == -1 and next_y == 0)	bgn = 4;
			if(next_x == -1 and next_y == -1)	bgn = 5;
			if(next_x == 0 and next_y == -1)	bgn = 6;
			if(next_x == 1 and next_y == -1)	bgn = 7;
			
			total = (bgn+1)%8;
			flag = true;
			save = -1; // если будем по диоганали - то сохраним

			while(total != bgn and flag){

				if(abs(total-bgn) != 4){
					switch(total){
						case(0):{
							clr = IMAGE.getPixel(x + 1, y);
							if(clr == clr_blc){
								IMAGE_RAINBOW.setPixel(x, y, GetRainbow(kol));
								x += 1;
								next_x = 1;
								next_y = 0;
								flag = false;
							}
							break;
						}
						case(1):{
							clr = IMAGE.getPixel(x + 1, y + 1);
							if(clr == clr_blc and save == -1)
								save = total;

							break;
						}
						case(2):{
							clr = IMAGE.getPixel(x, y + 1);
							if(clr == clr_blc){
								IMAGE_RAINBOW.setPixel(x, y, GetRainbow(kol));
								y += 1;
								next_x = 0;
								next_y = 1;
								flag = false;
							}
							break;
						}
						case(3):{
							clr = IMAGE.getPixel(x - 1, y + 1);
							if(clr == clr_blc and save == -1)
								save = total;

							break;
						}
						case(4):{
							clr = IMAGE.getPixel(x - 1, y);
							if(clr == clr_blc){
								IMAGE_RAINBOW.setPixel(x, y, GetRainbow(kol));
								x -= 1;
								next_x = -1;
								next_y = 0;
								flag = false;
							}
							break;
						}
						case(5):{
							clr = IMAGE.getPixel(x - 1, y - 1);
							if(clr == clr_blc and save == -1)
								save = total;
							break;
						}
						case(6):{
							clr = IMAGE.getPixel(x, y - 1);
							if(clr == clr_blc){
								IMAGE_RAINBOW.setPixel(x, y, GetRainbow(kol));
								y -= 1;
								next_x = 0;
								next_y = -1;
								flag = false;
							}
							break;
						}
						case(7):{
							clr = IMAGE.getPixel(x + 1, y - 1);
							if(clr == clr_blc and save == -1)
								save = total;
							/*clr = IMAGE.getPixel(x + 1, y - 1);
							if(clr == clr_blc){
								IMAGE.setPixel(x, y, GetRainbow(kol));
								x += 1;
								y -= 1;
								next_x = 1;
								next_y = -1;
								flag = false;
							}*/
							break;
						}
					}
				}
				total = (total+1)%8;
			}
			
			if(flag){
				switch(save){
					case(1):{
						clr = IMAGE.getPixel(x + 1, y + 1);
						if(clr == clr_blc){
							IMAGE_RAINBOW.setPixel(x, y, GetRainbow(kol));
							x += 1;
							y += 1;
							next_x = 1;
							next_y = 1;
						}
						break;
					}
					case(3):{
						clr = IMAGE.getPixel(x - 1, y + 1);
						if(clr == clr_blc){
							IMAGE_RAINBOW.setPixel(x, y, GetRainbow(kol));
							x -= 1;
							y += 1;
							next_x = -1;
							next_y = 1;
						}
						break;
					}
					case(5):{
						clr = IMAGE.getPixel(x - 1, y - 1);
						if(clr == clr_blc){
							IMAGE_RAINBOW.setPixel(x, y, GetRainbow(kol));
							x -= 1;
							y -= 1;
							next_x = -1;
							next_y = -1;
							flag = false;
						}
						break;
					}
					case(7):{
						clr = IMAGE.getPixel(x + 1, y - 1);
						if(clr == clr_blc){
							IMAGE_RAINBOW.setPixel(x, y, GetRainbow(kol));
							x += 1;
							y -= 1;
							next_x = 1;
							next_y = -1;
							flag = false;
						}
						break;
					}
					default:{
						cout<<save<<endl;
						cout<<"ERROR\n"<<x<<" "<<y<<"\nfile save to Error.png";
						IMAGE_RAINBOW.setPixel(x, y, Color(255, 99, 99));
						IMAGE_RAINBOW.saveToFile("Error.png");
						return;
						break;
					}
				}	
			}
			//if(abs(total-x) != 4)
		}
		kol++;
	}
	PIXELS_POSITION.push_back(complex <double>(XX - wd2, YY - hg2));
	fout<<x<<" "<<y<<endl;
	
	IMAGE_RAINBOW.saveToFile("result.png");
	
	cout<<"\ncheck result.png and pixel position.txt \ntotal pixel = "<<kol<<endl;
	
	SCENE = 2;
	fout.close();
} 

void Connect_Dot(float x1, float y1, float x2, float y2, Color Cl){	
	LINE_DOT[0].position = Vector2f(x1, y1);
	LINE_DOT[1].position = Vector2f(x2, y2);
	
	LINE_DOT[0].color = Cl;
	LINE_DOT[1].color = Cl;
	
	window.draw(LINE_DOT, 2, Lines);  
}

int Circle_number(int i)
{
  if (i%2) 
  	return 1 + int(i/2); 

  else 
  	return -i/2;
}

void Render(){
	int hg2 = HEIGHT/2, wd2 = WIDTH/2;
	
	window.draw(vert_stick, 4, Quads); // вертикальная
	
	line_horiz[0].position.y = 150;
	line_horiz[1].position.y = 150;
	window.draw(line_horiz, 2, Lines);
	
	line_horiz[0].position.y = 300;
	line_horiz[1].position.y = 300;
	window.draw(line_horiz, 2, Lines);
	
	line_horiz[0].position.y = 450;
	line_horiz[1].position.y = 450;
	window.draw(line_horiz, 2, Lines);
	
	line_horiz[0].position.y = 600;
	line_horiz[1].position.y = 600;
	window.draw(line_horiz, 2, Lines);
	
	arrow_up[0].position.y = 15;
	arrow_up[1].position.y = 0;
	arrow_up[2].position.y = 15;
	window.draw(arrow_up, 3, Triangles);
	
	arrow_up[0].position.y = 315;
	arrow_up[1].position.y = 300;
	arrow_up[2].position.y = 315;
	window.draw(arrow_up, 3, Triangles);
	
	arrow_right[0].position.y = 144;
	arrow_right[1].position.y = 150;
	arrow_right[2].position.y = 156;
	window.draw(arrow_right, 3, Triangles);	
	
	arrow_right[0].position.y = 444;
	arrow_right[1].position.y = 450;
	arrow_right[2].position.y = 456;
	window.draw(arrow_right, 3, Triangles);
	
	OSI_LINE.setRotation(0);
	window.draw(OSI_LINE);	
	
	OSI_LINE.rotate(-90);
	window.draw(OSI_LINE);	
	
	TEXT_WHITE_ALPHA.setString("X");
	TEXT_WHITE_ALPHA.setPosition(WIDTH + 7, 0);
	window.draw(TEXT_WHITE_ALPHA);
	
	TEXT_WHITE_ALPHA.setPosition(680, 300);
	window.draw(TEXT_WHITE_ALPHA);
	
	TEXT_WHITE_ALPHA.setString("Y");
	TEXT_WHITE_ALPHA.setPosition(WIDTH + 7, 300);
	window.draw(TEXT_WHITE_ALPHA);
	
	TEXT_WHITE_ALPHA.setPosition(405, 10);
	window.draw(TEXT_WHITE_ALPHA);
	
	TEXT_WHITE_ALPHA.setString(L"T");
	TEXT_WHITE_ALPHA.setPosition(1175, 120);
	window.draw(TEXT_WHITE_ALPHA);
	
	TEXT_WHITE_ALPHA.move(0, 300);
	window.draw(TEXT_WHITE_ALPHA);
	
	switch(SCENE){
		case 1:{
			TEXT_WHITE_ALPHA.setString("Press enter to go next");
			TEXT_WHITE_ALPHA.setPosition(WIDTH - TEXT_WHITE_ALPHA.getGlobalBounds().width - 5, HEIGHT - TEXT_WHITE_ALPHA.getGlobalBounds().height - 10);
			window.draw(TEXT_WHITE_ALPHA);
			
			int sz = PIXELS_POSITION.size();
			float last_sha_dot_x, last_sha_dot_y, last_i_sz = 0;
			if(sz != 0){
				last_sha_dot_x = PIXELS_POSITION[0].real();
				last_sha_dot_y = PIXELS_POSITION[0].imag();
			}
			//cout<<sz<<endl;
			for(int i=1;i<sz;i++){ //Red
				line_red[0].position = Vector2f(PIXELS_POSITION[i-1].real() + wd2, PIXELS_POSITION[i-1].imag() + hg2);
				line_red[1].position = Vector2f(PIXELS_POSITION[i].real() + wd2, PIXELS_POSITION[i].imag() + hg2);
				window.draw(line_red, 2, Lines);	
				
				Connect_Dot(WIDTH + 400. * i/sz, -PIXELS_POSITION[i].real()*3/8. + 150, WIDTH + 400. * last_i_sz, -last_sha_dot_x*3/8. + 150, Color::White);
				Connect_Dot(WIDTH + 400. * i/sz, PIXELS_POSITION[i].imag()/2. + 450, WIDTH + 400. * last_i_sz, last_sha_dot_y/2. + 450, Color::White);
				last_sha_dot_x = PIXELS_POSITION[i].real();		
				last_sha_dot_y = PIXELS_POSITION[i].imag();
				last_i_sz = float(i)/sz;
			}
				
			if(sz != 0){
				line_pink[0].position = Vector2f(PIXELS_POSITION[0].real() + wd2, PIXELS_POSITION[0].imag() + hg2);
				line_pink[1].position = Vector2f(PIXELS_POSITION[sz-1].real() + wd2, PIXELS_POSITION[sz-1].imag() + hg2);
				window.draw(line_pink, 2, Lines);
			}			
			
			break;
		}
		case 2:{
			float x = 0, y = 0, x1, y1, rad;
			int hg25 = hg2 - 25;
			bool k_flag = true;
						
			short j = 0;
			while(j != K_SPEED){
				x = 0;
				y = 0;
				
				if(MAIN_TIME >= 1){
					MAIN_TIME = 0;
					//cout<<SHADOW_DOT.size()<<"\n";
					if(FLAG_PERIOD == 0)
						FLAG_PERIOD = 1;
				}	
				
				if(K_SPEED > 0){
					j ++;
					MAIN_TIME += DELTA_TIME;
				}
				else{
					k_flag = false;
					j = K_SPEED;
					MAIN_TIME += DELTA_TIME/abs(K_SPEED);
					if(MAIN_TIME - K_TIME >= DELTA_TIME){
						k_flag = true;
						K_TIME = MAIN_TIME;
					}
				}
				
				for(int i = 0; i < CIRCLES_KOL; i++) {	
					TEMP_COMPLEX = CONSTANTS[i] * exp(imaginary_i * MAIN_TIME * double(Circle_number(i)) * 2. * M_PI);
					
					x1 = TEMP_COMPLEX.real();
					y1 = TEMP_COMPLEX.imag();
						
					//cout<<MAIN_TIME<<" "<<i<<") "<<TEMP_COMPLEX<<endl;
					if(j == K_SPEED){
						if(K_SPEED > 0){
							if(i < 500){
								rad = abs(CONSTANTS[i]);
									
								WHITE_CIRCLE.setRadius(rad);
								WHITE_CIRCLE.setPosition(x - rad + wd2, y - rad + hg25);
								window.draw(WHITE_CIRCLE);
								if(i < 100)
									Connect_Dot(x + x1 + wd2, y + y1 + hg25, x + wd2, y + hg25, Color(153, 217, 205, 120));
							}	
						}
						else{
							if(i < 500){
								rad = abs(CONSTANTS[i]);
									
								WHITE_CIRCLE.setRadius(rad);
								WHITE_CIRCLE.setPosition(x - rad + wd2, y - rad + hg25);
								window.draw(WHITE_CIRCLE);
								if(i < 100)
									Connect_Dot(x + x1 + wd2, y + y1 + hg25, x + wd2, y + hg25, Color(153, 217, 205, 120));
							}	
							else
								i = CIRCLES_KOL;
						}					
					}
									
					x += x1;
					y += y1;
				}
					
				if(k_flag and FLAG_PERIOD < 3){
					SHADOW_DOT.push_back(vector <float>());
					SHADOW_DOT[SHADOW_DOT.size()-1].push_back(x);
					SHADOW_DOT[SHADOW_DOT.size()-1].push_back(y);
					if(FLAG_PERIOD >= 1)
						FLAG_PERIOD ++;
				}
			}
		
			float last_sha_dot_x = SHADOW_DOT[0][0], last_sha_dot_y = SHADOW_DOT[0][1];
			float last_i_sz = 0;
			int sz = 1/DELTA_TIME; // размер всех пикселей изобр

			for(int i=1; i + 1 < SHADOW_DOT.size(); i++){
			//	cout<<last_i_sz<<" "<<last_sha_dot_x<<" "<<SHADOW_DOT[i][0]<<"\n";
				Connect_Dot(SHADOW_DOT[i][0] + wd2, SHADOW_DOT[i][1] + hg25, last_sha_dot_x + wd2, last_sha_dot_y + hg25, Color::White); //------------ GetRainbow(i)
				Connect_Dot(WIDTH + 400. * i/sz, -SHADOW_DOT[i][0]*3/8. + 150, WIDTH + 400. * last_i_sz, -last_sha_dot_x*3/8. + 150, Color::White);
				Connect_Dot(WIDTH + 400. * i/sz, SHADOW_DOT[i][1]/2. + 450, WIDTH + 400. * last_i_sz, last_sha_dot_y/2. + 450, Color::White);
				last_sha_dot_x = SHADOW_DOT[i][0];		
				last_sha_dot_y = SHADOW_DOT[i][1];
				last_i_sz = float(i)/sz;
			}
			line_t[0].position.x = WIDTH + MAIN_TIME * 400;
			line_t[1].position.x = WIDTH + MAIN_TIME * 400;
			//window.draw(line_t, 2, Lines);
			
			if(MAIN_TIME/DELTA_TIME != 0){
				int t = round(MAIN_TIME/DELTA_TIME);
				while(t > SHADOW_DOT.size())
					t--;
				
				RED_DOTE.setPosition(Vector2f(line_t[0].position.x-2, -SHADOW_DOT[t-1][0]*3/8. + 148)); // 150 - 2
				//window.draw(RED_DOTE);
				RED_DOTE.setPosition(Vector2f(line_t[0].position.x-2, SHADOW_DOT[t-1][1]/2 + 448)); // 450 - 2
				//window.draw(RED_DOTE);
			}
			
			WHITE_BUTTON.setPosition(0, HEIGHT - 49);
			window.draw(WHITE_BUTTON);
			
			WHITE_BUTTON.move(WIDTH - 100, 0);
			window.draw(WHITE_BUTTON);
			
			VERT_POLZ.setPosition(126 + X_POLZ * (WIDTH - 266), HEIGHT-40);
			window.draw(VERT_POLZ);
			
			window.draw(LINE_POLZ);  //Отрисовка линии ползунка
			
			if(SHADOW_BUTTON == 'P' or BUTTON == 'P'){
				VERT_POLZ.setFillColor(Color(100, 100, 100, 128)); 
				window.draw(VERT_POLZ);
				if(BUTTON == 'P')
					window.draw(VERT_POLZ);
				VERT_POLZ.setFillColor(Color(153, 217, 205)); 
			}
			
			TEXT_BLACK.setString(to_string(CIRCLES_KOL));
			TEXT_BLACK.setPosition(50 - TEXT_BLACK.getGlobalBounds().width/2, HEIGHT - 35);
			window.draw(TEXT_BLACK);
			
			if(K_SPEED > 0)
				TEXT_BLACK.setString(to_string(K_SPEED));
			else
				TEXT_BLACK.setString("1/" + to_string(-K_SPEED));
			TEXT_BLACK.setPosition(WIDTH - 65 - TEXT_BLACK.getGlobalBounds().width/2, HEIGHT - 35);
			window.draw(TEXT_BLACK);
			
			BUTTON_1.setPosition(WIDTH - 25, HEIGHT - 48);
			window.draw(BUTTON_1);
			BUTTON_1.move(0, 24);
			window.draw(BUTTON_1);
			
			//TRIANGLE_BUT.
			TRIANGLE_BUT.setRotation(0.f); 
			TRIANGLE_BUT.setPosition(WIDTH - 23, HEIGHT - 45);
			window.draw(TRIANGLE_BUT);
			
			TRIANGLE_BUT.setRotation(180.f); 
			TRIANGLE_BUT.setPosition(WIDTH - 3, HEIGHT - 5);
			window.draw(TRIANGLE_BUT);
			
			if(SHADOW_BUTTON == 'D' or BUTTON == 'D'){
				BUTTON_1.setFillColor(Color(100, 100, 100, 128)); 
				window.draw(BUTTON_1);
				if(BUTTON == 'D')
					window.draw(BUTTON_1);
				BUTTON_1.setFillColor(Color(113, 127, 159));
			}
			BUTTON_1.move(0, -24);
			
			if(SHADOW_BUTTON == 'U' or BUTTON == 'U'){
				BUTTON_1.setFillColor(Color(100, 100, 100, 128)); 
				window.draw(BUTTON_1);
				if(BUTTON == 'U')
					window.draw(BUTTON_1);
				BUTTON_1.setFillColor(Color(113, 127, 159));
			}
			
			break;
		}
	}
}
/*void Swap_const(int L, int R){
	swap(RADIUS_SKAL[L], RADIUS_SKAL[R]);
	swap(CONSTANTS[L], CONSTANTS[R]);
}

void Qsort_const(int nStart, int nEnd){
	int L, R, c; 
	float X;
	if(nStart >= nEnd) return;
	L = nStart; R = nEnd;
	X = RADIUS_SKAL[(L+R) / 2];
	while(L <= R){
		while(RADIUS_SKAL[L] > X) L++;
		while(RADIUS_SKAL[R] < X) R--;
		if(L <= R){
			Swap_const(L, R);
			L++; R--;
		}
	}
	Qsort_const(nStart, R);
	Qsort_const(L, nEnd);
}*/


void Calc_const(){
	MAIN_TIME = 0;
	CIRCLES_KOL = 5000;
	
	SHADOW_DOT.clear();
	CONSTANTS.clear();
	MAIN_TIME = 0;
	X_POLZ = 0.5;
	K_SPEED = 1;
	FLAG_PERIOD = 0;
		
	for(int i = 0; i <= CIRCLES_KOL; i++) 
		CONSTANTS.push_back((0, 0));
	
	if(PIXELS_POSITION.size() >= 3000)
		DELTA_TIME = 1./(PIXELS_POSITION.size()-1); //(PIXELS_POSITION.size()-1)
	else
		DELTA_TIME = 1./3000.; //(PIXELS_POSITION.size()-1)
			
	for(int i = 0; i <= CIRCLES_KOL; i++){
		//for(MAIN_TIME = 0; MAIN_TIME <= 1; MAIN_TIME += DELTA_TIME) 
		for(MAIN_TIME = DELTA_TIME/2.; MAIN_TIME < 1; MAIN_TIME += DELTA_TIME) 
	       	CONSTANTS[i] +=  PIXELS_POSITION[round(MAIN_TIME * (PIXELS_POSITION.size() - 1))] * exp(double(-Circle_number(i)) * M_PI * 2.*imaginary_i * MAIN_TIME);
		CONSTANTS[i] *= DELTA_TIME;
	}	
	
	//Qsort_const(0, RADIUS_SKAL.size());
	MAIN_TIME = - DELTA_TIME;
	
	//WIDTH = 800;
	//cout<<"RESIZE!"<<endl;
	HEIGHT = 650;
	window.setSize(sf::Vector2u(WIDTH*1.5, HEIGHT));         
	
	window.setView(
	View(
	    Vector2f(WIDTH*3/4, HEIGHT/2.0), // wd * 1.5/20
	    Vector2f(WIDTH*1.5, HEIGHT)
	    )
	);
	
	CIRCLES_KOL = 1000;
}

void Snap_back(){
	double now_time = 0, x = 0, y = 0;
	bool temp_flag = true;
	short flag_per = 0;
	
	if(FLAG_PERIOD != 0)
		temp_flag = false;
	
	SHADOW_DOT.clear();
	
	while(true){	
		x = 0; 
		y = 0;
		if(temp_flag and now_time >= MAIN_TIME)
			break;	
		
		if(now_time >= 1){
			now_time = 0;
			if(flag_per == 0)
				flag_per = 1;
		}	
			
		for(int i = 0; i < CIRCLES_KOL; i++) {				
			TEMP_COMPLEX = CONSTANTS[i] * exp(imaginary_i * now_time * double(Circle_number(i)) * 2. * M_PI);
			//cout<<TEMP_COMPLEX<<" "<<endl;
			x += TEMP_COMPLEX.real();
			y += TEMP_COMPLEX.imag();
		}		
		
		if(flag_per < 3){
			SHADOW_DOT.push_back(vector <float>());
			SHADOW_DOT[SHADOW_DOT.size()-1].push_back(x); 
			SHADOW_DOT[SHADOW_DOT.size()-1].push_back(y);
			if(flag_per >= 1)
				flag_per ++;
		}
		else
			break;
			
		now_time += DELTA_TIME;
	}
}

int main()
{		
	Start();
	
	int x = 300, y = 463;
	char a;
	
	cout<<"1) Get Pixel in data.png\n2) Mini paint in window\n[1] ";
	cin>>a;
	
	int wd2 = WIDTH/2, hg2 = HEIGHT/2;
	
	if(a == '1') // для изобр.
	{
		//x = 385;
		//y = 244;
		a = '0';
		cout<<"Enter x: ";
	//	cin>>x;
		cout<<"\nEnter y: ";
	//	cin>>y;
		cout<<"\ndebug mode:\n[0] ";
	//	cin>>a;
		
		if(a == '1')
			OTLADKA = true;
		Start_search(x, y);
		SCENE == 2;
		
		Calc_const();
	}
	
	window.setVisible(true);
	
	// окно нельзя разворачивать !!! для пэинта
	while (window.isOpen())  //Главный цикл приложения. Выполняется, пока открыто окно
	{	
		//ContextSettings settings;
		Event event;
		while (window.pollEvent(event))  //Если произошло событие
		{
			if (event.type == Event::Closed){  //Если событие - закртыие окна
				fout.close();
				window.close();  //Закрываем окно
			}
			
            switch(SCENE){
				case 1:{ //========================
	            if(event.type == sf::Event::KeyPressed){
	            	if(event.key.code == sf::Keyboard::R){
		            	PIXELS_POSITION.clear();
						BUTTON = '-';
						CLICK = false;
					}
					if(event.key.code == sf::Keyboard::Enter and PIXELS_POSITION.size() > 1){
		            	SCENE = 2;
		            	BUTTON = '-';
						CLICK = false;
						
						//cout<<PIXELS_POSITION[0]<<" "<<PIXELS_POSITION[PIXELS_POSITION.size()-1]<<endl;
						
						Calc_const();
					}
				}
				if (event.type == sf::Event::MouseMoved){
					if(CLICK and BUTTON == 'L'){
						double x = event.mouseMove.x;
					    double y = event.mouseMove.y; 	
					    
					    if((0 <= x and x <= WIDTH) and (0 <= y and y <= HEIGHT))
					    	PIXELS_POSITION.push_back(complex<double>(x - wd2, y - hg2));
						
					}
				}
					// если нажали на кнопку резет - то закрываем и открываем поток (очищаем)
				if ((event.type == Event::MouseButtonPressed or event.type == Event::MouseButtonReleased) and event.mouseButton.button == sf::Mouse::Left)
				{
				    double x, y;	
				    x = event.mouseButton.x;
					y = event.mouseButton.y;
					if(0 <= x and x <= WIDTH)
						CLICK = !CLICK;
					else
						CLICK = false;
					
					//cout<<CLICK<<" LF "<<x<<" "<<y<<endl;
	
					if(BUTTON != 'R'){
						BUTTON = 'L';
						if(CLICK){
							PIXELS_POSITION.clear();
							PAINT_X = x;
							PAINT_Y = y;
							PIXELS_POSITION.push_back(complex<double>(x - wd2, y - hg2));
						}
					}
					// change, no testiung!!!!!!!!!!!!!!!!
					if(!CLICK)
						BUTTON = '-';	
				}//========================
				if (event.type == Event::MouseButtonReleased and event.mouseButton.button == sf::Mouse::Right)
				{
					if(PIXELS_POSITION.size() > 1){
						int sz = PIXELS_POSITION.size() - 1;
				    	double x = PIXELS_POSITION[sz].real(), y = PIXELS_POSITION[sz].imag();	
				    	
						double x0 = PIXELS_POSITION[0].real(), y0 = PIXELS_POSITION[0].imag(), delta;
						short plus;
						
						x0 -= 0.5;
						y0 -= 0.5;
							
						if(abs(x - x0) >= abs(y - y0)){
							delta = (y - y0)/abs(x - x0); 
							plus = (x - x0)/abs(x - x0);
							while(x <= x0 or x >= x0 + 1){
								PIXELS_POSITION.push_back(complex<double>(x, round(y)));
								x -= plus;
								y -= delta;
							}
						}
						else{
							delta = (x - x0)/abs(y - y0); 
							plus = (y - y0)/abs(y - y0);
							while(y <= y0 or y >= y0 + 1){								
								PIXELS_POSITION.push_back(complex<double>(round(x), y));
								y -= plus;
								x -= delta;
							}
						}
						PIXELS_POSITION.push_back(complex<double>(x, y));
					}
				} 
				if(!CLICK)
					BUTTON = '-';
				break;
			} 		
			
			case 2:{ //=====================================
				if(event.type == sf::Event::KeyPressed){
					cout<<event.key.code<<endl;
					if(event.key.code == sf::Keyboard::Enter and PIXELS_POSITION.size() > 1){
			            for(int i = 0; i < PIXELS_POSITION.size(); i++)
							cout<<PIXELS_POSITION[i]<<"\n";
						cout<<"=====================================\n";
					}
	            	if(event.key.code == sf::Keyboard::R){
		            	PIXELS_POSITION.clear();
						BUTTON = '-';
						CLICK = false;
						SCENE = 1;
						
						HEIGHT = 600;
						window.setSize(sf::Vector2u(WIDTH*1.5, HEIGHT));         
						
						window.setView(
						View(
						    Vector2f(WIDTH*3 / 4.0, HEIGHT/2.0), 
						    Vector2f(WIDTH*1.5, HEIGHT)
						    )
						);
					}
				}
				if ((event.type == Event::MouseButtonPressed or event.type == Event::MouseButtonReleased)and event.mouseButton.button == sf::Mouse::Left)
					{
					    int x, y;	
					    x = event.mouseButton.x;
						y = event.mouseButton.y;
						if(0 <= x and x <= WIDTH)
							CLICK = !CLICK;
						else
							CLICK = false;
							
						if(CLICK) {
							if((133 <= x and x <= WIDTH - 133) and (HEIGHT - 50 <= y and y <= HEIGHT))
								BUTTON = 'P';
							if((WIDTH - 30 <= x and x <= WIDTH) and (HEIGHT - 49 <= y and y <= HEIGHT - 25))
								BUTTON = 'U';
							if((WIDTH - 30 <= x and x <= WIDTH) and (HEIGHT - 25 < y and y <= HEIGHT))
								BUTTON = 'D';
						}
						else{
							switch(BUTTON){
								case 'P':{
									if(X_POLZ < 0.5)
									    CIRCLES_KOL = 2000 * X_POLZ;  // 1000/0.5
									else
									    CIRCLES_KOL = 8000 * X_POLZ - 3000;  //1000 + 4000(x-0.5)/0.5
									    
									if(CIRCLES_KOL < 1)
									   	CIRCLES_KOL = 1;
										
									Snap_back();
							    	BUTTON = '-';
									break;
								}
								case 'U':{
									K_SPEED ++;
									if(K_SPEED == -1 or K_SPEED == 0)
										K_SPEED = 1;
									if(K_SPEED > 50)
										K_SPEED = 50;
									break;
								}
								case 'D':{
									K_SPEED --;
									if(K_SPEED == 0)
										K_SPEED = -2;
									if(K_SPEED < -50)
										K_SPEED = -50;
									break;
								}				
							}
							BUTTON = '-';
						}
						
						//cout<<CLICK<<" LF "<<x<<" "<<y<<endl;
					}// 
				
				if (event.type == sf::Event::MouseMoved){
					int x = event.mouseMove.x;
					int y = event.mouseMove.y; 	
					if((0 <= x and x <= WIDTH) and (0 <= y and y <= HEIGHT)){
						if(BUTTON == 'P'){
							 X_POLZ = (x - 133.)/(WIDTH-266);
						    if(X_POLZ > 1)
						    	X_POLZ = 1;
						    if(X_POLZ < 0)
						    	X_POLZ = 0;
						}		
						if((133 <= x and x <= WIDTH - 133) and (HEIGHT - 50 <= y and y <= HEIGHT))
							SHADOW_BUTTON = 'P';	
						else{
							if((WIDTH - 30 <= x and x <= WIDTH) and (HEIGHT - 49 <= y and y <= HEIGHT - 25))
								SHADOW_BUTTON = 'U';	
							else{
								if((WIDTH - 30 <= x and x <= WIDTH) and (HEIGHT - 25 < y and y <= HEIGHT))
									SHADOW_BUTTON = 'D';
								else
									SHADOW_BUTTON = '-';
							}
						}			
					}	
					else {
						BUTTON = '-';
						SHADOW_BUTTON = '-';
					}
				}	
				
				if(event.type == Event::MouseWheelMoved){					
					int x = event.mouseWheel.x, y = event.mouseWheel.y;
					
					if((133 <= x and x <= WIDTH - 133) and (HEIGHT - 50 <= y and y <= HEIGHT)){
						CIRCLES_KOL += event.mouseWheel.delta;
						if(CIRCLES_KOL < 1)
							CIRCLES_KOL = 1;
						if(CIRCLES_KOL > 5000)
							CIRCLES_KOL = 5000;
						
						if(CIRCLES_KOL < 1000)
							X_POLZ = CIRCLES_KOL/2000.;  // 1000/0.5
						else
							X_POLZ = (CIRCLES_KOL + 3000.)/8000.;  //CIRCLES_KOL = 8000 * X_POLZ - 3000// 1000 + 4000(x-0.5)/0.5
									
						Snap_back();
						BUTTON = '-';
					}
					if((WIDTH - 100 <= x and x <= WIDTH) and (HEIGHT - 50 <= y and y <= HEIGHT)){
						K_SPEED += event.mouseWheel.delta;
						if(K_SPEED == 0 or K_SPEED == -1){
							if(event.mouseWheel.delta > 0)
								K_SPEED = 1;
							else
								K_SPEED = -2;
						}
						if(K_SPEED > 50)
							K_SPEED = 50;
						if(K_SPEED < -50)
							K_SPEED = -50;
					}
				}
				break;
				}
			}
		}
				
		window.clear(Color(0, 137, 207)); 
		
		Render();
		
		window.display();
	}
}
