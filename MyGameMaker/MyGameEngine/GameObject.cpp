#include "GameObject.h"
#include<iostream> //In .h should not be opengl, only in c++ as iostrem
using namespace std;



void GameObject::paint() 
{
	cout << "Hello World from paint function" << endl;
}
