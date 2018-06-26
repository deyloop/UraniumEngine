#pragma once
#ifndef _DATANODE_H_
#define _DATANODE_H_

#include "DataTag.h"

class DataNode {
public:
	void  compose( vector<Tag>& tags);
	Attribute& GetAttribute( const char* attrname );
	DataNode&  GetNode(const char* nodename );

	DataNode* parent;
	vector<DataNode> subNodes;
	vector<Attribute> attributes;
	std::string name;

	static DataNode Defualt;
};

#endif