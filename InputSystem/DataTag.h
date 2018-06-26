#pragma once
#ifndef _DATATAG_H_
#define _DATATAG_H_

#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
using namespace std;

struct Attribute {
	string name;
	string value;
	static Attribute Default;

	operator int( ){return atoi(value.c_str()); }
	operator bool( ){return (value == "true");}
	operator float( ){return atof(value.c_str()); }
	operator char*( ){return const_cast<char*>(value.c_str());}
};

enum TagType {
	TAG_OPENING,
	TAG_CLOSING,
	TAG_SELF_CLOSING
};

struct Tag {
	Tag( ) : type(TAG_OPENING){}
	string name;
	vector<Attribute> attributes;
	TagType type;

	void extract(const char* contents);
};

#endif