int WIDTH = 800, HEIGHT = 600;  //Длинна, ширина
bool PAINT = false, OTLADKA = false, CLICK = false;
char SHADOW_BUTTON = '-';
char BUTTON = '-';

int PAINT_X, PAINT_Y; // нажатие первоначальное
int SCENE = 1; // 1 - первое 2 - второе
int CIRCLES_KOL = 0;

int FLAG_PERIOD = 0;
float X_POLZ =0.5;
short K_SPEED = 1;

double MAIN_TIME = 0, DELTA_TIME, K_TIME; // 