//#include<stdio.h>
//#include<Windows.h>
//#include<process.h>
//#include<time.h>
//
//#define TRUE 1
//#define FALSE 0
//#define MAX_COUNT 10000
//
//typedef struct Queue_Node {
//	int data;
//	struct Queue_Node *link;
//}QueueN;
//
//typedef struct QueueHeader {
//	QueueN *front;
//	QueueN *rear;
//}QueueH;
//
//QueueH qHeader = { NULL,NULL }; 
//CRITICAL_SECTION lock;
//int lockSelection = TRUE; // TRUE �� ����ȭ, FALSE �� �񵿱�ȭ
//
//void enQueue(QueueH *header, int data);
//int deQueue(QueueH *header);
//DWORD WINAPI ThreadFunc(LPTSTR argv[]);
//int isEmpty(QueueH *header);
//
//int main(DWORD argc, LPTSTR argv[]) {
//	int start_time = clock();
//	int end_time = 0;
//	unsigned int uiThreadID = 0;
//	//ũ��Ƽ�� ���� �ʱ�ȭ
//	InitializeCriticalSection(&lock);
//	DWORD  ThreadID;
//	//������ ����
//
//	HANDLE hThread1 = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &ThreadID);
//	HANDLE hThread2 = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &ThreadID);
//	if (NULL == hThread1 || NULL == hThread2) {
//		printf("[ERROR] hThread is NULL..\n");
//		return 0;
//	}
//	
//	WaitForSingleObject(hThread1, INFINITE);//�����尡 ����� �� ���� ��ٸ�
//	WaitForSingleObject(hThread2, INFINITE);
//	DeleteCriticalSection(&lock);// lock ��ü
//	//������ �ڵ��� �ݴ´�.
//	CloseHandle(hThread1);
//	CloseHandle(hThread2);
//	if (isEmpty(&qHeader)) {
//		printf("Empty\n");
//	}
//	else {
//		printf("Not empty\n");
//	}
//	end_time = clock();
//	printf("�� ���� �ð� : %0.3f��\n", (float)(end_time - start_time)/1000);
//	return 0;
//}
//
//
//int isEmpty(QueueH *header) {
//	if (header->front == NULL && header->rear == NULL) {
//		return TRUE;
//	}
//	return FALSE;
//}
//DWORD WINAPI ThreadFunc(LPTSTR argv[]) {
//	int i = 0;
//	int data = NULL;
//	for (i = 0; i < MAX_COUNT; ++i) {
//		enQueue(&qHeader, i);
//		data = deQueue(&qHeader);
//		if (data == NULL) {
//			printf("empty.. Thread ID: %d\n",GetCurrentThreadId());
//		}
//	}
//	return 0;
//}
//void enQueue(QueueH *header, int data) {
//	QueueN *new_Node = (QueueN*)malloc(sizeof(QueueN));
//
//	new_Node->data = data;//data ����
//	new_Node->link = NULL;
//
//	if (lockSelection == TRUE) {
//		EnterCriticalSection(&lock);//lock
//	}
//
//	if (header->front == NULL) {//empty
//		header->front = new_Node;
//	}
//	else {
//		header->rear->link = new_Node;
//	}
//
//	header->rear = new_Node;
//
//	if (lockSelection == TRUE) {
//		LeaveCriticalSection(&lock);
//	}
//	return;
//}
//int deQueue(QueueH *header) {
//	int return_value = NULL;
//	if (lockSelection == TRUE) {
//		EnterCriticalSection(&lock);//lock
//	}
//	if (isEmpty(header)) {  // ����� ��� NULL ��ȯ
//		printf("Queue empty!!\n");
//		if (lockSelection == TRUE) {
//			LeaveCriticalSection(&lock);
//		}
//		return return_value;
//	}
//	if (header != NULL) {
//		return_value = header->front->data;
//
//		QueueN *del_Node_Ptr = header->front;
//
//		if (header->front == header->rear) {
//			header->rear = header->front = NULL;
//		}
//		else {
//			header->front = header->front->link;
//		}
//
//		free(del_Node_Ptr);
//	}
//	if (lockSelection == TRUE) {
//		LeaveCriticalSection(&lock);
//	}
//	return return_value;
//}