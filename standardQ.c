#include<stdio.h>
#include<Windows.h>
#include<process.h>
#include<time.h>

#define TRUE 1
#define FALSE 0
#define MAX_COUNT 10000

typedef struct Queue_Node {
	int data;
	struct Queue_Node *link;
}QueueN;

typedef struct QueueHeader {
	QueueN *front;
	QueueN *rear;
}QueueH;

QueueH qHeader = { NULL,NULL }; 
CRITICAL_SECTION lock;
int lockSelection = TRUE; // TRUE 면 동기화, FALSE 면 비동기화

void enQueue(QueueH *header, int data);
int deQueue(QueueH *header);
DWORD WINAPI ThreadFunc(LPTSTR argv[]);
int isEmpty(QueueH *header);

int main(DWORD argc, LPTSTR argv[]) {
	int start_time = clock();
	int end_time = 0;
	unsigned int uiThreadID = 0;
	//크리티컬 섹션 초기화
	InitializeCriticalSection(&lock);
	DWORD  ThreadID;
	//쓰레드 생성

	HANDLE hThread1 = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &ThreadID);
	HANDLE hThread2 = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &ThreadID);
	if (NULL == hThread1 || NULL == hThread2) {
		printf("[ERROR] hThread is NULL..\n");
		return 0;
	}
	
	WaitForSingleObject(hThread1, INFINITE);//쓰레드가 종료될 때 까지 기다림
	WaitForSingleObject(hThread2, INFINITE);
	DeleteCriticalSection(&lock);// lock 해체
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
	printf("총 실행 시간 : %0.3f초\n", (float)(end_time - start_time)/1000);
	return 0;
}


int isEmpty(QueueH *header) {
	if (header->front == NULL && header->rear == NULL) {
		return TRUE;
	}
	return FALSE;
}
DWORD WINAPI ThreadFunc(LPTSTR argv[]) {
	int i = 0;
	int data = NULL;
	for (i = 0; i < MAX_COUNT; ++i) {
		enQueue(&qHeader, i);
		data = deQueue(&qHeader);
		if (data == NULL) {
			printf("empty.. Thread ID: %d\n",GetCurrentThreadId());
		}
	}
	return 0;
}
void enQueue(QueueH *header, int data) {
	QueueN *new_Node = (QueueN*)malloc(sizeof(QueueN));

	new_Node->data = data;//data 삽입
	new_Node->link = NULL;

	if (lockSelection == TRUE) {
		EnterCriticalSection(&lock);//lock
	}

	if (header->front == NULL) {//empty
		header->front = new_Node;
	}
	else {
		header->rear->link = new_Node;
	}

	header->rear = new_Node;

	if (lockSelection == TRUE) {
		LeaveCriticalSection(&lock);
	}
	return;
}
int deQueue(QueueH *header) {
	int return_value = NULL;
	if (lockSelection == TRUE) {
		EnterCriticalSection(&lock);//lock
	}
	if (isEmpty(header)) {  // 비었을 경우 NULL 반환
		printf("Queue empty!!\n");
		if (lockSelection == TRUE) {
			LeaveCriticalSection(&lock);
		}
		return return_value;
	}
	if (header != NULL) {
		return_value = header->front->data;

		QueueN *del_Node_Ptr = header->front;

		if (header->front == header->rear) {
			header->rear = header->front = NULL;
		}
		else {
			header->front = header->front->link;
		}

		free(del_Node_Ptr);
	}
	if (lockSelection == TRUE) {
		LeaveCriticalSection(&lock);
	}
	return return_value;
}