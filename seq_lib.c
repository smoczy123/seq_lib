#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct abstractionClass {
	int counter;
	char *name;
};

typedef struct abstractionClass abstractionClass_t;

struct seq {
	struct seq* children[3];
	abstractionClass_t* abCls;
};

typedef struct seq seq_t;

seq_t * seq_new(void) {
	seq_t* seq = malloc(sizeof(seq_t));
	if (seq == NULL) {
		errno = ENOMEM;
	}
	else {
		for (int i = 0; i < 3; i++) {
			seq->children[i] = NULL;
		}
		seq->abCls = malloc(sizeof(abstractionClass_t));
		if (seq->abCls == NULL) {
			errno = ENOMEM;
			return NULL;
		}
		seq->abCls->counter = 1;
	}
	return seq;
}

void seq_delete(seq_t *p) {
	if (p!= NULL) {
		for (int i = 0; i < 3; i++) {
			if (p->children[i] != NULL) {
				seq_delete(p->children[i]);
			}
		}
		if (p->abCls->counter == 1) {
			free(p->abCls);
		}
		else {
			p->abCls->counter--;
		}
		free(p);
	}
}

static int valid_seq(char const *s) {
	if (s == NULL || strlen(s) == 0) {
		return 0;
	}
	for (int i = 0; i < strlen(s); i++) {
		int num = s[i] - '0';
		if (num > 2 || num < 0) {
			return 0;
		}
	}
	return 1;
}

static int valid_string(char const *s) {
	if (s == NULL || strlen(s) == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

int seq_add(seq_t *p, char const *s) {
	if (!valid_seq(s) || p == NULL) {
		errno = EINVAL;
		return -1;
	}
	seq_t *temp = p;
	seq_t *firstCreated = NULL;
	for (int i = 0; i < strlen(s); i++) {
		int num = s[i] - '0';
		if (num > 2 || num < 0) {
			errno = EINVAL;
			return -1;
		}
		if (temp->children[num] == NULL) {
			temp->children[num] = seq_new();
			if (temp->children[num] == NULL) {
				errno = ENOMEM;
				seq_delete(firstCreated);
				return -1;
			}
			if (firstCreated == NULL) {
				firstCreated = temp->children[num];
			}
		}

		temp = temp->children[num];
	}
	if (firstCreated == NULL) {
		return 0;
	}
	else {
		return 1;
	}
}

static seq_t *seq_find(seq_t *p, char const *s) {
	seq_t *temp = p;
	for (int i = 0; i < strlen(s); i++) {
		int num = s[i] - '0';
		temp = temp->children[num];
		if (temp == NULL) {
			return NULL;
		}
	}
	return temp;
}

int seq_remove(seq_t *p, char const *s) {
	if (!valid_seq(s) || p == NULL) {
		errno = EINVAL;
		return -1;
	}
	seq_t *temp = seq_find(p, s);
	if (temp == NULL) {
		return 0;
	}
	seq_delete(temp);
	return 1;
}

int seq_valid(seq_t *p, char const *s) {
	if (!valid_seq(s) || p == NULL) {
		errno = EINVAL;
		return -1;
	}
	seq_t *temp = seq_find(p, s);
	if (temp == NULL) {
		return 0;
	}
	else {
		return 1;
	}
}

int seq_set_name(seq_t *p, char const *s, char const *n) {
	if (!valid_seq(s) || !valid_string(n) || p == NULL) {
		errno = EINVAL;
		return -1;
	}
	seq_t *temp = seq_find(p, s);
	if (temp == NULL || (temp->abCls->name != NULL && 
			!strcmp(n, temp->abCls->name))) {
		return 0;
	}

	else {
		strcpy(temp->abCls->name, n);
		return 1;
	}
}

char const *seq_get_name(seq_t *p, char const *s) {
	if (!valid_seq(s) || p == NULL) {
		errno = EINVAL;
		return NULL;
	}
	seq_t *temp = seq_find(p, s);
	if (temp == NULL || temp->abCls->name == NULL) {
		errno = 0;
		return NULL;
	}
	return temp->abCls->name;
}
/*
int seq_equiv(seq_t *p, char const *s1, char const *s2) {
	if (!valid_seq(s1) || !valid_seq(s2) || p == NULL) {
		errno = EINVAL;
		return -1;
	}
	seq_t *t1 = seq_find(p, s1);
	seq_t *t2 = seq_find(p, s2);
	if (t1 == NULL || t2 == NULL || t1->abCls == t2->abCls) {
		return 0;
	}

	
}*/
