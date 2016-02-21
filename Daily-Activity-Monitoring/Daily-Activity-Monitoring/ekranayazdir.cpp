#include "ekranayazdir.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

ekranayazdir::ekranayazdir(int sayi)
{
	hour =sayi;
}

void ekranayazdir::yazdir() {
	cout << hour;
}

int main() {
	int x;
	cin >> x;
	ekranayazdir ey(x);
	ey.yazdir();
	cout << endl;
	cin >> x;
	return 0; 
}