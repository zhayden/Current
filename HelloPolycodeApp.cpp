#include "HelloPolycodeApp.h"
#include <iostream>
#include <sstream>

HelloPolycodeApp::HelloPolycodeApp(PolycodeView *view) : EventHandler() {

	core = new SDLCore(view, 640,480,false,false,0,0,90);	  

	CoreServices::getInstance()->getResourceManager()->addArchive("default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);

	screen = new Screen();
	label = new ScreenLabel("Welcome to the Flow", 32);
	screen->addChild(label);

	speed_l = new ScreenLabel("Speed: 0", 28);
	speed_l->setPosition(480,0);
	screen->addChild(speed_l);

	boost_l = new ScreenLabel("Boost: 0", 28);
	boost_l->setPosition(480, 30);
	screen->addChild(boost_l);
	
	cscene = new CollisionScene();

	more_boost = Color(0x00CC00FF);
	less_boost = Color(0x806600FF);
	faster_move = Color(0x9900FFFF);
	slower_move = Color(0xCC2900FF);
	faster_recharge = Color(0xFFFF00FF);
	slower_recharge = Color(0xFF3300FF);

	ScenePrimitive *tmp;
	
	//Create tunnel sections
	sections.push_back(section(10, 10, 20, Vector3(19,0,0)));

	tmp = new ScenePrimitive(ScenePrimitive::TYPE_TORUS, .25, .1, 10,10);
	tmp->setColor(more_boost);
	tmp->setPosition(Vector3(4, 4,4) + sections.back().position);
	tmp->setRoll(90);
	sections.back().obstacles.push_back(tmp);

	tmp = new ScenePrimitive(ScenePrimitive::TYPE_SPHERE, .25, 10,10);
	tmp->setColor(less_boost);
	tmp->setPosition(Vector3(4, 4,-4) + sections.back().position);
	sections.back().obstacles.push_back(tmp);

	tmp = new ScenePrimitive(ScenePrimitive::TYPE_TORUS, .25, .1, 10,10);
	tmp->setColor(faster_move);
	tmp->setPosition(Vector3(4, 0,4) + sections.back().position);
	tmp->setRoll(90);
	sections.back().obstacles.push_back(tmp);

	tmp = new ScenePrimitive(ScenePrimitive::TYPE_SPHERE, .25, 10,10);
	tmp->setColor(slower_move);
	tmp->setPosition(Vector3(4, 0,-4) + sections.back().position);
	sections.back().obstacles.push_back(tmp);

	tmp = new ScenePrimitive(ScenePrimitive::TYPE_TORUS, .25, .1, 10,10);
	tmp->setColor(faster_recharge);
	tmp->setPosition(Vector3(4, -4,4) + sections.back().position);
	tmp->setRoll(90);
	sections.back().obstacles.push_back(tmp);

	tmp = new ScenePrimitive(ScenePrimitive::TYPE_SPHERE, .25, 10,10);
	tmp->setColor(slower_recharge);
	tmp->setPosition(Vector3(4, -4,-4) + sections.back().position);
	sections.back().obstacles.push_back(tmp);	
	
	sections.push_back(section(10, 10, 6, 8, 4, Vector3(31,0,0)));	
	sections.push_back(section(6, 8, 20, Vector3(43,0,0)));

	tmp = new ScenePrimitive(ScenePrimitive::TYPE_BOX, .5,.5,.5);
	tmp->setColor(.9,0,0,1);
	tmp->setPosition(Vector3(0,0,0) + sections.back().position);
	sections.back().enemies.push_back(tmp);

	sections.push_back(section(6, 8, 4, 4, 4, Vector3(55,0,0)));	
	sections.push_back(section(4, 4, 100, Vector3(107,0,0)));

	tmp = new ScenePrimitive(ScenePrimitive::TYPE_TORUS, .25, .1, 10,10);
	tmp->setColor(more_boost);
	tmp->setPosition(Vector3(-40, 1,1) + sections.back().position);
	tmp->setRoll(90);
	sections.back().obstacles.push_back(tmp);

	tmp = new ScenePrimitive(ScenePrimitive::TYPE_TORUS, .25, .1, 10,10);
	tmp->setColor(more_boost);
	tmp->setPosition(Vector3(-35, -1,-1) + sections.back().position);
	tmp->setRoll(90);
	sections.back().obstacles.push_back(tmp);
	
	sections.push_back(section(4, 4, 4, 8, 100, Vector3(207,0,0)));
	sections.push_back(section(4, 8, 8, 4, 500, Vector3(507,0,0)));

	//Add tunnel sections to scene
	for(int i=0; i<sections.size(); ++i){
		for(int j=0; j<sections[i].walls.size(); ++j){
			cscene->addCollisionChild(sections[i].walls[j]);
		}
		for(int j=0; j<sections[i].obstacles.size(); ++j){
			cscene->addCollisionChild(sections[i].obstacles[j]);
		}
		for(int j=0; j<sections[i].enemies.size(); ++j){
			cscene->addCollisionChild(sections[i].enemies[j]);
		}
	}
	
	//player
	obj = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 1,1,1);
	obj->setPosition(Vector3(10,-1,0));
	obj->setColor(.5,0,0,1);
	cscene->addCollisionChild(obj);

	home = 10;
	
	x_in = 0;
	y_in = 0;
	z_in = 0;
	
	yspeed=0;
	zspeed=0;

	boost = MAX_BOOST;

	max_move = MAX_MOVE_SPEED;
	max_boost = MAX_BOOST;
	boost_rate = BOOST_RATE;

	//cameras
	cam1 = new Camera(cscene);
	cam1->setPosition(0,0,0);
	cam1->lookAt(Vector3(10,0,0));

	cam2 = new Camera(cscene);
	cam2->setPosition(0,0,0);
	cam2->lookAt(Vector3(10,-1,0));

	cam3 = new Camera(cscene);
	cam3->setPosition(0,0,0);
	cam3->lookAt(Vector3(10,0,0));

	cscene->setActiveCamera(cam3);
	
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYUP);

	//add instruction msgs
	addmsg("Arrow keys or WASD to move");
	addmsg("Shift & Ctrl to speedup & slow");
	addmsg("Space to restart");
	addmsg("F1-F3 to change camera");
}

