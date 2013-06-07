#include "HelloPolycodeApp.h"
#include <iostream>
#include <sstream>

HelloPolycodeApp::HelloPolycodeApp(PolycodeView *view) : EventHandler() {

	core = new SDLCore(view, 640,480,false,false,0,0,90);	  

	CoreServices::getInstance()->getResourceManager()->addArchive("default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);

	Screen *screen = new Screen();
	label = new ScreenLabel("Welcome to the Flow", 32);
	screen->addChild(label);
	
	cscene = new CollisionScene();
	
	//Create tunnel sections
	sections.push_back(section(10, 10, 20, Vector3(19,0,0)));
	sections[0].walls[0]->setColor(0,0,.4,1);
	sections[0].walls[1]->setColor(0,0,.8,1);
	sections[0].walls[2]->setColor(0,0,.5,1);
	sections[0].walls[3]->setColor(0,0,.5,1);
	
	sections.push_back(section(10, 10, 6, 8, 4, Vector3(31,0,0)));
	sections[1].walls[0]->setColor(0,0,.2,1);
	sections[1].walls[1]->setColor(0,0,.2,1);
	sections[1].walls[2]->setColor(0,0,.3,1);
	sections[1].walls[3]->setColor(0,0,.3,1);
	
	sections.push_back(section(6, 8, 20, Vector3(43,0,0)));
	sections[2].walls[0]->setColor(0,0,.4,1);
	sections[2].walls[1]->setColor(0,0,.8,1);
	sections[2].walls[2]->setColor(0,0,.5,1);
	sections[2].walls[3]->setColor(0,0,.5,1);
	
	sections.push_back(section(6, 8, 4, 4, 4, Vector3(55,0,0)));
	sections[3].walls[0]->setColor(0,0,.2,1);
	sections[3].walls[1]->setColor(0,0,.2,1);
	sections[3].walls[2]->setColor(0,0,.3,1);
	sections[3].walls[3]->setColor(0,0,.3,1);
	
	sections.push_back(section(4, 4, 100, Vector3(107,0,0)));
	sections[4].walls[0]->setColor(0,0,.4,1);
	sections[4].walls[1]->setColor(0,0,.8,1);
	sections[4].walls[2]->setColor(0,0,.5,1);
	sections[4].walls[3]->setColor(0,0,.5,1);
	
	sections.push_back(section(4, 4, 4, 8, 100, Vector3(207,0,0)));
	sections[5].walls[0]->setColor(0,0,.2,1);
	sections[5].walls[1]->setColor(0,0,.2,1);
	sections[5].walls[2]->setColor(0,0,.3,1);
	sections[5].walls[3]->setColor(0,0,.3,1);

	//Add tunnel sections to scene
	for(int i=0; i<sections.size(); ++i){
		for(int j=0; j<sections[i].walls.size(); ++j){
			cscene->addCollisionChild(sections[i].walls[j]);
		}
	}
	
	//player
	obj = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 1,1,1);
	obj->setPosition(Vector3(10,-1,0));
	obj->setColor(.5,0,0,1);
	cscene->addCollisionChild(obj);
	
	x_in = 0;
	y_in = 0;
	z_in = 0;
	
	yspeed=0;
	zspeed=0;
	
	cscene->getActiveCamera()->setPosition(0,0,0);
	cscene->getActiveCamera()->lookAt(Vector3(10,-1,0));
	
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYUP);
	
}

void HelloPolycodeApp::handleEvent(Event *e) {
	if(e->getDispatcher() == core->getInput()) {
		InputEvent *inputEvent = (InputEvent*)e;
		
		switch(e->getEventCode()) {
			case InputEvent::EVENT_KEYDOWN:
				switch (inputEvent->keyCode()) {		
					case KEY_UP:
						y_in = 1;
					break;
					case KEY_DOWN:
						y_in = -1;
					break;
					case KEY_LEFT:
						z_in = -1;
					break;
					case KEY_RIGHT:
						z_in = 1;
					break;														
				}
			break;
			case InputEvent::EVENT_KEYUP:
				switch (inputEvent->key) {
					case KEY_UP:
					case KEY_DOWN:
						y_in = 0;			
					break;					
					case KEY_LEFT:
					case KEY_RIGHT:
						z_in = 0;
					break;										
				}
			break;			
		}
	}
}

HelloPolycodeApp::~HelloPolycodeApp() {

}

