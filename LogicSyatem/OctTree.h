#pragma once
template <class T>
struct Node {
	Node<T>	*tlb,*tlf,*trb,*trf,
			*blb,*blf,*brb,*brf;
	T*		data;
	int		size;
};

template <class T>
class OctTree {
public:
	OctTree (const int size) {
		this->size = size;
		root = nullptr;
	}

	T& add(int x,int y,int z) {
		//assuming the coodrs are within the limit.
		
	}

	T& at (int x,int y,int z) {

	}

private:
	Node<T> *root;
	int size;
};