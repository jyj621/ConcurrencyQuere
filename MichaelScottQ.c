#include<stdio.h>
#include<Windows.h>
#include<process.h>
#include<time.h>

#define TRUE 1
#define FALSE 0
#define MAX_COUNT 10000
#define EMPTY_VALUE -99

typedef struct Queue_Node {
	int data;
	struct Queue_Node *link;
}QueueN;

typedef struct QueueHeader {
	QueueN *front;
	QueueN *rear;
	CRITICAL_SECTION front_lock;
	CRITICAL_SECTION rear_lock;
}QueueH;

QueueH qHeader = { NULL,NULL };
int lockSelection = TRUE; // TRUE 면 동기화, FALSE 면 비동기화
int isEmpty(QueueH *header);
void enQueue(QueueH *header, int data);
int deQueue(QueueH *header);
DWORD WINAPI ThreadFunc(LPTSTR argv[]);

void Queue_Init(QueueH *header);

int main(DWORD argc, LPTSTR argv[]) {
	int start_time = clock();
	int end_time = 0;
	unsigned int uiThreadID = 0;
	DWORD  ThreadID;
	Queue_Init(&qHeader);//큐 초기화
	//쓰레드 생성
	HANDLE hThread1 = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &ThreadID);
	HANDLE hThread2 = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &ThreadID);
	if (NULL == hThread1 || NULL == hThread2) {
		printf("[ERROR] hThread is NULL..\n");
		return 0;
	}

	WaitForSingleObject(hThread1, INFINITE);//쓰레드가 종료될 때 까지 기다림
	WaitForSingleObject(hThread2, INFINITE);

	DeleteCriticalSection(&qHeader.front_lock); // lock 해체
	DeleteCriticalSection(&qHeader.rear_lock);
	//쓰레드 핸들을 닫는다.
	CloseHandle(hThread1);
	CloseHandle(hThread2);
	if (isEmpty(&qHeader)) {
		printf("Empty\n");
	}
	else {
		printf("Not empty\n");
	}
	end_time = clock();
	printf("총 실행 시간 : %0.3f초\n", (float)(end_time - start_time) / 1000);
	return 0;
}

void Queue_Init(QueueH *header) {
	QueueN *tmp_Node = (QueueN*)malloc(sizeof(QueueN));

	tmp_Node->data = tmp_Node->link = NULL;
	header->front = header->rear = tmp_Node;
	InitializeCriticalSection(&header->front_lock);
	InitializeCriticalSection(&header->rear_lock);
}
DWORD WINAPI ThreadFunc(LPTSTR argv[]) {
	int i = EMPTY_VALUE;
	int data = NULL;
	for (i = 0; i < MAX_COUNT; ++i) {
		enQueue(&qHeader, i);
		data = deQueue(&qHeader);
		if (data == EMPTY_VALUE) {
			printf("empty.. Thread ID: %d\n", GetCurrentThreadId());
		}
	}
	return 0;
}
void enQueue(QueueH *header, int data) {
	QueueN *new_Node = (QueueN*)malloc(sizeof(QueueN));
	new_Node->data = data;//data 삽입
	new_Node->link = NULL;

	if (lockSelection == TRUE) {
		EnterCriticalSection(&header->rear_lock);//lock
	}
	header->rear->link = new_Node;
	header->rear = new_Node;
	
	if (lockSelection == TRUE) {
		LeaveCriticalSection(&header->rear_lock);
	}
	return;
}
int isEmpty(QueueH *header) {
	if (header->front->link == NULL && header->rear->link == NULL) {
		return TRUE;
	}
	return FALSE;
}
int deQueue(QueueH *header) {
	int return_value = EMPTY_VALUE;
	if (lockSelection == TRUE) {
		EnterCriticalSection(&header->front_lock);//lock
	}
	QueueN *del_Node_Ptr = header->front;
	QueueN * new_Head = del_Node_Ptr->link;
	if (new_Head == NULL) {
		return_value = header->front->data;
		if (lockSelection == TRUE) {
			LeaveCriticalSection(&header->front_lock);
		}
		return return_value;
	}
	return_value = new_Head->data;
	header->front = new_Head;
	if (lockSelection == TRUE) {
		LeaveCriticalSection(&header->front_lock);
	}
	free(del_Node_Ptr);
	return return_value;
}