#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>

struct pop_entry {
  int year;
  int population;
  char boro[15];
};

long long get_file_size(char *f) {
	struct stat a;
	if (stat(f, &a)==-1) {
		printf("The file [%s] was not successfully opened\n", f);
		printf("%s\n", strerror(errno));
		return -1;
	}
	return a.st_size;
}

int nxt_index(char s[], int i) {
	while (s[i] != 0) i++;
	return i+1;
}

void print_entry(struct pop_entry *a) {
	printf("Year: %d     Boro: %s     Pop: %d\n", a->year, a->boro, a->population);
}

void read_csv(char *in, char *out) {
	char *bor[5];
	bor[0]="Manhattan    ";
	bor[1]="Brooklyn     ";
	bor[2]="Queens       ";
	bor[3]="Bronx        ";
	bor[4]="Staten Island";
	long long sz = get_file_size(in);
	if (sz==-1) return;
	char s[sz+1];
	printf("Reading %s...\n", in);
	int filea = open(in, O_RDONLY, 0777);
	read(filea, s, sz+1);
	int cnt = -5;
	for (int i = 0; i < sz+1; i++) {
		if (s[i]==0) break;
		if (s[i]==',') cnt++, s[i]=0;
		if (s[i]=='\n') s[i]=0;
	}
	struct pop_entry entries[cnt];
	int ind = 0;
	for (int i = 0; i < 6; i++) ind = nxt_index(s, ind);
	for (int i = 0; 5*i < cnt; i++) {
		int yr;
		sscanf(s+ind, "%d", &yr);
		ind = nxt_index(s, ind);
		for (int j = 0; j < 5; j++) {
			int k = 5*i+j;
			entries[k].year = yr;
			strcpy(entries[k].boro, bor[j]);
			sscanf(s+ind, "%d", &entries[k].population);
			ind = nxt_index(s, ind);
		}
	}
	int fileb = open(out, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if (fileb==-1) {
		printf("Error in writing to file %s\n%s\n", out, strerror(errno));
		return;
	}
	printf("Wrote %lu bytes to %s\n", write(fileb, entries, cnt * sizeof(struct pop_entry)), out);
}

void read_data(char *in) {
	long long sz = get_file_size(in);
	if (sz==-1) return;
	struct pop_entry *entries = malloc(sz);
	int filea = open(in, O_RDONLY, 0777);
	read(filea, entries, sz);
	for (int i = 0; i*sizeof(struct pop_entry)<sz; i++) {
		printf("# %03d     ", i);
		print_entry(entries+i);
	}
}

void add_data(char *out) {
	printf("Enter year boro pop: ");
	struct pop_entry a;
	if (scanf("%d %s %d", &a.year, a.boro, &a.population) != 3) {
		printf("Please enter the data in the correct format\n");
		return;
	}
	while (strlen(a.boro)<13) strcat(a.boro, " ");
	int fileb = open(out, O_WRONLY | O_APPEND, 0777);
	if (fileb==-1) {
		printf("Error in appending to file %s\n%s\n", out, strerror(errno));
		return;
	}
	lseek(fileb, 0, SEEK_END);
	write(fileb, &a, sizeof(struct pop_entry));
	printf("Appended data to file [%s]: ", out);
	print_entry(&a);
}

void update_data(char *out) {
	read_data(out);
	long long sz = get_file_size(out);
	sz /= sizeof(struct pop_entry);
	int i;
	printf("Entry to update: ");
	if (scanf("%d", &i) != 1) {
		printf("Please enter the data in the correct format\n");
		return;
	}
	if (i >= sz) {
		printf("Index is out of bounds\n");
		return;
	}
	printf("Enter year boro pop: ");
	struct pop_entry a;
	if (scanf("%d %s %d", &a.year, a.boro, &a.population) != 3) {
		printf("Please enter the data in the correct format\n");
		return;
	}
	while (strlen(a.boro)<13) strcat(a.boro, " ");
	int fileb = open(out, O_WRONLY, 0777);
	lseek(fileb, i*sizeof(struct pop_entry), SEEK_SET);
	write(fileb, &a, sizeof(struct pop_entry));
	printf("File [%s] updated.\n", out);
}

int main() {
	read_csv("data.csv", "nyc_pop.data");
	add_data("nyc_pop.data");
	update_data("nyc_pop.data");
	read_data("nyc_pop.data");
}
