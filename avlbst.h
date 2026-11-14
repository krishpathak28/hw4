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
    balance_ += diff;
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
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void insertFixup(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFixup(AVLNode<Key, Value>* node, int8_t diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
  
  if (this->root_ == NULL) {
    this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
    return;
  }

  AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
  AVLNode<Key, Value>* parent = NULL;

  while (curr != NULL) {
    parent = curr;
    if (new_item.first == curr->getKey()) {
      curr->setValue(new_item.second);
      return;
    } 
    else if (new_item.first < curr->getKey()) {
      curr = curr->getLeft();
    } 
    else {
      curr = curr->getRight();
    }
  }

  AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
  if (new_item.first < parent->getKey()) {
    parent->setLeft(newNode);
    parent->updateBalance(-1);
  } 
  else {
    parent->setRight(newNode);
    parent->updateBalance(1);
  }

  if (parent->getBalance() != 0) {
    insertFixup(parent, newNode);
  }
}


template<class Key, class Value>
void AVLTree<Key, Value>::insertFixup(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node) {
  if (parent == NULL || parent->getParent() == NULL) return;
  
  AVLNode<Key, Value>* grandParent = parent->getParent();
  
  if (parent == grandParent->getLeft()) {
    grandParent->updateBalance(-1);
    if (grandParent->getBalance() == 0) return;
    else if (grandParent->getBalance() == -1) {
      insertFixup(grandParent, parent);
    } 
    else if (grandParent->getBalance() == -2) {
      if (parent->getBalance() == -1) { // Zig-zig (Left-Left)
        rotateRight(grandParent);
        parent->setBalance(0);
        grandParent->setBalance(0);
      } 
      else { // Zig-zag (Left-Right)
        rotateLeft(parent);
        rotateRight(grandParent);
        if (node->getBalance() == -1) {
          parent->setBalance(0);
          grandParent->setBalance(1);
        } 
        else if (node->getBalance() == 0) {
          parent->setBalance(0);
          grandParent->setBalance(0);
        } 
        else {
          parent->setBalance(-1);
          grandParent->setBalance(0);
        }
        node->setBalance(0);
      }
    }
  } 
  else { // Parent is right child
    grandParent->updateBalance(1);
    if (grandParent->getBalance() == 0) return;
    else if (grandParent->getBalance() == 1) {
      insertFixup(grandParent, parent);
    } 
    else if (grandParent->getBalance() == 2) {
      if (parent->getBalance() == 1) { // Zig-zig (Right-Right)
        rotateLeft(grandParent);
        parent->setBalance(0);
        grandParent->setBalance(0);
      } 
      else { // Zig-zag (Right-Left)
        rotateRight(parent);
        rotateLeft(grandParent);
        if (node->getBalance() == 1) {
          parent->setBalance(0);
          grandParent->setBalance(-1);
        } 
        else if (node->getBalance() == 0) {
          parent->setBalance(0);
          grandParent->setBalance(0);
        } 
        else {
          parent->setBalance(1);
          grandParent->setBalance(0);
        }
        node->setBalance(0);
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

  AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
  if (node == NULL) return;

  if (node->getLeft() != NULL && node->getRight() != NULL) {
    AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
    nodeSwap(node, pred);
  }

  AVLNode<Key, Value>* parent = node->getParent();
  int8_t diff = 0;
  if (parent != NULL) {
    if (node == parent->getLeft()) diff = 1;
    else diff = -1;
  }

  AVLNode<Key, Value>* child = (node->getLeft() != NULL) ? node->getLeft() : node->getRight();
  
  if (child != NULL) {
    child->setParent(parent);
  }
  
  if (parent == NULL) {
    this->root_ = child;
  } 
  else if (node == parent->getLeft()) {
    parent->setLeft(child);
  } 
  else {
    parent->setRight(child);
  }

  delete node;
  removeFixup(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFixup(AVLNode<Key, Value>* node, int8_t diff) {
  if (node == NULL) return;
  
  AVLNode<Key, Value>* parent = node->getParent();
  int8_t nextDiff = 0;
  if (parent != NULL) {
    if (node == parent->getLeft()) nextDiff = 1;
    else nextDiff = -1;
  }

  if (diff == -1) {
    if (node->getBalance() + diff == -2) {
      AVLNode<Key, Value>* leftChild = node->getLeft();
      if (leftChild->getBalance() == -1) { // Zig-zig
        rotateRight(node);
        node->setBalance(0);
        leftChild->setBalance(0);
        removeFixup(parent, nextDiff);
      } 
      else if (leftChild->getBalance() == 0) { // Zig-zig
        rotateRight(node);
        node->setBalance(-1);
        leftChild->setBalance(1);
      } 
      else { // Zig-zag
        AVLNode<Key, Value>* grandChild = leftChild->getRight();
        rotateLeft(leftChild);
        rotateRight(node);
        if (grandChild->getBalance() == 1) {
          node->setBalance(0);
          leftChild->setBalance(-1);
        } 
        else if (grandChild->getBalance() == 0) {
          node->setBalance(0);
          leftChild->setBalance(0);
        } 
        else {
          node->setBalance(1);
          leftChild->setBalance(0);
        }
        grandChild->setBalance(0);
        removeFixup(parent, nextDiff);
      }
      } 
      else if (node->getBalance() + diff == -1) {
        node->setBalance(-1);
      } 
      else {
        node->setBalance(0);
        removeFixup(parent, nextDiff);
      }
  } 
  else if (diff == 1) {
    if (node->getBalance() + diff == 2) {
      AVLNode<Key, Value>* rightChild = node->getRight();
      if (rightChild->getBalance() == 1) { // Zig-zig
        rotateLeft(node);
        node->setBalance(0);
        rightChild->setBalance(0);
        removeFixup(parent, nextDiff);
      } 
      else if (rightChild->getBalance() == 0) { // Zig-zig
        rotateLeft(node);
        node->setBalance(1);
        rightChild->setBalance(-1);
      } 
      else { // Zig-zag
        AVLNode<Key, Value>* grandChild = rightChild->getLeft();
        rotateRight(rightChild);
        rotateLeft(node);
        if (grandChild->getBalance() == -1) {
          node->setBalance(0);
          rightChild->setBalance(1);
        } 
        else if (grandChild->getBalance() == 0) {
          node->setBalance(0);
          rightChild->setBalance(0);
        } 
        else {
          node->setBalance(-1);
          rightChild->setBalance(0);
        }
        grandChild->setBalance(0);
        removeFixup(parent, nextDiff);
      }
    } 
    else if (node->getBalance() + diff == 1) {
      node->setBalance(1);
    } 
    else {
      node->setBalance(0);
      removeFixup(parent, nextDiff);
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

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
  AVLNode<Key, Value>* rightChild = node->getRight();
  node->setRight(rightChild->getLeft());
  if (rightChild->getLeft() != NULL) {
    rightChild->getLeft()->setParent(node);
  }
  rightChild->setParent(node->getParent());
  
  if (node->getParent() == NULL) {
    this->root_ = rightChild;
  } 
  else if (node == node->getParent()->getLeft()) {
    node->getParent()->setLeft(rightChild);
  } 
  else {
    node->getParent()->setRight(rightChild);
  }
  
  rightChild->setLeft(node);
  node->setParent(rightChild);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {
  AVLNode<Key, Value>* leftChild = node->getLeft();
  node->setLeft(leftChild->getRight());
  if (leftChild->getRight() != NULL) {
    leftChild->getRight()->setParent(node);
  }
  leftChild->setParent(node->getParent());
  
  if (node->getParent() == NULL) {
    this->root_ = leftChild;
  } 
  else if (node == node->getParent()->getRight()) {
    node->getParent()->setRight(leftChild);
  } 
  else {
    node->getParent()->setLeft(leftChild);
  }
  
  leftChild->setRight(node);
  node->setParent(leftChild);
}

#endif