bool HelloPolycodeApp::Update() {
	Vector3 pos = obj->getPosition();
	
	//find section obj is in
	int sec;
	for(sec=0; sec<sections.size(); ++sec){	//Loop through sections
		if(sections[sec].inSection(obj->getPosition())){	//If in section
			break;
		}
	}
	if(sec >= sections.size()){
		sec = 0;
	}
	Number elapsed = core->getElapsed();
	Number speed = 100/sections[sec].getArea(pos)*elapsed*MOVE_SPEED;
	
	//Update speed
	if(y_in){
		yspeed += y_in*MOVE_SPEED_STEP;
		if(yspeed > MAX_MOVE_SPEED) yspeed = MAX_MOVE_SPEED;
		else if(yspeed < -MAX_MOVE_SPEED) yspeed = -MAX_MOVE_SPEED;
	}else if(yspeed){
		if(yspeed > 0){
			yspeed -= MOVE_SPEED_STEP;
			if(yspeed < 0) yspeed = 0;
		}else{//yspeed < 0
			yspeed += MOVE_SPEED_STEP;
			if(yspeed > 0) yspeed = 0;
		}
	}
	if(z_in){
		zspeed += z_in*MOVE_SPEED_STEP;
		if(zspeed > MAX_MOVE_SPEED) zspeed = MAX_MOVE_SPEED;
		else if(zspeed < -MAX_MOVE_SPEED) zspeed = -MAX_MOVE_SPEED;
	}else if(zspeed){
		if(zspeed > 0){
			zspeed -= MOVE_SPEED_STEP;
			if(zspeed < 0) zspeed = 0;
		}else{//zspeed < 0
			zspeed += MOVE_SPEED_STEP;
			if(zspeed > 0) zspeed = 0;
		}
	}
	
	//Update position
	pos.y += yspeed*elapsed;
	pos.z += zspeed*elapsed;
	pos.x += speed;
	obj->setPosition(pos);
	
	//Update Camera
	cscene->getActiveCamera()->lookAt(pos);
	Vector3 direction = obj->getPosition() - cscene->getActiveCamera()->getPosition();
	//direction.Normalize();
	direction = direction * (MOVE_SPEED/2*elapsed);
	direction.x = speed;
	
	cscene->getActiveCamera()->setPosition(cscene->getActiveCamera()->getPosition() + direction);
	
	
	//Test collision
	for(int j=0; j<sections[sec].walls.size(); ++j){	//Loop through walls in section
		CollisionResult res = cscene->testCollision(obj, sections[sec].walls[j]);
		if(res.collided) {
			//obj->setColor(0,.5,0,1);
			pos += res.colNormal*res.colDist;
			obj->setPosition(pos);
		} else {
			//obj->setColor(.5,0,0,1);
		}
	}
	
    return core->updateAndRender();
}

section::section(Number height, Number width, Number depth, Vector3 pos)
	:height(height), width(width), depth(depth), walls(), position(pos){
	type = normal;
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth,.1, width));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth,.1, width));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth, height, .1));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth, height, .1));
	setPosition(pos);
}

section::section(Number height1, Number width1, Number height2, Number width2, Number depth, Vector3 pos)
	:height((height1+height2)/2), width((width1+width2)/2), height1(height1), width1(width1), height2(height2), width2(width2), depth(depth), walls(), position(pos){
	type = transition;
	
	Number h = height1 > height2 ? height1 : height2;
	Number w = width1 > width2 ? width1 : width2;
	Number hx = height1/2 - height2/2;
	Number wx = width1/2 - width2/2;
	Number dw = sqrt(wx*wx + depth*depth);
	Number dh = sqrt(hx*hx + depth*depth);
	
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, dh,.1, w));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, dh,.1, w));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, dw, h, .1));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, dw, h, .1));
	
	Number roll_angle = atan(hx/depth)*180/PI;
	Number yaw_angle = atan(wx/depth)*180/PI;
	
	walls[0]->setRoll(roll_angle);
	walls[1]->setRoll(-roll_angle);
	walls[2]->setYaw(-yaw_angle);
	walls[3]->setYaw(yaw_angle);
	
	setPosition(pos);
}

void section::setPosition(Vector3 pos){
	position = pos;
	walls[0]->setPosition(position+Vector3(0,-height/2,0));
	walls[1]->setPosition(position+Vector3(0,height/2,0));
	walls[2]->setPosition(position+Vector3(0,0,-width/2));
	walls[3]->setPosition(position+Vector3(0,0,width/2));
}

void section::setPosition(Number x, Number y, Number z){
	setPosition(Vector3(x,y,z));
}

bool section::inSection(Vector3 loc){
	return loc.x >= position.x-depth/2 && loc.x <= position.x+depth/2;
}

Number section::getArea(){
		return width*height;
}

Number section::getArea(Vector3 pos){
	if(type == normal)
		return getArea();
	
	Number xx = pos.x - (position.x - depth/2);	//Distance through section
	Number percent = xx/depth;	//Percent of way through section
	
	Number dw = width2 - width1;	//Delta width
	Number dh = height2 - height1;	//Delta height
	
	Number cw = dw*percent + width1;	//Width at pos
	Number ch = ch*percent + height1;	//Height at pos
	
	return cw*ch;
}
