#include <string>
#include <fstream>
#include "Params.h"
#include "Procedure.h"
#include "Event.h"
#include "LifeForm.h"
#include "QuadTree.h"
#include "ObjInfo.h"

using namespace std;

bool LifeForm::testMode = false;

/* Test LifeForms, created by LifeForm::create_life when executed in test mode.
 * These LifeForms are for evaluating the general correctness of the simulator */

fstream fout("testResults.txt", fstream::trunc | fstream::out);
int totalPoints = 0;
int score = 0;

void logTest(bool passed, string message, int points) {
	fout << message << " ";
	if (passed) {
		fout << "PASSED for " << points << " points\n";
		score += points;
	} else {
		fout << "FAILED for " << -points << " points\n";
	}
	totalPoints += points;
}


void checkTest3(void) {
}

void LifeForm::place(LifeForm* obj, Point p) {
	obj->start_point = p;
	obj->pos = p;
	space.insert(obj, obj->pos,
				make_procedure(*obj, &LifeForm::region_resize));
	(void) new Event(age_frequency, make_procedure(*obj, &LifeForm::age));
	obj->is_alive = true;
}

void LifeForm::runTests(void) {
	//LifeForm* obj = new Test1;
	//place(obj, Point(100.0, 100.0));
	//obj = new Test2;
	//place(obj, Point(90.0, 100.0));

	//obj = new Test3a;
	//place(obj, Point(50.0,50.0));
	//obj = new Test3b;
	//place(obj, Point(150.0,50.0));
	//new Event(10000, make_procedure(checkTest3));
}