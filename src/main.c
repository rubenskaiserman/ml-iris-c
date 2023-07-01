#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct IrisInformation
{
    float sepal_length;
    float sepal_width;
    float petal_length;
    float petal_width;
    char *class;
};
const int DATA_SIZE = 151;

struct IrisInformation *getDataFrom(const char *path)
{
    struct IrisInformation *data = malloc(DATA_SIZE * sizeof(struct IrisInformation));

    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }
    char *line = malloc(100 * sizeof(char));
    int i = 0;
    fgets(line, sizeof(line), file); // dropping column name line
    while (fgets(line, 100, file) != NULL)
    {
        struct IrisInformation iris;
        char *token = strtok(line, ",");
        iris.sepal_length = atof(token);
        token = strtok(NULL, ",");
        iris.sepal_width = atof(token);
        token = strtok(NULL, ",");
        iris.petal_length = atof(token);
        token = strtok(NULL, ",");
        iris.petal_width = atof(token);
        token = strtok(NULL, ",");
        iris.class = malloc(strlen(token) + 1); // +1 for null terminator
        strcpy(iris.class, token);

        data[i] = iris;
        i++;
    }

    fclose(file);
    return data;
}

int main()
{
    const char *path = "../database/iris.csv";
    struct IrisInformation *data = getDataFrom(path);
    if (data != NULL)
    {
        // Printing the data for verification
        for (int j = 0; j < DATA_SIZE; j++)
        {
            printf("Iris %d:\n", j + 1);
            printf("Sepal Length: %.2f\n", data[j].sepal_length);
            printf("Sepal Width: %.2f\n", data[j].sepal_width);
            printf("Petal Length: %.2f\n", data[j].petal_length);
            printf("Petal Width: %.2f\n", data[j].petal_width);
            printf("Class: %s\n", data[j].class);
            printf("---------------------\n");
        }




        for (int j = 0; j < DATA_SIZE; j++)
        {
            free(data[j].class);
        }
        free(data);
    }

    return 0;
}