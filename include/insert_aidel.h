#ifndef _INSERT_AIDEL_H_
#define _INSERT_AIDEL_H_


#include <iostream>
#include <utility>
#include <immintrin.h>

#include "search.h"

#define MAXSLOTS 16

using namespace std;


template<typename dataType> struct insertNode {
	dataType data[MAXSLOTS];
	int slotuse;
};
typedef	insertNode<int> iinsertNode;
typedef	insertNode<long> linsertNode;

template<typename nodeType, typename dataType> struct insertRoot {
	bool flag;
	dataType data[MAXSLOTS];
	int slotuse;
	nodeType *child[MAXSLOTS];
};
typedef insertRoot<iinsertNode, int> iinsertRoot;
typedef insertRoot<linsertNode, long> linsertRoot;

template<typename nodeType, typename indexType> nodeType* process_insertRoot(std::vector<indexType> index)
{
	nodeType *p = (nodeType *)malloc(sizeof(nodeType) * index.size());
	for (long unsigned int i = 0; i < index.size(); i++){
		p[i].flag = true;
		p[i].slotuse = 0;
		for (int j=0; j<MAXSLOTS; j++){
			p[i].data[j] = INT_MAX;
			p[i].child[j] = NULL;
		} 
	}
 
	return p;
}

template<typename nodeType, typename dataType> int insertKey(nodeType *node, dataType key) {
	nodeType* root = node;
	if(root->slotuse>=MAXSLOTS) {
		//printf("The node is FULL\n");
		return -1;
	}
	int pos = linear_search_avx(root->data, MAXSLOTS, key);
	if(root->data[pos] == key) return -1;

	for (int i=root->slotuse; i > pos; i--)
		root->data[i] = root->data[i-1];
	root->data[pos] = key;
	root->slotuse++;

	return 1;
}

template<typename nodeType, typename dataType> int findKey(nodeType *node, dataType key) {
	nodeType* root = node;
	int pos = linear_search_avx(root->data, MAXSLOTS, key);
	return root->data[pos] == key? root->data[pos] : 0;
}



// ========================= insert using level bins ==================================
template<typename nodeType> nodeType* new_inertNode(){
	nodeType *p = (nodeType *)malloc(sizeof(nodeType));
	p->slotuse=0;
	for (int i = 0; i < MAXSLOTS; i++){
		p->data[i] = INT_MAX;
	}
	return p;
}


template<typename nodeType, typename dataType> int insertKey_level(nodeType *node, dataType key) {
	nodeType* root = node;
	int pos = linear_search_avx(root->data, MAXSLOTS, key);


	// in the root?
	if(pos < root->slotuse && root->data[pos] == key)  return -1;

	// can be inserted to previous node?
	if(pos>0 && root->child[pos-1]->slotuse < MAXSLOTS-1){
		iinsertNode* pre_child = root->child[pos-1];	

		if(pos < root->slotuse && root->child[pos]->data[0] < key){
			iinsertNode* child = root->child[pos];
			int child_pos = linear_search_avx(child->data, MAXSLOTS, key);
			if(child->data[child_pos]==key) return -1;

			
			pre_child->data[pre_child->slotuse] = root->data[pos-1];
			pre_child->slotuse++;
			root->data[pos-1] = child->data[0];
			
			for(int i=0; i<child_pos-1; i++){
				child->data[i] = child->data[i+1];
			}
			child->data[child_pos-1] = key;
		}
		else {
			pre_child->data[pre_child->slotuse] = root->data[pos-1];
			pre_child->slotuse++;
			root->data[pos-1] = key;
		}
		return 1;

	}

	//current node exit?
	if(pos>=root->slotuse){
		if(root->slotuse >= MAXSLOTS){
			printf("This node is FULL!\n");
			exit(1);
			return -1;
		}
		root->data[root->slotuse] = key;
		root->child[root->slotuse] = new_inertNode<iinsertNode>();
		root->slotuse++;
		return 1;
	}

	// insert current node
	iinsertNode* child= root->child[pos];

	// the child is FULL?
	if(child->slotuse >= MAXSLOTS){
		if(root->slotuse >= MAXSLOTS) {
			printf("This node is FULL!\n");
			printf("root: key:%d, pos:%d\n", key, pos);
			for(int i=0; i<root->slotuse;i++){
				printf("%d ", root->data[i]);
			}
			printf("\n");
			printf("child:\n");
			for(int i=0; i<root->slotuse;i++){
				printf("%d ", root->data[i]);
			}
			printf("child\n");
			exit(1);
			return -1;
		}
		// move the data in root and child
		for(int i=root->slotuse; i>pos+1; i--){
			root->data[i] = root->data[i-1];
			root->child[i] = root->child[i-1];
		}
		root->data[pos+1]=root->data[pos];
		root->child[pos+1]=new_inertNode<iinsertNode>();
		root->slotuse++;

		iinsertNode* child_1 = root->child[pos+1];
		root->data[pos] = child->data[MAXSLOTS/2];
		for(int i=0; i<MAXSLOTS/2-1; i++ ){
			child_1->data[i] = child->data[MAXSLOTS/2+1+i];
			child->data[MAXSLOTS/2+1+i]=INT_MAX;
		}
		child->slotuse = MAXSLOTS/2;
		child_1->slotuse = MAXSLOTS/2-1;

		return insertKey_level(node, key);
	}

	//this node is not full --> direct insert
	int child_pos = linear_search_avx(child->data, MAXSLOTS, key);
	if(child->data[child_pos]==key) return -1;
	
	for (int i=child->slotuse; i > child_pos; i--)
		child->data[i] = child->data[i-1];
	child->data[child_pos] = key;
	child->slotuse++;

	return 1;
}


template<typename nodeType, typename dataType> int findKey_level(nodeType *node, dataType key) {
	nodeType* root = node;
	int pos = linear_search_avx(root->data, MAXSLOTS, key);
	if (pos >= root->slotuse) return 0;
	if (root->data[pos] == key) return root->data[pos];

	iinsertNode* child= root->child[pos];
	int child_pos = linear_search_avx(child->data, MAXSLOTS, key);
	return (child->data[child_pos]==key)? child->data[child_pos]:0;
}






//======================== insert long key =========================

template<typename nodeType, typename indexType> nodeType* process_insertRoot_long(std::vector<indexType> index)
{
	nodeType *p = (nodeType *)malloc(sizeof(nodeType) * index.size());
	for (long unsigned int i = 0; i < index.size(); i++){
		p[i].flag = true;
		p[i].slotuse = 0;
		for (int j=0; j<MAXSLOTS; j++){
			p[i].data[j] = LONG_MAX;
			p[i].child[j] = NULL;
		}
	}
 
	return p;
}


template<typename nodeType, typename dataType> int insertKey_long(nodeType *node, dataType key) {
	nodeType* root = node;
	if(root->slotuse>=MAXSLOTS) {
		printf("The node is FULL\n");
		return -1;
	}
	int pos = linear_search_avx_long(root->data, MAXSLOTS, key);
	if(root->data[pos] == key) return -1;

	for (int i=root->slotuse; i > pos; i--)
		root->data[i] = root->data[i-1];
	root->data[pos] = key;
	root->slotuse++;

	return 1;
}

template<typename nodeType, typename dataType> dataType findKey_long(nodeType *node, dataType key) {
	nodeType* root = node;
	int pos = linear_search_avx_long(root->data, MAXSLOTS, key);
	return root->data[pos] == key? root->data[pos] : 0;
}



#endif