void HelloPolycodeApp::handleEvent(Event *e) {
	if(e->getDispatcher() == core->getInput()) {
		InputEvent *inputEvent = (InputEvent*)e;
		
		switch(e->getEventCode()) {
			case InputEvent::EVENT_KEYDOWN:
				switch (inputEvent->keyCode()) {		
					case KEY_UP:
					case KEY_w:
						y_in = 1;
					break;
					case KEY_DOWN:
					case KEY_s:
						y_in = -1;
					break;
					case KEY_LEFT:
					case KEY_a:
						z_in = -1;
					break;
					case KEY_RIGHT:
					case KEY_d:
						z_in = 1;
					break;
					case KEY_F1:
						cscene->setActiveCamera(cam1);
					break;
					case KEY_F2:
						cscene->setActiveCamera(cam2);
					break;
					case KEY_F3:
						cscene->setActiveCamera(cam3);
					break;
					case KEY_LSHIFT:
						x_in = 1;
					break;
					case KEY_LCTRL:
						x_in = -1;
					break;
					case KEY_SPACE:
						obj->setPosition(10,-1,0);
						cam1->setPosition(0,0,0);
						cam2->setPosition(0,0,0);
						home = 10;
						label->setText("Welcome to the Flow");
						boost = MAX_BOOST;
						max_move = MAX_MOVE_SPEED;
						max_boost = MAX_BOOST;
						boost_rate = BOOST_RATE;
						for(int i=0; i<sections.size(); ++i){
							for(int j=0; j<sections[i].obstacles.size(); ++j){
								sections[i].obstacles[j]->enabled = true;
							}
						}
					break;
				}
			break;
			case InputEvent::EVENT_KEYUP:
				switch (inputEvent->key) {
					case KEY_UP:
					case KEY_DOWN:
					case KEY_w:
					case KEY_s:
						y_in = 0;			
					break;					
					case KEY_LEFT:
					case KEY_RIGHT:
					case KEY_a:
					case KEY_d:
						z_in = 0;
					break;
					case KEY_LSHIFT:
					case KEY_LCTRL:
						x_in = 0;
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
	if(sec > 3){
		label->setText("");	//Turn off intro msg after a little while
	}
	if(sec >= sections.size()){
		label->setText("Game Over");	//Turn on end msg when no more sections
		return core->updateAndRender();
	}
	
	Number speed = 100/sections[sec].getArea(pos)*MOVE_SPEED;
	std::stringstream ss;
	ss << "Speed: " << (int)speed;
	speed_l->setText(ss.str());		//output speed

	Number elapsed = core->getElapsed();
	speed *= elapsed;

	home += speed;	//Update home position
	
	//Update speeds
	if(y_in && boost > 0){
		yspeed += y_in*MOVE_SPEED_STEP;
		boost -= BOOST_BURN*elapsed;
		if(yspeed > max_move) yspeed = max_move;
		else if(yspeed < -max_move) yspeed = -max_move;
	}else if(yspeed){
		if(yspeed > 0){
			yspeed -= MOVE_SPEED_STEP;
			if(yspeed < 0) yspeed = 0;
		}else{//yspeed < 0
			yspeed += MOVE_SPEED_STEP;
			if(yspeed > 0) yspeed = 0;
		}
	}
	if(z_in && boost > 0){
		zspeed += z_in*MOVE_SPEED_STEP;
		boost -= BOOST_BURN*elapsed;
		if(zspeed > max_move) zspeed = max_move;
		else if(zspeed < -max_move) zspeed = -max_move;
	}else if(zspeed){
		if(zspeed > 0){
			zspeed -= MOVE_SPEED_STEP;
			if(zspeed < 0) zspeed = 0;
		}else{//zspeed < 0
			zspeed += MOVE_SPEED_STEP;
			if(zspeed > 0) zspeed = 0;
		}
	}
	if(x_in && boost > 0){
		pos.x += x_in*max_move*elapsed;
		boost -= BOOST_BURN*elapsed;
	}

	//recharge boost
	if(boost < max_boost){
		boost += boost_rate*elapsed;
	}
	ss.str("");
	ss << "Boost: " << (int)boost;
	boost_l->setText(ss.str());
	
	//Update position
	pos.y += yspeed*elapsed;
	pos.z += zspeed*elapsed;
	pos.x += speed;
	obj->setPosition(pos);

	//Update enemies
	for(int j=0; j<sections[sec].enemies.size(); ++j){
		Vector3 direction = pos - sections[sec].enemies[j]->getPosition();
		direction.x = 0;
		direction.Normalize();
		direction = direction * MAX_MOVE_SPEED*elapsed / 4;
		sections[sec].enemies[j]->Translate(direction);
	}
	
	//Test collisions
	for(int j=0; j<sections[sec].walls.size(); ++j){	//Loop through walls in section
		CollisionResult res = cscene->testCollision(obj, sections[sec].walls[j]);
		if(res.collided) {
			pos += res.colNormal*res.colDist;
			obj->setPosition(pos);
		}
	}
	for(int j=0; j<sections[sec].obstacles.size(); ++j){
		if(!sections[sec].obstacles[j]->enabled){continue;}
		CollisionResult res = cscene->testCollision(obj, sections[sec].obstacles[j]);
		if(res.collided) {
			ScenePrimitive *ob = sections[sec].obstacles[j];
			if(ob->color == more_boost){
				max_boost += 5;
				ob->enabled = false;
				addmsg("+5 to max boost");
			}else if(ob->color == less_boost){
				max_boost -= 5;
				if(max_boost < 0){max_boost = 0;}
				if(boost > max_boost){boost = max_boost;}
				ob->enabled = false;
				addmsg("-5 to max boost");
			}else if(ob->color == faster_move){
				max_move += 1;
				ob->enabled = false;
				addmsg("+1 to move speed");
			}else if(ob->color == slower_move){
				max_move -= 1;
				if(max_move < 0){max_move = 0;}
				ob->enabled = false;
				addmsg("-1 to move speed");
			}else if(ob->color == faster_recharge){
				boost_rate += 2;
				ob->enabled = false;
				addmsg("+2 to boost recharge");
			}else if(ob->color == slower_recharge){
				boost_rate -= 2;
				if(boost_rate < 0){boost_rate = 0;}
				ob->enabled = false;
				addmsg("-2 to boost recharge");
			}
		}
	}
	for(int j=0; j<sections[sec].enemies.size(); ++j){
		CollisionResult res = cscene->testCollision(obj, sections[sec].enemies[j]);
		if(res.collided){
			obj->setPosition(sections.back().position + Vector3(sections.back().depth,0,0));

		}
	}

	//Return to home x pos (collisions/movement may have thrown it off)
	if(pos.x != home){
		if(pos.x < home){
			pos.x += MOVE_SPEED/4*elapsed;
			if(pos.x > home){
				pos.x = home;
			}
		}else{//obj->getPosition().x > home
			pos.x -= MOVE_SPEED/4*elapsed;
			if(pos.x < home){
				pos.x = home;
			}
		}
		obj->setPosition(pos);
	}

	//Update Cameras
	cam2->lookAt(pos);
	Vector3 direction = obj->getPosition() - cam2->getPosition();
	direction = direction * (MOVE_SPEED/2*elapsed);
	direction.x = speed;
	
	cam2->Translate(direction);
	cam1->Translate(speed,0,0);

	direction = obj->getPosition() * .25;
	direction.x = home-10;
	cam3->setPosition(direction);

	//Update msgs
	for(std::list<msg>::iterator it = msgs.begin(); it != msgs.end(); ++it){
		it->age += elapsed;
		if(it->age > MAX_AGE){
			screen->removeChild(it->label);
			it = msgs.erase(it);
		}
	}
	
    return core->updateAndRender();
}

void HelloPolycodeApp::addmsg(const String & text){
	for(std::list<msg>::iterator it = msgs.begin(); it != msgs.end(); ++it){
		it->label->setPosition(it->label->getScreenPosition()+Vector2(0,-22));
	}

	ScreenLabel *tmp = new ScreenLabel(text, 20);
	tmp->setPosition(0,450);
	screen->addChild(tmp);
	msgs.push_back(msg(tmp));
}

section::section(Number height, Number width, Number depth, Vector3 pos)
	:height(height), width(width), depth(depth), walls(), position(pos){
	type = normal;
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth,.1, width));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth,.1, width));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth, height, .1));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth, height, .1));
	setPosition(pos);
	walls[0]->setColor(0,0,.4,1);
	walls[1]->setColor(0,0,.8,1);
	walls[2]->setColor(0,0,.5,1);
	walls[3]->setColor(0,0,.5,1);
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

	walls[0]->setColor(0,0,.2,1);
	walls[1]->setColor(0,0,.2,1);
	walls[2]->setColor(0,0,.3,1);
	walls[3]->setColor(0,0,.3,1);
}

void section::setPosition(Vector3 pos){
	walls[0]->setPosition(pos+Vector3(0,-height/2,0));
	walls[1]->setPosition(pos+Vector3(0,height/2,0));
	walls[2]->setPosition(pos+Vector3(0,0,-width/2));
	walls[3]->setPosition(pos+Vector3(0,0,width/2));
	for(int i=0; i<obstacles.size(); ++i){
		Vector3 loc = obstacles[i]->getPosition() - position;
		obstacles[i]->setPosition(loc + pos);
	}
	for(int i=0; i<enemies.size(); ++i){
		Vector3 loc = enemies[i]->getPosition() - position;
		enemies[i]->setPosition(loc + pos);
	}
	position = pos;
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

msg::msg(ScreenLabel *label)
	:label(label), age(0){}
