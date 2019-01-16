typedef struct account
{
	char *acc;
	char *pass;
	int sts;

}val;

typedef struct node{
	val val;
	struct node *next;
}node;

typedef struct pairNode{
	node *first;
	node *last;
}linkList;

linkList *newLinkList();

//add a value to end of list
void addToLastOfList(linkList *list,val El);

//display all node infor of list
void printList(linkList *list);

//create a new node
//input:Value of node
node* newNode(val El);

//create a strcut Val
val newVal(char *acc,char *pass);
val newEmptyVal();
//print Value 
void printVal(val El);

//check if node is existed in list?
//return 1 if is existed
//return 0 if not
int isExistedNode(linkList *list,char *acc);

//find node in list by value of node
node* findNodeByValue(linkList *list,char *acc);

linkList* getListUserFromFile(FILE *file);
int isExistedUserName(linkList *list,char *username);
int isCorrectPassword(val account,char *input_pass);
void updateAccountToFile(linkList *list,char *fname);