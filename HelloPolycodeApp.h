#include <Polycode.h>
#include "PolycodeView.h"
#include "Polycode3DPhysics.h"
#include <vector>

using namespace Polycode;

#define MOVE_SPEED 4.0
#define MOVE_SPEED_STEP 0.1
#define MAX_MOVE_SPEED 4.0

enum section_type {normal, transition};

struct section {
	section_type type;
	Number height, width, depth;
	Number height1, width1, height2, width2;
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


class HelloPolycodeApp : public EventHandler {
public:
    HelloPolycodeApp(PolycodeView *view);
    ~HelloPolycodeApp();
    bool Update();
    void handleEvent(Event *e);
    
private:
	Core *core;
	ScreenLabel *label;
	ScreenLabel *boost_l;
	ScreenLabel *speed_l;
	CollisionScene *cscene;
	ScenePrimitive *obj;
	Number yspeed;
	Number zspeed;
	Number boost;
	int x_in, y_in, z_in;
	
	std::vector<section> sections;	
};

