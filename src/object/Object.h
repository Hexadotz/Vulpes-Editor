#pragma once
#include <string>

class Object {
	public:
		int id;
		Object(const std::string& obj_name = "Object"); // constructor
		virtual ~Object(); // the virtue allow for adding bullshit later to the function the ~ is to specify it's a destructor, my head hurts

};