#include "DataTag.h"

Attribute Attribute::Default;

void Tag::extract( const char* contents ) {
	if(contents[0] != '<') cout << "Format error\n";
		
		//extract the name
		int i;
		for (i = 1; 
			(contents[i] >= 'A' && contents[i] <= 'Z')||
			(contents[i] >= 'a' && contents[i] <= 'z')||
			(contents[i] >= '0' && contents[i] <= '9')||
			(contents[i] == '_' || contents[i] == '.'|| contents[i] == '/')&&
			contents[i] != 0; 
			++i) {
			if(contents[i] == '/'){
				type = TAG_CLOSING;
				continue;
			}
			name += contents[i];
		}
		
		//now the attributes.
		while (contents[i] != '>' && (contents[i] != '/' && contents[i + 1] != '>')) {
			if(contents[i] == ' ' || contents[i] == '\t') {++i; continue;}
			Attribute attr;
			while (contents[i] != ' ' && contents[i] != '=' && contents[i] != '\t') {
				attr.name += contents[i]; ++i;
			}
			while(contents[i] == ' ' || contents[i] == '\t' || contents[i] == '=' || contents[i] == '\"' )++i;
			while (contents[i] != '\"') {
				attr.value += contents[i];
				++i;
			}
			attributes.push_back(attr);
			++i;
		}
		if(contents[i] == '/' && contents[i + 1] == '>') type = TAG_SELF_CLOSING;

}