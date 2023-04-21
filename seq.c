#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct abstractionClass {
	int counter;
	char *name;
};

typedef struct abstractionClass abClass_t;

struct seq {
	struct seq* children[3];
	abClass_t* abCls;
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
		seq->abCls = malloc(sizeof(abClass_t));
		if (seq->abCls == NULL) {
			free(seq);
			errno = ENOMEM;
			return NULL;
		}
		seq->abCls->counter = 1;
		seq->abCls->name = NULL;
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
			if (p->abCls->name != NULL) {
				free(p->abCls->name);
			}
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
	int len = strlen(s);
	for (int i = 0; i < len; i++) {
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
	seq_t *father = NULL;
	int father_num;
	int len = strlen(s);
	for (int i = 0; i < len; i++) {
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
				if (father != NULL) {
					father->children[father_num] = NULL;
				}
				return -1;
			}
			if (firstCreated == NULL) {
				firstCreated = temp->children[num];
				father = temp;
				father_num = num;
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

static seq_t *seq_find(seq_t *p, char const *s, int father) {
	seq_t *temp = p;
	int len = strlen(s);
	for (int i = 0; i < len - father; i++) {
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
	int len = strlen(s);
	seq_t *father = seq_find(p, s, 1);
	int num = s[len - 1] - '0';
		
	if (father == NULL || father->children[num] == NULL) {
		return 0;
	}
	seq_delete(father->children[num]);
	father->children[num] = NULL;
	return 1;
}

int seq_valid(seq_t *p, char const *s) {
	if (!valid_seq(s) || p == NULL) {
		errno = EINVAL;
		return -1;
	}
	seq_t *temp = seq_find(p, s, 0);
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
	seq_t *temp = seq_find(p, s, 0);

	if (temp == NULL || (temp->abCls->name != NULL && 
			!strcmp(n, temp->abCls->name))) {
		return 0;
	}

	else {
		char *temp_name =  malloc(sizeof(char) * (strlen(n) + 1));
		if (temp_name == NULL) {
			errno = ENOMEM;
			return -1;
		}
		if (temp->abCls->name != NULL) {
			free(temp->abCls->name);
		}
		temp->abCls->name = temp_name;
		strcpy(temp->abCls->name, n);
		return 1;
	}
}

char const *seq_get_name(seq_t *p, char const *s) {
	if (!valid_seq(s) || p == NULL) {
		errno = EINVAL;
		return NULL;
	}
	seq_t *temp = seq_find(p, s, 0);
	if (temp == NULL || temp->abCls->name == NULL) {
		errno = 0;
		return NULL;
	}
	return temp->abCls->name;
}

int seq_merge(seq_t *p, abClass_t *newAbCls, abClass_t *abCls1, abClass_t *abCls2) {
	if (p == NULL) {
		return 0;	
	}
	if (p->abCls == abCls1 || p->abCls == abCls2) {
		p->abCls = newAbCls;
	}
	for (int i = 0; i < 3; i++) {
		seq_merge(p->children[i], newAbCls, abCls1, abCls2);
	}
	return 1;

}
int seq_equiv(seq_t *p, char const *s1, char const *s2) {
	if (!valid_seq(s1) || !valid_seq(s2) || p == NULL) {
		errno = EINVAL;
		return -1;
	}
	seq_t *t1 = seq_find(p, s1, 0);
	seq_t *t2 = seq_find(p, s2, 0);
	if (t1 == NULL || t2 == NULL || t1->abCls == t2->abCls) {
		return 0;
	}
	
	abClass_t *newAbCls = malloc(sizeof(abClass_t));
	if (newAbCls ==  NULL) {
		errno = ENOMEM;
		return -1;
	}
	newAbCls->counter = t1->abCls->counter + t2->abCls->counter;
	char *name1 = t1->abCls->name;
	char *name2 = t2->abCls->name;
	if (name1 == NULL && name2 == NULL) {
		newAbCls->name = NULL;
	}
	else {
		if (name1 == NULL) {
				newAbCls->name = malloc(sizeof(char) * (strlen(name2) + 1));
				if (newAbCls->name == NULL) {
					errno = ENOMEM;
					free(newAbCls);
					return -1;
				}
				strcpy(newAbCls->name, name2);
				free(name2);
		} else if (name2 == NULL) {
				newAbCls->name = malloc(sizeof(char) * (strlen(name1) + 1));
				if (newAbCls->name == NULL) {
					errno = ENOMEM;
					free(newAbCls);
					return -1;
				}
				strcpy(newAbCls->name, name1);
				free(name1);
		} else {
				newAbCls->name = malloc(sizeof(char) * (strlen(name1) + strlen(name2) + 1));
				if (newAbCls->name == NULL) {
					errno = ENOMEM;
					free(newAbCls);
					return -1;
				}
				strcpy(newAbCls->name, name1);
				strcat(newAbCls->name, name2);
				free(name1);
				free(name2);
		}
	}
	free(t1->abCls);
	free(t2->abCls);
	seq_merge(p, newAbCls, t1->abCls, t2->abCls);
	return 1;
}
