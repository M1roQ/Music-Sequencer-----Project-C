#define _CRT_SECURE_NO_WARNINGS

#include"bst.h"

//creating and initializing a new BST_Node for a note with the given bar and index and the specified frequency. 
BST_Node *newBST_Node(double freq, int bar, double index){ 
    BST_Node *new_node = NULL;
    new_node = (BST_Node *)calloc(1, sizeof(BST_Node));

    if (new_node != NULL){
        if (index >= 1.0 || index < 0.0){
            fprintf(stderr, "Invalid index entered. Please enter an index between 0.0 and 1.0.");
            exit(EXIT_FAILURE);
        }
        
        new_node->freq = freq;
        new_node->bar = bar;
        new_node->index = index;
        new_node->key = (10.0 * bar) + index;

        new_node->left = NULL;
        new_node->right = NULL;
    }
    return new_node;
}

//Inserting a new node into the BST
BST_Node *BST_insert(BST_Node *root, BST_Node *new_node){
    BST_Node *p = NULL;

    if (root == NULL) return new_node;

    if (new_node->key < root->key) root->left = BST_insert(root->left, new_node);

    else if (new_node->key > root->key) root->right = BST_insert(root->right, new_node);

    else  p = root;
    
    return root;
}

//traversal of the BST to generate an ordered list of notes to be played. 
void BST_makePlayList(BST_Node *root){
    BST_Node *p = NULL;
    p = root;

    if (p != NULL){
        BST_makePlayList(p->left);

        //Each note is added to a linked-list and the play list is then playable using the code in note_io.c 
        playlist_head = playlist_insert(playlist_head, p->freq, p->bar, p->index);

        BST_makePlayList(p->right);
    } 

}

//Deleting the BST and freeing all memory used for nodes in it.
void delete_BST(BST_Node *root){
    BST_Node *p = NULL;
    p = root;

    if (p != NULL){
        delete_BST(p->left);

        delete_BST(p->right);

        free(p);
        p = NULL;
    }
}
