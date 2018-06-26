#include "DataNode.h"

DataNode DataNode::Defualt;

void DataNode::compose(vector<Tag>& tags) {
	Tag metag		= tags[0];
	name			= metag.name;
	attributes		= metag.attributes;

	if(metag.type == TAG_OPENING){
		vector<Tag> childtags;
		for (int i = 1; i < tags.size( ) && !(tags[i].name == metag.name && tags[i].type == TAG_CLOSING); ++i) {
			childtags.push_back(tags[i]);
		}
		for (int i = 0; i < childtags.size( ); ++i) {
			Tag tag = childtags[i];
			if(tag.type == TAG_CLOSING) continue;
			DataNode child;
			child.parent = this;
			vector<Tag> chTags; 
			if(tag.type != TAG_SELF_CLOSING){
				for (; !(tag.name == childtags[i].name && childtags[i].type == TAG_CLOSING); ++i) {
					chTags.push_back(childtags[i]);
				}
				chTags.push_back(childtags[i]);
			}else chTags.push_back(tag);
			child.compose(chTags);
			subNodes.push_back(child);
		}
	}
}

Attribute & DataNode::GetAttribute( const char * attrname ) {
	for (auto& attr : attributes) {
		if(attr.name == attrname) return attr;
	}
	//no such attribute found.
	return Attribute::Default;
}

DataNode & DataNode::GetNode( const char * nodename ) {
	for (auto& node : subNodes) {
		if(node.name == nodename) return node;
	}
	//found nothing.
	return Defualt;
}
