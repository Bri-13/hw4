#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    //std::cout << "Balance pre update: " << this->getKey() << " " << static_cast<int>(this->getBalance()) << std::endl;
    balance_ += diff;
    //std::cout << "Balance post update: " << this->getKey() << " " <<static_cast<int>(this->getBalance()) << std::endl;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateRight(AVLNode<Key,Value>* n1);
    void rotateLeft(AVLNode<Key,Value>* n1);
    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void removeFix(AVLNode<Key,Value>* n, int difference);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == NULL){
      AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
      this->setRoot(static_cast<Node<Key, Value>*>(newNode));
      return;
    }
    
    AVLNode<Key, Value> *curr = static_cast<AVLNode<Key, Value>*>(this->getRoot());
    AVLNode<Key, Value> *parent = NULL;
    
    // find where to insert
    while(curr != NULL){
      parent = curr;
      if(new_item.first < curr->getKey()){
        curr = curr->getLeft();
      }
      else if(new_item.first > curr->getKey()){
        curr = curr->getRight();
      }
      else{
        // overwrites the current value with the updated value
        curr->setValue(new_item.second);
        return;
      }
    }

    //std::cout << "parent: " << parent->getKey() << std::endl;
    // insert into tree
    AVLNode<Key, Value> *newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

    if(new_item.first < parent->getKey()){
      //std::cout << "sets to parent's left child" << std::endl;
      parent->setLeft(newNode);
    }
    else{
      //std::cout << "sets as parent's right child" << std::endl;
      parent->setRight(newNode);
    }

    // update balances and rotate if needed
    insertFix(parent, newNode);

    // std::cout << "Insert " << new_item.first << std::endl;
    // BinarySearchTree<Key,Value>::print();
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n){
  //std::cout << "gets called" << std::endl;
  //std::cout << "Inserting: " << std::endl;
  if(p == NULL){
    return;
  }

  if(n == p->getLeft()){
    p->updateBalance(-1);
  }
  else{
    p->updateBalance(1);
  }

  if(p->getBalance() == 0){
    return;
  }
  AVLNode<Key, Value> *g = p->getParent();
  if(g == NULL){
    return;
  }

  // else if(p->getBalance() == -2 || p->getBalance() == 2){
  //   AVLNode<Key, Value> *g = p->getParent();
  //   if(g == NULL){
  //     return;
  //   }

  //   if(p->getBalance() == -2){
  //     if(n->getBalance() == -1){
  //       rotateRight(g);
  //       p->setBalance(0);
  //       g->setBalance(0);
  //     }
  //     else if(n->getBalance() == 1){
  //       rotateLeft(p);
  //       rotateRight(g);
  //       if(n->getBalance() == -1){
  //         p->setBalance(0);
  //         g->setBalance(1);
  //         n->setBalance(0);
  //       }
  //       else if(n->getBalance() == 0){
  //         p->setBalance(0);
  //         g->setBalance(0);
  //         n->setBalance(0);
  //       }
  //       else{
  //         p->setBalance(-1);
  //         g->setBalance(0);
  //         n->setBalance(0);
  //       }
  //     }
  //   }
  //   else if(p->getBalance() == 2){
  //     if(n->getBalance() == 1){
  //       rotateLeft(g);
  //       p->setBalance(0);
  //       g->setBalance(0);
  //     }
  //     else if(n->getBalance() == -1){
  //       rotateRight(p);
  //       rotateLeft(g);
  //       if(n->getBalance() == 1){
  //         p->setBalance(0);
  //         g->setBalance(-1);
  //         n->setBalance(0);
  //       }
  //       else if(n->getBalance() == 0){
  //         p->setBalance(0);
  //         g->setBalance(0);
  //         n->setBalance(0);
  //       }
  //       else{
  //         p->setBalance(1);
  //         g->setBalance(0);
  //         n->setBalance(0);
  //       }
  //     }
  //   }
  //   return;
  // }
  // AVLNode<Key, Value>* g = p->getParent();
  // if(g != NULL){
  //   insertFix(g, p);
  // }
  
  
  //std::cout << "starts checking" << std::endl;
  //check if p is left child of g first
  if(p == g->getLeft()){
    g->updateBalance(-1);
    // std::cout << g->getBalance() << std::endl;
    if(g->getBalance() == 0){
      return;
    }
    else if(g->getBalance() == -1){
      //std::cout << "here is called" << std::endl;
      insertFix(g, p);
      return;
    }
    else if(g->getBalance() == -2){
      //std::cout << "gets in" << std::endl;
      if(p->getBalance() <= 0){
        // zig-zig case
        //std::cout << "works" << std::endl;
        rotateRight(g);
        p->setBalance(0);
        g->setBalance(0);
      }
      else{
        // zig-zag case
        rotateLeft(p);
        rotateRight(g);
        if(n->getBalance() == -1){
          p->setBalance(0);
          g->setBalance(1);
          n->setBalance(0);
        }
        else if(n->getBalance() == 0){
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
        else{
          p->setBalance(-1);
          g->setBalance(0);
          n->setBalance(0);
        }
      }
    }
  }
  else{
    g->updateBalance(1);
    if(g->getBalance() == 0){
      return;
    }
    else if(g->getBalance() == 1){
      insertFix(g, p);
      return;
    }
    else if(g->getBalance() == 2){
      if(p->getBalance() >= 0){
        // zig-zig case
        rotateLeft(g);
        p->setBalance(0);
        g->setBalance(0);
      }
      else{
        // zig-zag case
        rotateRight(p);
        rotateLeft(g);
        if(n->getBalance() == 1){
          p->setBalance(0);
          g->setBalance(-1);
          n->setBalance(0);
        }
        else if(n->getBalance() == 0){
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
        else{
          p->setBalance(1);
          g->setBalance(0);
          n->setBalance(0);
        }
      }
    }
  }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    // finds node
    AVLNode<Key, Value> *curr = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(curr == NULL){
      // return if not in tree
      return;
    }
    

    AVLNode<Key, Value>* parent = curr->getParent();
    int difference = 0;
    // std::cout << "Curr: " << curr->getKey() << std::endl;
    // std::cout << "LC: " << curr->getLeft()->getKey() << std::endl;
    // std::cout << "RC: " << curr->getRight()->getKey() << std::endl;
    //parent = curr->getParent();

    if(curr->getParent() == NULL){
      // removing root
      // std::cout << "enters here" << std::endl;
      if((curr->getLeft() != NULL) && (curr->getRight() != NULL)){
        // has 2 children
        // std::cout << "correctly" << std::endl;
        AVLNode<Key, Value> *pre = static_cast<AVLNode<Key, Value>*>(this->predecessor(curr));
        nodeSwap(curr, pre);
        
        //std::cout << "Current: " << pre->getKey() << std::endl;
        this->setRoot(static_cast<Node<Key, Value>*>(pre));
      }
      else if(curr->getLeft() != NULL){
          // just has a left child
          // if(curr->getParent() == NULL){
              this->setRoot(static_cast<Node<Key, Value>*>(curr->getLeft()));
              curr->getLeft()->setParent(NULL);
          // }
          // else{
              //nodeSwap(curr, curr->getParent());
              // if(curr == curr->getParent()->getLeft()){
              //     curr->getParent()->setLeft(curr->getLeft());
              // }
              // else{
              //     curr->getParent()->setRight(curr->getLeft());
              // }
              // curr->getLeft()->setParent(curr->getParent());
          // }
      }
      else if(curr->getRight() != NULL){
          // just has a right child
          // if(curr->getParent() == NULL){
              this->setRoot(static_cast<Node<Key, Value>*>(curr->getRight()));
              curr->getRight()->setParent(NULL);
          // }
          // else{
          //     //nodeSwap(curr, curr->getParent());
          //     if(curr == curr->getParent()->getLeft()){
          //         curr->getParent()->setLeft(curr->getRight());
          //     }
          //     else{
          //         curr->getParent()->setRight(curr->getRight());
          //     }
          //     curr->getRight()->setParent(curr->getParent());
          // }
      }
      else{
        // std::cout << "goes here" << std::endl;
        this->setRoot(static_cast<Node<Key, Value>*>(NULL));
      }
      
      if(curr != NULL){
        if(curr->getParent() != NULL && curr->getParent()->getLeft() == curr){
          curr->getParent()->setLeft(NULL);
        }
        else if(curr->getParent() != NULL && curr->getParent()->getRight() == curr){
          curr->getParent()->setRight(NULL);
        }
      }
      delete curr;

      // removeFix(static_cast<AVLNode<Key, Value>*>(this->getRoot()), );
      return;
    }

    //std::cout << "Parent key is " << parent->getKey() << " curr key is " << curr->getKey() << std::endl;
    if(curr->getParent() != NULL){
      //std::cout << "gets here" << std::endl;
      // std::cout << parent->getLeft()->getKey() << std::endl;
      //std::cout << curr->getParent()->getKey() << std::endl;
      if(curr == parent->getLeft()){
        //std::cout << "goes here" << std::endl;
        difference = 1;
      }
      else{
        difference = -1;
      }
    }

    if((curr->getLeft() != NULL) && (curr->getRight() != NULL)){
        // has 2 children
        // std::cout << "correctly" << std::endl;
        AVLNode<Key, Value> *pre = static_cast<AVLNode<Key, Value>*>(this->predecessor(curr));
        nodeSwap(curr, pre);
    }
    if(curr->getLeft() != NULL){
        // just has a left child
        if(curr->getParent() == NULL){
            this->setRoot(static_cast<Node<Key, Value>*>(curr->getLeft()));
            curr->getLeft()->setParent(NULL);
        }
        else{
            //nodeSwap(curr, curr->getParent());
            if(curr == curr->getParent()->getLeft()){
                curr->getParent()->setLeft(curr->getLeft());
            }
            else{
                curr->getParent()->setRight(curr->getLeft());
            }
            curr->getLeft()->setParent(curr->getParent());
        }
    }
    else if(curr->getRight() != NULL){
        // just has a right child
        if(curr->getParent() == NULL){
            this->setRoot(static_cast<Node<Key, Value>*>(curr->getRight()));
            curr->getRight()->setParent(NULL);
        }
        else{
            //nodeSwap(curr, curr->getParent());
            if(curr == curr->getParent()->getLeft()){
                curr->getParent()->setLeft(curr->getRight());
            }
            else{
                curr->getParent()->setRight(curr->getRight());
            }
            curr->getRight()->setParent(curr->getParent());
        }
    }
    else{
        if(parent != NULL){
            if(curr == curr->getParent()->getLeft()){
                curr->getParent()->setLeft(NULL);
            }
            else{
                curr->getParent()->setRight(NULL);
            }
        }
    }

    delete curr;
    curr = NULL;

    // check if balanced and rotate if not until you reach node->parent = root
    //if(parent!= NULL) std::cout << "Passing in " << parent->getKey() << std::endl; 
    removeFix(parent, difference);

    // std::cout << "Removing " << key << std::endl;
    // BinarySearchTree<Key,Value>::print();
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int difference){
  //std::cout << "Difference: " << difference << std::endl;
  if(n == NULL){
    return;
  }
  //int bal = static_cast<int>(n->getBalance());
  //std::cout << "Balance pre update " << bal << std::endl;
  //n->updateBalance(difference);
  //bal = static_cast<int>(n->getBalance());
  //std::cout << "Balance post update " << bal << std::endl;

  // start with difference = 1 case
  if(n->getBalance() + difference <= -2){
    AVLNode<Key, Value>* c = n->getLeft();
    if(c == NULL){
      //std::cout << "goes here" << std::endl;
      return;
    }
    //int bal = static_cast<int>(c->getBalance());
    //std::cout << "LeftChild: " << bal << std::endl;
    
    if(c->getBalance() == -1){
      // zig-zig case
      rotateRight(n);
      n->setBalance(0);
      c->setBalance(0);
      //removeFix(p, ndiff);
    }
    else if(c->getBalance() == 0){
      // zig-zig case
      rotateRight(n);
      n->setBalance(-1);
      c->setBalance(1);
    }
    else if (c->getBalance() == 1){
      // zig-zag case
      AVLNode<Key, Value>* g = c->getRight();
      rotateLeft(c);
      rotateRight(n);
      if(g->getBalance() == 1){
        n->setBalance(0);
        c->setBalance(-1);
        g->setBalance(0);
      }
      else if(g->getBalance() == 0){
        n->setBalance(0);
        c->setBalance(0);
        g->setBalance(0);
      }
      else if(g->getBalance() == -1){
        n->setBalance(1);
        c->setBalance(0);
        g->setBalance(0);
      }
      //removeFix(p, ndiff);
    }
  }
  else if(n->getBalance() + difference >= 2){
    AVLNode<Key, Value>* c = n->getRight();
    
    if(c->getBalance() == 1){
      // zig-zig case
      rotateLeft(n);
      n->setBalance(0);
      c->setBalance(0);
      //removeFix(p, ndiff);
    }
    else if(c->getBalance() == 0){
      // zig-zig case
      rotateLeft(n);
      n->setBalance(1);
      c->setBalance(-1);
    }
    else if (c->getBalance() == -1){
      // zig-zag case
      AVLNode<Key, Value>* g = c->getLeft();
      rotateRight(c);
      rotateLeft(n);
      if(g->getBalance() == -1){
        n->setBalance(0);
        c->setBalance(1);
        g->setBalance(0);
      }
      else if(g->getBalance() == 0){
        n->setBalance(0);
        c->setBalance(0);
        g->setBalance(0);
      }
      else if(g->getBalance() == 1){
        n->setBalance(-1);
        c->setBalance(0);
        g->setBalance(0);
      }
      //removeFix(p, ndiff);
    }
  }
  else if(n->getBalance() + difference == -1){
    n->setBalance(-1);
  }
  else if(n->getBalance() + difference == 0){
    AVLNode<Key, Value>* p = n->getParent();
    n->setBalance(0);
    removeFix(p, 0);
  }
  else if(n->getBalance() + difference == 1){
    n->setBalance(1);
  }

  AVLNode<Key, Value>* p = n->getParent();
  int ndiff = 0;
  if(p != NULL){
    if(n == p->getLeft()){
      ndiff = 1;
    }
    else{
      ndiff = -1;
    }
    removeFix(p, ndiff);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* n1){
  if(n1 == NULL || n1->getLeft() == NULL){
    return;
  }
  AVLNode<Key, Value>* n2 = n1->getLeft();
  AVLNode<Key, Value>* n3 = n1->getParent();

  n1->setLeft(n2->getRight());
  if(n2->getRight() != NULL){
    n2->getRight()->setParent(n1);
  }
  n2->setRight(n1);
  n1->setParent(n2);
  n2->setParent(n3);
  if(n3 == NULL){
    this->setRoot(static_cast<Node<Key, Value>*>(n2));
    n2->setParent(NULL);
  }
  else{
    if(n1 == n3->getLeft()){
      n3->setLeft(n2);
    }
    else{
      n3->setRight(n2);
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* n1){
  if(n1 == NULL || n1->getRight() == NULL){
    return;
  }
  AVLNode<Key, Value>* n2 = n1->getRight();
  AVLNode<Key, Value>* n3 = n1->getParent();

  n1->setRight(n2->getLeft());
  if(n2->getLeft() != NULL){
    n2->getLeft()->setParent(n1);
  }
  n2->setLeft(n1);
  n1->setParent(n2);
  n2->setParent(n3);
  if(n3 == NULL){
    this->setRoot(static_cast<Node<Key, Value>*>(n2));
    n2->setParent(NULL);
  }
  else{
    if(n1 == n3->getRight()){
      n3->setRight(n2);
    }
    else{
      n3->setLeft(n2);
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
