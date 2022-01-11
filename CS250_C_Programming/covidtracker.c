#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct TreeNode {
    char name[20];
    struct TreeNode* right;
    struct TreeNode* left;
};

void preOrder(struct TreeNode* t) {
    if (t == NULL) {
        return;
    }
    printf("%s\n", t->name);
    
    preOrder(t->left);
    preOrder(t->right);
}

void freeAll(struct TreeNode* t) {
    if (t == NULL) {
        return;
    }

    struct TreeNode* tLeft = NULL;
    struct TreeNode* tRight = NULL;

    tLeft = t->left;
    tRight = t->right;

    free(t);
    if (tLeft != NULL) {
        freeAll(tLeft);
    }
    if (tRight != NULL) {
        freeAll(tRight);
    }
}

struct TreeNode* search(struct TreeNode* t, char* goalNode);

int main(int argc, char* argv[]) {
    char patient0Name[20] = "BlueDevil";
    char name1[20];
    char name2[20];
    
    FILE* f = fopen(argv[1], "r+");

    struct TreeNode* patient0 = (struct TreeNode*) malloc(sizeof(TreeNode));
    
    strcpy(patient0->name, patient0Name);
    patient0->left = NULL;
    patient0->right = NULL;

    int index = 0;

    while (1) {

        fscanf(f, "%s", name1);
        if (strcmp(name1, "DONE") == 0) {
            break;
        }
        fscanf(f, "%s", name2);

        struct TreeNode* newPatient = (struct TreeNode*) malloc(sizeof(TreeNode));
        strcpy(newPatient->name, name1);
        newPatient->left = NULL;
        newPatient->right = NULL;

        struct TreeNode* parent = search(patient0, name2);

        if (parent->left == NULL) {
            parent->left = newPatient;
        }
        else {
            int compare = strcmp(parent->left->name, name1);
            if (compare < 0) {
                parent->right = newPatient;
            }
            if (compare > 0) {
                struct TreeNode* temp = parent->left;
                parent->left = newPatient;
                parent->right = temp;
            }
        }
        index++;

    }
    fclose(f);
    preOrder(patient0);
    freeAll(patient0);

    return 0;
}

struct TreeNode* search(struct TreeNode* t, char* goalNode) {
    if (t == NULL) {
        return NULL;
    }
    if (strcmp(t->name, goalNode) == 0) {
        return t;
    }
    struct TreeNode* temp = search(t->left, goalNode);
    if (temp != NULL) {
        return temp;
    }
    return search(t->right, goalNode);
}

  