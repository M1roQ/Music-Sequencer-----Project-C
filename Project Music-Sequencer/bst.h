#ifndef BST_H
#define BST_H

#include<stdio.h>
#include<stdlib.h>
#include"note_io.h"

//The data for one node of the BST  represents one note from a musical score.
typedef struct BST_Node_Struct{
    double key; //a unique identifier for stored notes in the order in which they occur in the song. 
                //key = (10.0*bar)+index
    double freq;
    int bar;
    double index;

    struct BST_Node_Struct* left;
    struct BST_Node_Struct* right;

} BST_Node;

BST_Node* newBST_Node(double freq, int bar, double index);
BST_Node* BST_insert(BST_Node* root, BST_Node* new_node);
void BST_makePlayList(BST_Node* root);
void delete_BST(BST_Node* root);

#endif // NOTE_IO_H
