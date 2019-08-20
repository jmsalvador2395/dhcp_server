#ifndef _SESSIONS_H
#define _SESSIONS_H

/*
 * holds session data
 */
struct session{
	int xid;
	char hostname[63];
	char macaddress[6];
	char* rqsts;
	int len_rqsts;
	short int session_step;
};

/*
 * node struct for bst implementation.
 *
 * for now i'll just use the mac address (converted to long int) as the key
 * could potentially cause problems but it's not like i'm making this for that many users anyway.
 */
struct node{
	unsigned long int key;
	struct session data;
};

/*
 * for now i'll use a simple bst to manage sessions
 */
unsigned long int genkey(char macaddress[]);

/*
 * return 1 for successful insert
 * 0 if fails
 */
int tree_insert(struct node* root, struct session s);

/*
 * checks if the session key exists in the tree
 */
int exists(struct node* root, char key[]);

/*
 * return 1 for successful delete
 * 0 if fails
 */
int node_delete(struct node* root, char key[]);

/*
 * finds and returns the session
 */
struct session find(struct node* root, char key[]);
#endif
