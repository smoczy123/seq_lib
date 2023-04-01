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
	struct seq* zero;
	struct seq* one;
	struct seq* two;
	abstractionClass_t* abCls;
};

typedef struct seq seq_t;

seq_t * seq_new(void) {
	seq_t* seq = malloc(sizeof(seq_t));
	if (seq == NULL) {
		errno = ENOMEM;
	}
	else {
		seq->zero = NULL;
		seq->one = NULL;
		seq->two = NULL;
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
		if (p->zero != NULL) {
			seq_delete(p->zero);
		}
		if (p->one != NULL) {
			seq_delete(p->one);
		}
		if (p->two != NULL) {
			seq_delete(p->two);
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

int seq_add(seq_t *p, char const *s) {
	seq_t* temp = p;
	if (p == NULL || s == NULL) {
		errno = EINVAL;
		return -1;
	}
	else {
		int retVal = 0;
		for (int i = 0; i < strlen(s); i++) {
			int num = s[i] - '0';
			switch (num) {
				case 0:
					if (temp->zero == NULL) {
						temp->zero = seq_new();
						retVal = 1;
						if (temp->zero == NULL) {
							errno = ENOMEM;
							return -1;
						}
					}
					temp = temp->zero;

				case 1:
					if (temp->one == NULL) {
						temp->one = seq_new();
						retVal = 1;
						if (temp->one == NULL) {
							errno = ENOMEM;
							return -1;
						}
					}
					temp = temp->one;

				case 2:
					if (temp->two == NULL) {
						retVal = 1;
						temp->two = seq_new();
						if (temp->two == NULL) {
							errno = ENOMEM;
							return -1;
						}
					}
					temp = temp->two;

				default:
					errno = EINVAL;
					return -1;
			}
		}
		return retVal;
	}
}



