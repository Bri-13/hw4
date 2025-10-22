#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <algorithm>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int getHeight(Node* parent){
  if(parent == nullptr){
    return 0;
  }

  int leftHeight = getHeight(parent->left);
  int rightHeight = getHeight(parent->right);

  return max(leftHeight, rightHeight) + 1;
}


bool equalPaths(Node * root)
{
  // base cases
  if(root == nullptr){
    return true;
  }
  if(root->left == nullptr && root->right != nullptr){
    if(root->right->right == nullptr){
      return true;
    }
  }
  if(root->right == nullptr && root->left != nullptr){
    if(root->left->left == nullptr){
      return true;
    }
  }

  // call helper function which is recursive
  int leftHeight = getHeight(root->left);
  int rightHeight = getHeight(root->right);

  if(leftHeight == rightHeight){
    return true;
  }
  return false;
}

