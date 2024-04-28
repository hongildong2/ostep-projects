#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "datastructure.h"
#include "mapreduce.h"

#define PARTITION_NUM (10)

static ds_t** datas;

void Map(char* file_name);
void Reduce(char *key, Getter get_next, int partition_number);

void init_datastructures(int num_partitions)
{
    datas = malloc(sizeof(ds_t*) * num_partitions);
    for (int i = 0; i < num_partitions; ++i) {
        ds_t* data = malloc(sizeof(ds_t));
        init_datastructure(data, i);
        datas[i] = data;
    }
}

int main(int argc, char* argv[])
{
    MR_Run(argc, argv, Map, 10, Reduce, 10, MR_DefaultHashPartition);
    return 0;
}



void MR_Run(int argc, char *argv[], 
	    Mapper map, int num_mappers, 
	    Reducer reduce, int num_reducers, 
	    Partitioner partition)
{
    init_datastructures(PARTITION_NUM);
    // create worker thread for map
    // Mappers create word count per single given file(partition_num)
    // Sort the kvs per partition_num

    // create worker thread for reducer
    // Reducers create total word count per partition_num, which introduce race condition if words get hash collision simultaneously on multiple reducers

    // Done

}

char* getter(char *key, int partition_number)
{
    // implement function that gets kv from your datastructure, and pass this when thread creation
    return NULL;
}

void MR_Emit(char *key, char *value)
{
    // create thread safe datastructure to store Map results

}

unsigned long MR_DefaultHashPartition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0')
        hash = hash * 33 + c;
    return hash % num_partitions;
}

void Map(char *file_name) {
    FILE *fp = fopen(file_name, "r");
    assert(fp != NULL);

    char *line = NULL;
    size_t size = 0;
    while (getline(&line, &size, fp) != -1) {
        char *token, *dummy = line;
        while ((token = strsep(&dummy, " \t\n\r")) != NULL) {
            MR_Emit(token, "1");
        }
    }
    // token will be partitioned
    free(line);
    fclose(fp);
}




void Reduce(char *key, Getter get_next, int partition_number) {
    int count = 0;
    char *value;
    while ((value = get_next(key, partition_number)) != NULL)
        count++;
    printf("%s %d\n", key, count);
}
