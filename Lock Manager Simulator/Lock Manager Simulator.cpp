#include <iostream>
#include <string>
#include <queue>
#include <Windows.h>

using namespace std;

#define N 26
#define M 256

enum lock {
	slock,
	ulock,
	xlock,
};

int (*lock)[N] = new int[M][N]; //lock[transactionID][(int) object] == 'S' transactionIDÔÚobjectÓÐSËø
queue <int> *qs[N];
queue <int> *qx[N];

void startTransID(int transactionID) {
	cout << "Transaction started" << endl;
}

bool isSlock(char object) {
	for (int i = 0;i < M;i++) {
		if (lock[i][(int) object] == slock) {
			return true;
			break;
		}
	}
	return false;
}

bool isXlock(char object) {
	for (int i = 0;i < M;i++) {
		if (lock[i][(int) object] == xlock) {
			return true;
			break;
		}
	}
	return false;
}

void slockTransObject(int transactionID, char object) {
	if (isXlock(object)) {
		if (qs[(int)object] == NULL) {
			qs[(int)object] = new queue<int>;
		}
		if (!(*qs[(int)object]).empty()) {
			cout << "Waiting for lock (X-lock held by: ";
			int size = (*qs[(int)object]).size();
			int *temp = new int[size / 2];
			for (int i = 0;i < size;i+=2) {
				cout << (*qs[(int)object]).front() << (*qs[(int)object]).front << " ";
				temp[i/2] = (*qs[(int)object]).pop();
			}
			cout << ")" << endl;
			for (int i = 0;i < size;i += 2) {
				(*qs[(int)object]).push(temp[i / 2]);
			}
		}
		(*qs[(int)object]).push(transactionID);
		(*qs[(int)object]).push(slock);
	}
	else {
		lock[transactionID][(int)object] = slock;
		cout << "Lock granted" << endl;
	}
}

void xlockTransObject(int transactionID, char object) {
	if (lock[transactionID][(int)object] == slock) {
		lock[transactionID][(int)object] = xlock;
		cout << "Upgrade to X-lock granted " << endl;
	}
	else if (isSlock(object)) {
		if (qs[(int)object] == NULL) {
			qs[(int)object] = new queue<int>;
		}
		if (!(*qs[(int)object]).empty()) {
			cout << "Waiting for lock (s-lock held by: ";
			int size = (*qs[(int)object]).size();
			int *temp = new int[size / 2];
			for (int i = 0;i < size;i += 2) {
				cout << (*qs[(int)object]).front() << (*qs[(int)object]).front << " ";
				temp[i / 2] = (*qs[(int)object]).pop();
			}
			cout << ")" << endl;
			for (int i = 0;i < size;i += 2) {
				(*qs[(int)object]).push(temp[i / 2]);
			}
		}
		(*qs[(int)object]).push(transactionID);
		(*qs[(int)object]).push(xlock);
	}
}

int main() 
{
	for (int i = 0;i < M;i++) {
		for (int j = 0;j < N;j++) {
			lock[i][j] = -1;
		}
	}

	string requestType;
	int transactionID;
	char object;

	while (true) {
		cin >> requestType >> transactionID >> object;
		
	}

	system("pause");
	return 0;
}