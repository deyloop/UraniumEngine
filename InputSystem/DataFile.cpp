#include "DataFile.h"
#include <cstdio>

void DataFile::parse( const char* filename ) {
	FILE* file = nullptr;
		fopen_s(&file,filename,"rb");
		if(!file){
			cout << "ERROR"; 
			//fclose(file); 
			return;
		}

		vector<Tag> tags;

		char c = ' ';
		while (c != EOF) {
			c = fgetc(file);
			if (c == '<') {
				Tag tag;
				string tag_conts;
				tag_conts = "<";
				while (c != '>') {
					c = fgetc(file);
					tag_conts += c;
				}
				tag.extract(tag_conts.c_str());
				tags.push_back(tag);
			}
		}
		fclose(file);
		if(tags.size( )!= 0){
			RootNode.compose(tags);
		}
		else cout << "File contains no data\n";

}