#pragma once
#include "Pos.h"

struct Node;

struct Path
{
	Path(Node* _parent, Node* _child);
	Node*	parent;	//接続元ノード
	Node*	child;	//接続先ノード
	double	len;	//三次元座標上の距離
};

struct Node
{

	Node(const Pos& _pos);
	bool	isSea;
	Pos		pos;
	Array<Path> paths;

	//経路探索アルゴリズム用
	bool	isInQueue;
	bool	isScaned;
	double	cost;
	Node*	from;
};