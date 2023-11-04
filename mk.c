#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define alf_size 26

typedef struct trie_node_t {
	int valid;
	struct trie_node_t *childs[26];
	int nr_childs;
	int freq;
	char *value;
} trie_node_t;

typedef struct trie_t {
	trie_node_t *root;
	int size;
} trie_t;

trie_node_t *create_node(void)
{
	//creez si initializez un nod din trie
	trie_node_t *node;
	node = malloc(sizeof(trie_node_t));
	node->nr_childs = 0;
	node->valid = 0;
	node->freq = 0;
	node->value = NULL;
	for (int i = 0; i < 26; i++)
		node->childs[i] = NULL;
	return node;
}

trie_t *create_trie(void)
{
	//creaza un trie
	trie_t *abc;
	abc = malloc(sizeof(trie_t));
	abc->size = 0;
	return abc;
}

void insert(trie_t *trie, char *key)
{
	trie_node_t *root = trie->root;
	trie->size++;
	int j;
	for (unsigned int i = 0; i < strlen(key); i++) {
		j = key[i] - 'a';
		if (!root->childs[j])
			root->childs[j] = create_node();//creez nodul
		if (!root->childs[j]->value) {
			root->childs[j]->value = malloc(i + 2);
			strncpy(root->childs[j]->value, key, i + 1);
			root->childs[j]->value[i + 1] = '\0';
			//introduc in nod cuvantul
		}
		root->nr_childs++;
		root = root->childs[j];
	}
	root->valid = 1;
	root->freq++;
	//la final marchez terminarea unui cuvant si frecventa acestuia
}

void delete_node(trie_node_t **abc, int pos)
{
	//functia primeste pozitia din vectorul de copii a nodului ce trebuie sters
	trie_node_t *node = *abc;
	trie_node_t *next = node->childs[pos];
	if (next->value)
		free(next->value);
	free(next);
	node->childs[pos] = NULL;
}

int delete_word(trie_node_t *root, char *key, unsigned int i)
{
	//oprirea din recursvitate este cand am parcurs tot cuvantului
	if (i == strlen(key)) {
		if (root->valid == 1) {
			root->valid = 0;
			return root->nr_childs == 0;
		}
		return 0;
	}
	trie_node_t *next;
	int j = key[i] - 'a';
	next = root->childs[j];
	i++;
	if (next && delete_word(next, key, i) == 1) {
		//la intoarcerea din recursivitate daca nodul nu face parte
		//dintr un cuvant este sters din memorie
		delete_node(&root, j);
		root->nr_childs--;
		if (root->nr_childs == 0 && root->valid == 0)
			return 1;
	}
	return 0;
}

trie_node_t *search(char *key, trie_node_t *root)
{
	//returneaza pointer la nodul ce contine cuvantul dorit
	if (!root)
		return NULL;
	for (unsigned int i = 0; i < strlen(key); i++) {
		if (!root)
			return NULL;
		int j;
		j = key[i] - 'a';
		root = root->childs[j];
	}
	return root;
}

void dfs_most_frequent(trie_node_t *root, char *str, int *max)
{
	//o parcurgere dfs ce verifica frecventa fiecarui cuvant
	if (!root)
		return;
	if (root->freq > *max) {
		strcpy(str, root->value);
		*max = root->freq;
	}
	for (int i = 0; i < 26; i++) {
		if (root->childs[i])
			dfs_most_frequent(root->childs[i], str, max);
	}
}

void lowst_lexic(trie_node_t *root, char *str)
{
	int ok = 1, i, pas = 0, sem = 1;
	//daca cuvnatul dat exista el este cel mai mic lexicografic
	if (root->valid == 1) {
		strncpy(str, root->value, strlen(root->value));
		return;
	}
	//parcurg cat mai la 'stanga' arborele
	//astfel iau cele mai mici litere d.p.d.v lexicografic
	while (sem && root && ok) {
		sem = 0;
		pas++;
		for (i = 0; i < 26; i++)
			if (root->childs[i]) {
				sem = 1;
				break;
			}
		if (sem) {
			//copiez in str cuvantul gasit
			if (root->childs[i]->value)
				strncpy(str, root->childs[i]->value,
						strlen(root->childs[i]->value));
			// daca este nodul valid inchei algoritmul
			if (root->childs[i]->valid == 1)
				ok = 0;
			root = root->childs[i];
		}
	}
	if (root->value)
		strcpy(str, root->value);
}

