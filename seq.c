#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct equivalence_class {
	int counter;
	char *name;
};

typedef struct equivalence_class equi_cls_t;

struct seq {
	struct seq* children[3];
	struct seq* father;
	equi_cls_t* equi_cls;
	// do DFS z pamięcią O(1)
	int visited;
};

typedef struct seq seq_t;

// sprawdza czy wskaźnik na znaki jest poprawnym napisem

static int valid_string(char const *s) {
	if (s == NULL || strlen(s) == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

// sprawdza czy wskaźnik na znaki jest poprawnym ciągiem

static int valid_seq(char const *s) {
	if (!valid_string(s)) {
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

// usuwa klasę abstrakcji podanego, usuwanego ciągu

static void remove_equi_cls_occurence (seq_t *seq) {
		if (seq->equi_cls->counter == 1) {
			if (seq->equi_cls->name != NULL) {
				free(seq->equi_cls->name);
			}
			free(seq->equi_cls);
		}
		else {
			seq->equi_cls->counter--;
		}
}

// znajduje podany ciąg lub jego ojca w drzewie

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

// łączy dwa napisy, z których co najmniej jeden nie jest NULLEM

static char *merge_strings(char *name1, char *name2) {
	char *new_name;
	if (name1 == NULL) {
			new_name = malloc(sizeof(char) *
					(strlen(name2) + 1));
			if (new_name == NULL) {
				errno = ENOMEM;	
				return NULL;
			}
			strcpy(new_name, name2);
			free(name2);
	} 
	else if (name2 == NULL) {
			new_name = malloc(sizeof(char) *
					(strlen(name1) + 1));
			if (new_name == NULL) {
				errno = ENOMEM;
				return NULL;
			}
			strcpy(new_name, name1);
			free(name1);
	}
	else {
			new_name = malloc(sizeof(char) *
					(strlen(name1) + strlen(name2) + 1));
			if (new_name == NULL) {
				errno = ENOMEM;
				return NULL;
			}
			strcpy(new_name, name1);
			strcat(new_name, name2);
			free(name1);
			free(name2);
	}
	return new_name;
}

// tworzy nowy pusty zbiór ciągów

seq_t * seq_new(void) {
	seq_t* seq = malloc(sizeof(seq_t));
	if (seq == NULL) {
		errno = ENOMEM;
	}
	else {
		for (int i = 0; i < 3; i++) {
			seq->children[i] = NULL;
		}
		seq->father = NULL;
		seq->visited = 0;
		seq->equi_cls = malloc(sizeof(equi_cls_t));
		if (seq->equi_cls == NULL) {
			free(seq);
			errno = ENOMEM;
			return NULL;
		}
		seq->equi_cls->counter = 1;
		seq->equi_cls->name = NULL;
	}
	return seq;
}

// usuwa zbiór ciągów i zwalnia całą używaną przez niego pamięć
// DFS z pamięcią O(1)

void seq_delete(seq_t *p) {
	if (p!= NULL) {
		seq_t *temp = p;
		seq_t *goal = p->father;
		// zwalniamy wszystkie dzieci p
		while (temp != goal) {
			int i = 0;
			while (i < 3) {
				if (temp->children[i] != NULL && temp->children[i]->visited == 0) {
					temp = temp->children[i];
					i = 4;
				}
				i++;
			}

			// wszystkie dzieci odwiedzone -> zwalniamy je i przechodzimy do ojca 
			if (i == 3) {
				for (int j = 0; j < 3; j++) {
					if (temp->children[j] != NULL) {
						remove_equi_cls_occurence(temp->children[j]);
						free(temp->children[j]);
						temp->children[j] = NULL;
					}
				}
				temp->visited = 1;
				temp = temp->father;
			}
		}

		// zwalniamy p
		remove_equi_cls_occurence(p);
		free(p);
	}
}


// dodaje do zbioru ciągów podany ciąg i wszystkie niepuste podciągi będące jego prefiksem

int seq_add(seq_t *p, char const *s) {
	if (!valid_seq(s) || p == NULL) {
		errno = EINVAL;
		return -1;
	}
	seq_t *temp = p;
	seq_t *first_created = NULL;
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
				// brak pamięci -> usuwamy pierwszy stworzony ciąg 
				// i nullujemy w ojcu, aby wrócić do stanu początkowego
				seq_delete(first_created);
				if (father != NULL) {
					father->children[father_num] = NULL;
				}
				return -1;
			}
			temp->children[num]->father = temp;
			if (first_created == NULL) {
				first_created = temp->children[num];
				father = temp;
				father_num = num;
			}
		}

		temp = temp->children[num];
	}
	return (first_created != NULL);
}

// usuwa ze zbioru ciągów podany ciąg i wszystkie ciągi,
// których jest on prefiksem

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

// sprawdza, czy podany ciąg należy do zbioru ciągów

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

// ustawia lub zmienia nazwę klasy abstrakcji,
// do której należy podany ciąg

int seq_set_name(seq_t *p, char const *s, char const *n) {
	if (!valid_seq(s) || !valid_string(n) || p == NULL) {
		errno = EINVAL;
		return -1;
	}
	seq_t *temp = seq_find(p, s, 0);

	if (temp == NULL || (temp->equi_cls->name != NULL && 
			!strcmp(n, temp->equi_cls->name))) {
		return 0;
	}

	else {
		char *temp_name =  malloc(sizeof(char) * (strlen(n) + 1));
		if (temp_name == NULL) {
			errno = ENOMEM;
			return -1;
		}
		if (temp->equi_cls->name != NULL) {
			free(temp->equi_cls->name);
		}
		temp->equi_cls->name = temp_name;
		strcpy(temp->equi_cls->name, n);
		return 1;
	}
}

// daje wskaźnik na napis zawierający nazwę 
// klasy abstrakcji, do której należy podany ciąg

char const *seq_get_name(seq_t *p, char const *s) {
	if (!valid_seq(s) || p == NULL) {
		errno = EINVAL;
		return NULL;
	}
	seq_t *temp = seq_find(p, s, 0);
	if (temp == NULL || temp->equi_cls->name == NULL) {
		errno = 0;
		return NULL;
	}
	return temp->equi_cls->name;
}

// przypisuje ciągi z podanych klas abstrakcji 
// do nowej, podanej klasy abstrakcji
// DFS z pamięcią O(1)

int seq_merge(seq_t *p, equi_cls_t *new_equi_cls, equi_cls_t *equi_cls1,
		equi_cls_t *equi_cls2) {
	if (p == NULL) {
		return 0;	
	}
	seq_t *temp = p;
	while (temp != NULL) {
		int i = 0;
		if (temp->equi_cls == equi_cls1 || temp->equi_cls == equi_cls2) {
			temp->equi_cls = new_equi_cls;
		}
		while (i < 3) {
			if (temp->children[i] != NULL && temp->children[i]->visited == 0) {
				temp = temp->children[i];
				i = 4;
			}
			i++;
		}
		// wszystkie dzieci zmienione -> przechodzimy na ojca
		if (i == 3) {
			for (int j = 0; j < 3; j++) {
				if (temp->children[j] != NULL) {
					temp->children[j]->visited = 0;
				}
			}
			temp->visited = 1;
			temp = temp->father;
		}
	}
	p->visited = 0;
	return 1;
}

// łączy w jedną klasę abstrakcji klasy abstrakcji
// reprezentowane przez podane ciągi

int seq_equiv(seq_t *p, char const *s1, char const *s2) {
	if (!valid_seq(s1) || !valid_seq(s2) || p == NULL) {
		errno = EINVAL;
		return -1;
	}
	seq_t *t1 = seq_find(p, s1, 0);
	seq_t *t2 = seq_find(p, s2, 0);
	if (t1 == NULL || t2 == NULL || t1->equi_cls == t2->equi_cls) {
		return 0;
	}
	
	equi_cls_t *new_equi_cls = malloc(sizeof(equi_cls_t));
	if (new_equi_cls ==  NULL) {
		errno = ENOMEM;
		return -1;
	}

	new_equi_cls->counter = t1->equi_cls->counter + t2->equi_cls->counter;
	char *name1 = t1->equi_cls->name;
	char *name2 = t2->equi_cls->name;

	if (name1 == NULL && name2 == NULL) {
		new_equi_cls->name = NULL;
	}
	else {
		new_equi_cls->name = merge_strings(name1, name2);
		if (new_equi_cls->name == NULL) {
			free(new_equi_cls);
			return -1;
		}
	}
	// nowa klasa przygotowana -> zwalniamy dwie poprzednie i przerzucamy ciągi
	free(t1->equi_cls);
	free(t2->equi_cls);
	seq_merge(p, new_equi_cls, t1->equi_cls, t2->equi_cls);
	return 1;
}
