#include <Polycode.h>
#include "PolycodeView.h"
#include "Polycode3DPhysics.h"
#include <vector>
#include <list>

using namespace Polycode;

#define MOVE_SPEED 4.0		//speed when area == 100
#define MOVE_SPEED_STEP 0.1
#define MAX_MOVE_SPEED 4.0

#define MAX_BOOST 20
#define BOOST_RATE 5	//points per second
#define BOOST_BURN 20

#define MAX_AGE 3		//Time a msg should be on screen

#define more_boost Color(0x00CC00FF)
#define less_boost Color(0x806600FF)
#define faster_move Color(0x9900FFFF)
#define slower_move Color(0xCC2900FF)
#define faster_recharge Color(0xFFFF00FF)
#define slower_recharge Color(0xFF3300FF)

enum section_type {normal, transition};

struct section {
	section_type type;
	Number height, width, depth;	//Normal dimensions of section
	Number height1, width1, height2, width2;	//dimentions of ends for transition sections
	Vector3 position;
	std::vector<ScenePrimitive*> walls;
	std::vector<ScenePrimitive*> obstacles;
	std::vector<ScenePrimitive*> enemies;
	
	section(Number height, Number width, Number depth, Vector3 pos=Vector3(0,0,0));
	section(Number height1, Number width1, Number height2, Number width2, Number depth, Vector3 pos=Vector3(0,0,0));
	void setPosition(Vector3 pos);
	void setPosition(Number x, Number y, Number z);
	bool inSection(Vector3 loc);
	Number getArea();
	Number getArea(Vector3 loc);
};

struct msg {
	ScreenLabel *label;
	Number age;

	msg(ScreenLabel *label);
};


class HelloPolycodeApp : public EventHandler {
public:
    HelloPolycodeApp(PolycodeView *view);
    ~HelloPolycodeApp();
    bool Update();
    void handleEvent(Event *e);
    void addmsg(const String & text);
    
private:
	Core *core;
	Screen *screen;
	ScreenLabel *label;
	ScreenLabel *boost_l;
	ScreenLabel *speed_l;
	std::list<msg> msgs;
	CollisionScene *cscene;
	Camera *cam1, *cam2, *cam3, *cam4;
	ScenePrimitive *obj;
	Number home;	//stores current home x-coordinate
	Number yspeed;
	Number zspeed;
	Number boost;
	short int x_in, y_in, z_in;

	Number max_move, max_boost, boost_rate;
	//Color more_boost, less_boost, faster_move, slower_move, faster_recharge, slower_recharge;
	
	std::vector<section> sections;

	bool addObstacle(Color c, Vector3 p);	//adds obstacle to last section in sections
	bool addEnemy(Vector3 p);				//adds enemy to last section in sections
	void addSection(Number length, Number endWidth, Number endHeight);
};