void lowest_length(trie_node_t *root, char *str, unsigned int *min)
{
	//o parcurgere dfs care verifica la fiecare pas daca exista cuvantul
	//si daca lungimea acestuia este minima
	if (!root)
		return;
	if (root->valid == 1 && strlen(root->value) < *min) {
		strcpy(str, root->value);
		*min = strlen(root->value);
	}
	for (int i = 0; i < 26; i++)
		if (root->childs[i])
			lowest_length(root->childs[i], str, min);
}

void auto_complete(trie_node_t *root, int type, char *key)
{
	//obtin pointer la nnodul de unde voi incepe algoritmul
	trie_node_t *node = search(key, root);
	char *str = calloc(50, 1);
	int max = 0;
	unsigned int min = 1000;
	if (type == 3) {
		if (!node) {
			printf("No words found\n");
			free(str);
			return;
		}
		dfs_most_frequent(node, str, &max);
		printf("%s\n", str);
	}
	if (type == 1) {
		if (!node) {
			printf("No words found\n");
			free(str);
			return;
		}
		lowst_lexic(node, str);
		printf("%s\n", str);
	}
	if (type == 2) {
		if (!node) {
			printf("No words found\n");
			free(str);
			return;
		}
		lowest_length(node, str, &min);
		printf("%s\n", str);
	}
	if (type == 0) {
		if (!node) {
			printf("No words found\n");
			printf("No words found\n");
			printf("No words found\n");
			free(str);
			return;
		}
		lowst_lexic(node, str);
		printf("%s\n", str);
		free(str);
		str = calloc(50, 1);
		lowest_length(node, str, &min);
		printf("%s\n", str);
		free(str);
		str = calloc(50, 1);
		dfs_most_frequent(node, str, &max);
		printf("%s\n", str);
	}
	free(str);
}

int check_dif(char *str, char *word, int k)
{
	//daca nu au lunngimi egale nu mai are sens comparatia
	if (strlen(str) != strlen(word))
		return 0;
	int n = strlen(str), p = 0;
	//calculez numarul de litere diferite dintre cele doua cuvinte
	for (int i = 0; i < n; i++) {
		if (str[i] != word[i])
			p++;
	}
	if (p <= k)
		return 1;
	return 0;
}

void autocorrect(trie_node_t *root, char *str, int k, int *sem)
{
	//o parcurgere dfs ce compara fiecare cuvant cu cel primit
	if (!root)
		return;
	if (root->value && root->valid == 1) {
		int ok = check_dif(str, root->value, k);
		if (ok == 1) {
			printf("%s\n", root->value);
			*sem = 1;
		}
	}
	for (int i = 0; i < 26; i++) {
		if (root->childs[i])
			autocorrect(root->childs[i], str, k, sem);
	}
}

void delete(trie_node_t **abc)
{
	//functia sterge tot arborele
	trie_node_t *root = *abc;
	if (!root)
		return;
	for (int i = 0; i < 26; i++)
		if (root->childs[i])
			delete(&root->childs[i]);
	free(root->value);
	free(root);
}

int main(void)
{
	char command[20];
	scanf("%s", command);
	trie_t *trie;
	char key[50];
	trie = create_trie();
	trie->root = create_node();
	while (1) {
		if (strcmp(command, "INSERT") == 0) {
			scanf("%s", key);
			insert(trie, key);
		}
		if (strcmp(command, "REMOVE") == 0) {
			scanf("%s", key);
			delete_word(trie->root, key, 0);
		}
		if (strcmp(command, "AUTOCOMPLETE") == 0) {
			int type;
			scanf("%s", key);
			scanf("%d", &type);
			auto_complete(trie->root, type, key);
		}
		if (strcmp(command, "EXIT") == 0) {
			delete(&trie->root);
			free(trie);
			break;
		}
		if (strcmp(command, "AUTOCORRECT") == 0) {
			int k, sem = 0;
			char str[50];
			scanf("%s", str);
			scanf("%d", &k);
			autocorrect(trie->root, str, k, &sem);
			if (sem == 0)
				printf("No words found\n");
		}
		if (strcmp(command, "LOAD") == 0) {
			char filename[20], word[50];
			scanf("%s", filename);
			FILE *in = fopen(filename, "rt");
			while (fscanf(in, "%s", word) != EOF)
				insert(trie, word);
			fclose(in);
		}
		scanf("%s", command);
	}
	return 0;
}
