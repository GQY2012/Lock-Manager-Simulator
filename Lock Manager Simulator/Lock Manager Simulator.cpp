#include <iostream>
#include <string>
#include <queue>
#include <cstdio>
#include <Windows.h>

using namespace std;

#define MAX_ObjectNum 26	//最大对象数
#define MAX_TransNum 256	//最大事务数

enum lock {
	slock,	
	ulock,	//u锁未使用
	xlock,
};

typedef struct object {
//	char objectID;
	int lock[MAX_TransNum];	//lock[i]表示事务i的锁类型
	queue<int> q;	//等待object加锁的事务队列
};

int trans[MAX_TransNum];	//trans[i] == 1代表事务i运行
object ob[MAX_ObjectNum];	//ob[i]表示A数据库对象

void startTransID(int transactionID) {
	trans[transactionID] = 1;
	cout << "Transaction started" << endl;
}

bool isTransStart(int transactionID) {
	return (trans[transactionID] == 1)? true:false;
}

int *isSlock(int objectID,bool& flag) {
	int *p = new int[MAX_TransNum];
	for (int i = 0;i < MAX_TransNum;i++)
		p[i] = -1;
	int *h = p;
	for (int i = 0;i < MAX_TransNum;i++) {
		if (ob[objectID].lock[i] == slock) {
			flag = true;
			*p = i;
			p++;
		}
	}
	return h;
}

int *isXlock(int objectID,bool& flag) {
	int *p = new int[MAX_TransNum];
	for (int i = 0;i < MAX_TransNum;i++)
		p[i] = -1;
	int *h = p;
	for (int i = 0;i < MAX_TransNum;i++) {
		if (ob[objectID].lock[i] == xlock) {
			flag = true;
			*p = i;
			p++;
		}
	}
	return h;
}


bool slockTransObject(int transactionID, int objectID) {
	if (!isTransStart(transactionID)) {
		printf("transaction %d not started\n", transactionID);
		return false;
	}

	bool sflag = false, xflag = false;
	int *ps = isSlock(objectID,sflag);
	int *px = isXlock(objectID,xflag);
	if (!xflag) {
		ob[objectID].lock[transactionID] = slock;
		printf("SLock granted to <%d>\n", transactionID);
		return true;
	}
	else {
		ob[objectID].q.push(transactionID);
		ob[objectID].q.push(slock);
		printf("Waiting for lock (X-lock held by: <%d>)\n", px[0]);
		return false;
	}
}

bool xlockTransObject(int transactionID, int objectID) {
	if (!isTransStart(transactionID)) {
		printf("transaction %d not started\n", transactionID);
		return false;
	}

	bool sflag = false, xflag = false;
	int *ps = isSlock(objectID, sflag);
	int *px = isXlock(objectID, xflag);
	if (!xflag && !sflag) {
		ob[objectID].lock[transactionID] = xlock;
		printf("XLock granted to <%d>\n", transactionID);
		return true;
	}
	else if (sflag) {
		if (ps[0] == transactionID && ps[1] == -1) {
			ob[objectID].lock[transactionID] = xlock;
			cout << "Upgrade to X-lock granted" << endl;
			return true;
		}
		else {
			ob[objectID].q.push(transactionID);
			ob[objectID].q.push(xlock);
			cout << "Waiting for lock (S-lock held by:";
			for (int i = 0;i < MAX_TransNum;i++) {
				if (ps[i] == -1)
					break;
				else {
					cout << "<" << ps[i] << ">";
				}
			}
			cout << ")" << endl;
			return false;
		}
	}
	else {
		ob[objectID].q.push(transactionID);
		ob[objectID].q.push(xlock);
		printf("Waiting for lock (X-lock held by: <%d>)\n", px[0]);
		return false;
	}
}

void unlockTransObject(int transactionID, int objectID) {
	if (!isTransStart(transactionID)) {
		printf("transaction %d not started\n", transactionID);
		return;
	}

	bool sflag = false, xflag = false;
	int *ps = isSlock(objectID, sflag);
	int *px = isXlock(objectID, xflag);

	ob[objectID].lock[transactionID] = -1;
	cout << "Lock released" << endl;
	if (!ob[objectID].q.empty()) {
		int transID = ob[objectID].q.front();
		ob[objectID].q.pop();
		int lock = ob[objectID].q.front();
		ob[objectID].q.pop();
		if (lock == slock) {
			slockTransObject(transID, objectID);
		}
		else {
			xlockTransObject(transID, objectID);
		}
		
	}
}

void endTransID(int transactionID) {
	if (!isTransStart(transactionID)) {
		printf("transaction %d not started\n", transactionID);
		return;
	}

	trans[transactionID] = 0;
	printf("Transaction %d ended\n",transactionID);
	for (int i = 0;i < MAX_ObjectNum;i++) {
		int newqsize = 0, qsize;
		qsize = ob[i].q.size();
		int temp[MAX_TransNum];
		for (int j = 0;j < qsize;j += 2) {
			if (ob[i].q.front() == transactionID) {
				ob[i].q.pop();
				ob[i].q.pop();
			}
			else {
				temp[newqsize] = ob[i].q.front();
				ob[i].q.pop();
				temp[newqsize + 1] = ob[i].q.front();
				ob[i].q.pop();
				newqsize += 2;
			}
		}
		for (int j = 0;j < newqsize;j += 2) {
			ob[i].q.push(temp[j]);
			ob[i].q.push(temp[j+1]);
		}
		//end队列中的事务

		
		if (ob[i].lock[transactionID] != -1) {
			if (ob[i].lock[transactionID] == slock) {
				printf("Release S-lock on %c\n", i + 'A');
			}
			else if(ob[i].lock[transactionID] == xlock) {
				printf("Release X-lock on %c\n", i + 'A');
			}
			ob[i].lock[transactionID] = -1;

			if (!ob[i].q.empty()) {
				int transID = ob[i].q.front();
				ob[i].q.pop();
				int lock = ob[i].q.front();
				ob[i].q.pop();
				if (lock == slock) {
					slockTransObject(transID, i);
				}
				else {
					xlockTransObject(transID, i);
				}

			}
		}
	}
}

int main() 
{
	for (int i = 0;i < MAX_ObjectNum;i++) {
		for (int j = 0;j < MAX_TransNum;j++) {
			ob[i].lock[j] = -1;
		}
	}
	memset(trans, 0, MAX_TransNum);
	string requestType;
	int transactionID;
	char object;

	while (true) {
		cin >> requestType >> transactionID >> object;
		int objectID = object - 'A';

		if (requestType == "start") {
			startTransID(transactionID);
		}
		else if (requestType == "slock") {
			slockTransObject(transactionID, objectID);
		}
		else if (requestType == "xlock") {
			xlockTransObject(transactionID, objectID);
		}
		else if (requestType == "unlock") {
			unlockTransObject(transactionID, objectID);
		}
		else if (requestType == "end") {
			endTransID(transactionID);
		}
		else if (requestType == "quit") {
			break;
		}
		else {

		}
	}

	system("pause");
	return 0;
}