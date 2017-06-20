#pragma once
#include "Pos.h"

struct Node;

struct Path
{
	Path(Node* _parent, Node* _child);
	Node*	parent;	//�ڑ����m�[�h
	Node*	child;	//�ڑ���m�[�h
	double	len;	//�O�������W��̋���
};

struct Node
{

	Node(const Pos& _pos);
	bool	isSea;
	Pos		pos;
	Array<Path> paths;

	//�o�H�T���A���S���Y���p
	bool	isInQueue;
	bool	isScaned;
	double	cost;
	Node*	from;
};