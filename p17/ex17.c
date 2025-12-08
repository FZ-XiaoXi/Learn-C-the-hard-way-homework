#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct Address {
    int id;
    int set;
    char *name;
    char *email;
};
struct Database {
	
	int MAX_DATA;
	int MAX_ROWS;
    struct Address *rows;
};

struct Connection {
    FILE *file;
    struct Database *db;
};

void Database_close(struct Connection *conn);
void die(struct Connection *conn,const char *message)
{
	Database_close(conn);
    
    if(errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }
    
    exit(1);
}

void Address_print(struct Address *addr)
{
    printf("%d %s %s\n",
            addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn)
{
    int rc = fread(&conn->db->MAX_ROWS, sizeof(int), 1, conn->file);
    if(rc != 1) die(conn, "Failed to load MAX_ROWS.");
    
    rc = fread(&conn->db->MAX_DATA, sizeof(int), 1, conn->file);
    if(rc != 1) die(conn, "Failed to load MAX_DATA.");
    
    conn->db->rows = malloc(sizeof(struct Address) * conn->db->MAX_ROWS);
    if(!conn->db->rows) die(conn, "Memory error for rows.");
    
    for(int i = 0; i < conn->db->MAX_ROWS; i++) {
        struct Address *addr = &conn->db->rows[i];
        
        rc = fread(&addr->id, sizeof(int), 1, conn->file);
        if(rc != 1) die(conn, "Failed to load address id.");
        
        rc = fread(&addr->set, sizeof(int), 1, conn->file);
        if(rc != 1) die(conn, "Failed to load address set.");
        
        addr->name = malloc(sizeof(char) * conn->db->MAX_DATA);
        addr->email = malloc(sizeof(char) * conn->db->MAX_DATA);
        if(!addr->name || !addr->email) die(conn, "Memory error for name/email.");
        
        rc = fread(addr->name, sizeof(char), conn->db->MAX_DATA, conn->file);
        if(rc != conn->db->MAX_DATA) die(conn, "Failed to load name.");
        
        rc = fread(addr->email, sizeof(char), conn->db->MAX_DATA, conn->file);
        if(rc != conn->db->MAX_DATA) die(conn, "Failed to load email.");
        
        addr->name[conn->db->MAX_DATA - 1] = '\0';
        addr->email[conn->db->MAX_DATA - 1] = '\0';
    }

}

struct Connection *Database_open(const char *filename, char mode)
{
    struct Connection *conn = malloc(sizeof(struct Connection));
    if(!conn) die(conn,"Memory error");

    conn->db = malloc(sizeof(struct Database));
    if(!conn->db) die(conn,"Memory error");

    if(mode == 'c') {
        conn->file = fopen(filename, "w");
    } else {
        conn->file = fopen(filename, "r+");

        if(conn->file) {
            Database_load(conn);
        }
    }

    if(!conn->file) die(conn,"Failed to open the file");

    return conn;
}

void Database_close(struct Connection *conn)
{
	if(conn) {
        if(conn->db && conn->db->rows) {
            for(int i = 0; i < conn->db->MAX_ROWS; i++) {
                if(conn->db->rows[i].email) {
                    free(conn->db->rows[i].email);
                    conn->db->rows[i].email = NULL;
                }
                if(conn->db->rows[i].name) {
                    free(conn->db->rows[i].name);
                    conn->db->rows[i].name = NULL;
                }
            }
            free(conn->db->rows);
            conn->db->rows = NULL;
        }
        if(conn->db) {
            free(conn->db);
            conn->db = NULL;
        }
        if(conn->file) {
            fclose(conn->file);
            conn->file = NULL;
        }
        free(conn);
    }
}

void Database_write(struct Connection *conn)
{
	rewind(conn->file);
    int rc = fwrite(&conn->db->MAX_ROWS, sizeof(int), 1, conn->file);
    if(rc != 1) die(conn, "Failed to write MAX_ROWS.");
    
    rc = fwrite(&conn->db->MAX_DATA, sizeof(int), 1, conn->file);
    if(rc != 1) die(conn, "Failed to write MAX_DATA.");

    for(int i = 0; i < conn->db->MAX_ROWS; i++) {
        struct Address *addr = &conn->db->rows[i];
        
        rc = fwrite(&addr->id, sizeof(int), 1, conn->file);
        if(rc != 1) die(conn, "Failed to write address id.");
        
        rc = fwrite(&addr->set, sizeof(int), 1, conn->file);
        if(rc != 1) die(conn, "Failed to write address set.");
        
        if(addr->set) {
            rc = fwrite(addr->name, sizeof(char), conn->db->MAX_DATA, conn->file);
            if(rc != conn->db->MAX_DATA) die(conn, "Failed to write name.");
            
            rc = fwrite(addr->email, sizeof(char), conn->db->MAX_DATA, conn->file);
            if(rc != conn->db->MAX_DATA) die(conn, "Failed to write email.");
        } else {
            char empty[conn->db->MAX_DATA];
            memset(empty, 0, conn->db->MAX_DATA);
            
            rc = fwrite(empty, sizeof(char), conn->db->MAX_DATA, conn->file);
            if(rc != conn->db->MAX_DATA) die(conn, "Failed to write empty name.");
            
            rc = fwrite(empty, sizeof(char), conn->db->MAX_DATA, conn->file);
            if(rc != conn->db->MAX_DATA) die(conn, "Failed to write empty email.");
        }
    }
    
    rc = fflush(conn->file);
    if(rc == -1) die(conn, "Cannot flush database.");
}

void Database_create(struct Connection *conn,int MAX_ROWS,int MAX_DATA)
{
    int i = 0;
	conn->db->MAX_ROWS=MAX_ROWS;
	conn->db->MAX_DATA=MAX_DATA;
	conn->db->rows=malloc(sizeof(struct Address)*MAX_ROWS);
    for(i = 0; i < MAX_ROWS; i++) {
        // make a prototype to initialize it
        conn->db->rows[i].id= i;
        conn->db->rows[i].set=0;
		(conn->db->rows+i)->email=malloc(sizeof(char)*MAX_DATA);
		(conn->db->rows+i)->name=malloc(sizeof(char)*MAX_DATA);
        // then just assign it
    }
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email)
{
    if(id < 0 || id >= conn->db->MAX_ROWS) {
        die(conn, "ID out of range");
    }
    
    struct Address *addr = &conn->db->rows[id];
    if(addr->set) die(conn, "Already set, delete it first");

    addr->set = 1;
    
    strncpy(addr->name, name, conn->db->MAX_DATA - 1);
    addr->name[conn->db->MAX_DATA - 1] = '\0';
    
    strncpy(addr->email, email, conn->db->MAX_DATA - 1);
    addr->email[conn->db->MAX_DATA - 1] = '\0';
}

void Database_get(struct Connection *conn, int id)
{
    struct Address *addr = conn->db->rows+id;

    if(addr->set) {
        Address_print(addr);
    } else {
        die(conn,"ID is not set");
    }
}

void Database_delete(struct Connection *conn, int id)
{
	if(id < 0 || id >= conn->db->MAX_ROWS) {
        die(conn, "ID out of range");
    }
    
    struct Address *addr = &conn->db->rows[id];
    addr->set = 0;
    if(addr->name) addr->name[0] = '\0';
    if(addr->email) addr->email[0] = '\0';
}

void Database_list(struct Connection *conn)
{
    int i = 0;
    struct Database *db = conn->db;

    for(i = 0; i < db->MAX_ROWS; i++) {
        struct Address *cur = db->rows+i;

        if(cur->set) {
            Address_print(cur);
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc < 3) die(NULL,"USAGE: ex17 <dbfile> <action> [action params]");

    char *filename = argv[1];
    char action = argv[2][0];
    struct Connection *conn = Database_open(filename, action);
    int id = 0;

	switch(action){
		case 'c':
			break;
		default:
    		if(argc > 3) id = atoi(argv[3]);
    		if(id >= conn->db->MAX_ROWS) die(conn,"There's not that many records.");
	}

    switch(action) {
        case 'c':
			if(argc!=5) die(conn,"Need MAX_ROWS MAX_DATA");
			int MAX_DATA=atoi(argv[4]);
			int MAX_ROWS=atoi(argv[3]);
            Database_create(conn,MAX_ROWS,MAX_DATA);
            Database_write(conn);
            break;

        case 'g':
            if(argc != 4) die(conn,"Need an id to get");

            Database_get(conn, id);
            break;

        case 's':
            if(argc != 6) die(conn,"Need id, name, email to set");

            Database_set(conn, id, argv[4], argv[5]);
            Database_write(conn);
            break;

        case 'd':
            if(argc != 4) die(conn,"Need id to delete");

            Database_delete(conn, id);
            Database_write(conn);
            break;

        case 'l':
            Database_list(conn);
            break;
        default:
            die(conn,"Invalid action, only: c=create, g=get, s=set, d=del, l=list");
    }

    Database_close(conn);

    return 0;
}
