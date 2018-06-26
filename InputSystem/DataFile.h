#pragma once
#ifndef _DATAFILE_H
#define _DATAFILE_H

#include "DataNode.h"

class DataFile {
public:
	void parse( const char* filename );
	
	DataNode RootNode;
};

#endif