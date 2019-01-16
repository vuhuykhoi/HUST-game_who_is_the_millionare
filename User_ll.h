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

/*
get List User From File
@params: file pointer
@return: lisklist that a node is an user
*/
linkList* getListUserFromFile(FILE *file);


/*
check if user is existed ?
@params: list user , username need to check
@return: 0 is not existed , 1 is existed
*/
int isExistedUserName(linkList *list,char *username);
/*
check if password is correct ?
@params: user , password need to check
@return: 0 is false , 1 is true
*/
int isCorrectPassword(val account,char *input_pass);

/*
rewrite all users in linklist to file ?
@params: linklist user,fname :file name to write
*/
void updateAccountToFile(linkList *list,char *fname);