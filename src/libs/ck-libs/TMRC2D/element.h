#ifndef ELEMENT_H
#define ELEMENT_H

#include "ref.h"
#include "refine.decl.h"

// Constants to tell FEM interface whether node is on a boudary between chunks
// and if it is the first of two split operations
#define LOCAL_FIRST 0x2
#define LOCAL_SECOND 0x0
#define BOUND_FIRST 0x3
#define BOUND_SECOND 0x1
 
extern CProxy_chunk mesh;

class element {  // triangular elements defined by three node references,
  // having three edge references to control refinement access to
  // this element, and provide connectivity to adjacent elements

  // targetArea is the area to attempt to refine this element
  // below. It's unset state is -1.0, and this will be detected as a
  // 'no refinement desired' state. currentArea is the actual area of
  // the triangle that was cached most recently.  This gets updated by
  // calls to either getArea or calculateArea.
  double targetArea, currentArea;
  int present;  // indicates this is an element present in the mesh

  /* node and edge numberings follow this convention regardless of 
     orientation:
                       0
                      / \
                    1/   \0
                    /     \
                   2_______1
	               2                              */
 public:
  int nodes[3];
  edgeRef edges[3];
  elemRef myRef;
  chunk *C;

  /// Basic element constructor
  element() {   targetArea = currentArea = -1.0; }
  element(int cid, int idx, chunk *C) { set(); set(cid, idx, C); }
  element(int *n) { set();  set(n); }
  element(int *n, edgeRef *e) { set();  set(n, e); }
  element(int n1, int n2, int n3, edgeRef e1, edgeRef e2, edgeRef e3) {
    set();  set(n1, n2, n3, e1, e2, e3);
  }
  element(int cid, int idx, chunk *C, int n1, int n2, int n3, 
	  edgeRef e1, edgeRef e2, edgeRef e3) {
    set();  set(cid, idx, C);  set(n1, n2, n3, e1, e2, e3);
  }
  /// Copy constructor
  element(const element& e) {
    targetArea = e.targetArea;  currentArea = e.currentArea;
    present = e.present;
    for (int i=0; i<3; i++) {
      nodes[i] = e.nodes[i];  edges[i] = e.edges[i];
    }
    myRef = e.myRef;  C = e.C;
  }
  /// Assignment
  element& operator=(const element& e) { 
    targetArea = e.targetArea;  currentArea = e.currentArea; 
    present = e.present;
    for (int i=0; i<3; i++) { 
      nodes[i] = e.nodes[i];  edges[i] = e.edges[i];
    }
    myRef = e.myRef;  C = e.C;
    return *this; 
  }

  /// Basic set operation
  void set() { targetArea = currentArea = -1.0;  present = 1; } 
  void set(int c, int i, chunk *ck) { set(); myRef.set(c, i); C = ck; }
  void set(int *n) { present = 1; for (int i=0; i<3; i++)  nodes[i] = n[i]; }
  void set(int *n, edgeRef *e) {
    present = 1;
    for (int i=0; i<3; i++) {
      nodes[i] = n[i];
      edges[i] = e[i];
    }
  }
  void set(int n1, int n2, int n3) {
    present = 1;
    nodes[0] = n1;  nodes[1] = n2;  nodes[2] = n3;
  }
  void set(int n1, int n2, int n3, edgeRef e1, edgeRef e2, edgeRef e3) {
    present = 1;
    nodes[0] = n1;  nodes[1] = n2;  nodes[2] = n3;
    edges[0] = e1;  edges[1] = e2;  edges[2] = e3;
  }
  void set(edgeRef& e1, edgeRef& e2, edgeRef& e3) {
    present = 1;  edges[0] = e1;  edges[1] = e2;  edges[2] = e3;
  }
  void set(int idx, edgeRef e) { edges[idx] = e; }

  /// Update an old edgeRef with a new one
  void update(edgeRef& oldval, edgeRef& newval) {
    CkAssert((edges[0]==oldval) || (edges[1]==oldval) || (edges[2]==oldval));
    if (edges[0] == oldval) edges[0] = newval;
    else if (edges[1] == oldval) edges[1] = newval;
    else edges[2] = newval;
  }
  /// Update an old node with a new one
  void update(int oldNode, int newNode) {
    CkAssert((nodes[0]==oldNode)||(nodes[1]==oldNode)||(nodes[2]==oldNode));
    if (nodes[0] == oldNode) nodes[0] = newNode;
    else if (nodes[1] == oldNode) nodes[1] = newNode;
    else if (nodes[2] == oldNode) nodes[2] = newNode;
  }

  /// Given index of a node on this element, return node's index in node array
  int getNode(int nodeIdx) { return nodes[nodeIdx]; }
  /// Given index of an edge on this element, return the edgeRef for the edge
  edgeRef getEdge(int edgeIdx) { return edges[edgeIdx]; }
  /// Given an edgeRef, find its index on this element
  int getEdgeIdx(edgeRef e) {
    CkAssert((e==edges[0]) || (e==edges[1]) || (e==edges[2]));
    if (edges[0] == e) return 0;
    else if (edges[1] == e) return 1;
    else return 2;
  }
  /// Given a node array index, find its index on this element
  int getNodeIdx(int n) {
    CkAssert((n==nodes[0]) || (n==nodes[1]) || (n==nodes[2]));
    if (nodes[0] == n) return 0;
    else if (nodes[1] == n) return 1;
    else return 2;
  }
  /// Given an edge index on this element, get the neighboring elemRef 
  elemRef getElement(int edgeIdx) { return edges[edgeIdx].getNbr(myRef); }
  
  void clear() { present = 0; }
  int isPresent() { return present; }

  // getArea & calculateArea both set currentArea; getArea returns it
  // minimizeTargetArea initializes or minimizes targetArea
  // getTargetArea & getCachedArea provide access to targetArea & currentArea
  double getArea() { calculateArea();  return currentArea; }
  void calculateArea();
  void minimizeTargetArea(double area) {
    if (((targetArea > area) || (targetArea < 0.0))  &&  (area >= 0.0))
      targetArea = area;
  }
  void resetTargetArea(double area) { targetArea = area; }
  void setTargetArea(double area) { 
    if ((area < targetArea) || (targetArea < 0.0)) targetArea = area; }
  double getTargetArea() { return targetArea; }
  double getCachedArea() { return currentArea; }

  void refine();
  void split(int longEdge);
  void coarsen();
  void collapse(int shortEdge);

  int findLongestEdge();
  int findShortestEdge();
  int isLongestEdge(edgeRef& e);

  // Mesh improvement stuff
  //void tweakNodes();
  //node tweak(node n[3], int i);

  void sanityCheck(chunk *c,elemRef shouldRef);
};

#endif
