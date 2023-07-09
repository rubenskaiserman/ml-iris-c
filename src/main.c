#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct IrisInformation
{
    double sepal_length;
    double sepal_width;
    double petal_length;
    double petal_width;
    char *class;
};

struct IrisInformationArrays
{
    char *atributos[5];
    double caracteristicas[150][4];
    char *class[150];
};
const int DATA_LENGTH = 150;

struct IrisInformationArrays *getDataFrom(char *path);
void printMenu();
void printStatistics(double caracteristicas[DATA_LENGTH][4], char *atributos[5], char *classes[DATA_LENGTH]);
double getMinimo(double caracteristicas[DATA_LENGTH][4], int col);
double getMaximo(double caracteristicas[DATA_LENGTH][4], int col);
double getMedia(double caracteristicas[DATA_LENGTH][4], int col);
double desvio_padrao(double caracteristicas[DATA_LENGTH][4], int col);
int *getAllOcorrencias(char *classes[DATA_LENGTH]);
int getOcorrencias(char *classes[DATA_LENGTH], char *classe);
char *guessClass(double attributes[DATA_LENGTH][4], char *classes[DATA_LENGTH], struct IrisInformation input);
double dist(double attr1[4], double attr2[4]);
char *knn(double attributes[DATA_LENGTH][4], char *classes[DATA_LENGTH], struct IrisInformation input, int k);
char *searchMajoritaryClass(char **classes, int size);
double *initializeArray(int size);
int getMaxValueIndex(double *array, int size);

int main()
{
    char *path = "../database/iris.csv";
    struct IrisInformationArrays *dataArrays = getDataFrom(path);
    // Assume-se que foram obtidos os dados
    struct IrisInformation input;
    char *class = malloc(256 * sizeof(char));
    int resposta;
    int precision;

    while (resposta != 3)
    {
        if (resposta < 0 || resposta > 3)
        {
            printf("------------------------\nOpção inválida.\n------------------------\n");
        }
        else if (resposta == 1)
        {
            printStatistics(dataArrays->caracteristicas, dataArrays->atributos, dataArrays->class);
        }
        else if (resposta == 2)
        {
            printf("Insira a amostra a ser classificada...\n\n");
            printf("Comprimento da Sepala: ");
            scanf("%lf", &input.sepal_length);
            printf("Largura da Sepala: ");
            scanf("%lf", &input.sepal_width);
            printf("Comprimento da Petala: ");
            scanf("%lf", &input.petal_length);
            printf("Largura da Petala: ");
            scanf("%lf", &input.petal_width);
            printf("Insira a precisão desejada: ");
            scanf("%d", &precision);

            // class = guessClass(dataArrays->caracteristicas, dataArrays->class, input);
            class = knn(dataArrays->caracteristicas, dataArrays->class, input, precision);
            printf("\n\nA classe da amostra é: %s\n\n\n", class);
        }
        printMenu();
        scanf("%d", &resposta);
    }

    // Freeing allocated memory
    for (int j = 0; j < DATA_LENGTH; j++)
    {
        free(dataArrays->class[j]);
    }
    free(dataArrays);
    free(class);

    printf("GOODBYE!!!!!!\n");
    return 0;
}

struct IrisInformationArrays *getDataFrom(char *path)
{
    const int LINE_SIZE = 256;

    struct IrisInformation *data = malloc(DATA_LENGTH * sizeof(struct IrisInformation));
    if (data == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Error opening the file.\n");
        exit(1);
    }

    char *line = malloc(LINE_SIZE * sizeof(char));
    fgets(line, LINE_SIZE, file); // skipping column name line

    int i = 0;
    while (fgets(line, LINE_SIZE, file) != NULL)
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
    free(line);

    struct IrisInformationArrays *arrays = malloc(sizeof(struct IrisInformationArrays));
    if (arrays == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < DATA_LENGTH; i++)
    {
        arrays->caracteristicas[i][0] = data[i].sepal_length;
        arrays->caracteristicas[i][1] = data[i].sepal_width;
        arrays->caracteristicas[i][2] = data[i].petal_length;
        arrays->caracteristicas[i][3] = data[i].petal_width;
        arrays->class[i] = data[i].class;
    }

    arrays->atributos[0] = "sepal_length";
    arrays->atributos[1] = "sepal_width";
    arrays->atributos[2] = "petal_length";
    arrays->atributos[3] = "petal_width";
    arrays->atributos[4] = "class";

    return arrays;
}

void printMenu()
{
    printf("\n\n\n------------------------------\n");
    printf("[1] – Mostrar estatísticas\n\n[2] – Classificar amostra\n\n[3] – Sair\n\n");
    printf("------------------------------\n\n\n");
}

void printStatistics(double caracteristicas[DATA_LENGTH][4], char *atributos[5], char *classes[DATA_LENGTH])
{
    printf("------------------------------\n");
    printf("Estatísticas\n");
    printf("------------------------------\n");

    for (int i = 0; i < 4; i++)
    {
        printf("%s\n", atributos[i]);
        printf("Mínimo: %lf\n", getMinimo(caracteristicas, i));
        printf("Máximo: %lf\n", getMaximo(caracteristicas, i));
        printf("Média: %lf\n", getMedia(caracteristicas, i));
        printf("Desvio padrão: %lf\n", desvio_padrao(caracteristicas, i));
        printf("------------------------------\n");
    }
    printf("Classes\n");
    printf("------------------------------\n");
    int *ocorrencias = malloc(3 * sizeof(int));
    ocorrencias = getAllOcorrencias(classes);
    printf("Iris-setosa: %d ocorrências\n", ocorrencias[0]);
    printf("Iris-versicolor: %d ocorrências\n", ocorrencias[1]);
    printf("Iris-virginica: %d ocorrências\n", ocorrencias[2]);
    free(ocorrencias);
}

double getMinimo(double caracteristicas[DATA_LENGTH][4], int col)
{
    double minimo = caracteristicas[0][col];
    for (int i = 0; i < DATA_LENGTH; i++)
    {
        if (caracteristicas[i][col] < minimo)
        {
            minimo = caracteristicas[i][col];
        }
    }
    return minimo;
}

double getMaximo(double caracteristicas[DATA_LENGTH][4], int col)
{
    double maximo = caracteristicas[0][col];
    for (int i = 0; i < DATA_LENGTH; i++)
    {
        if (caracteristicas[i][col] > maximo)
        {
            maximo = caracteristicas[i][col];
        }
    }
    return maximo;
}

double getMedia(double caracteristicas[DATA_LENGTH][4], int col)
{
    double media = 0;
    for (int i = 0; i < DATA_LENGTH; i++)
    {
        media += caracteristicas[i][col];
    }
    media /= DATA_LENGTH;
    return media;
}

double desvio_padrao(double caracteristicas[DATA_LENGTH][4], int col)
{
    // Pesquisei por calculo de desvio padrão no google e foi o que eu achei em:
    // https://pt.khanacademy.org/math/statistics-probability/summarizing-quantitative-data/variance-standard-deviation-population/a/calculating-standard-deviation-step-by-step
    // Ficou bem perto do exemplo e faz sentido, portanto foi mantido assim.

    double media = getMedia(caracteristicas, col);
    double desvio_padrao = 0;
    for (int i = 0; i < DATA_LENGTH; i++)
    {
        desvio_padrao += (media - caracteristicas[i][col]) * (media - caracteristicas[i][col]);
    }
    desvio_padrao /= DATA_LENGTH;
    desvio_padrao = sqrt(desvio_padrao);

    return desvio_padrao;
}

int *getAllOcorrencias(char *classes[DATA_LENGTH])
{
    int *ocorrencias = malloc(3 * sizeof(int));

    ocorrencias[0] = getOcorrencias(classes, "setosa\r\n");
    ocorrencias[1] = getOcorrencias(classes, "versicolor\r\n");
    ocorrencias[2] = getOcorrencias(classes, "virginica\r\n");
    // Contudo todavia e entretanto, o \r\n é necessário para o strcmp funcionar.
    // Pois, aparentemente, o csv foi gerado no Windows e o \r\n é o caractere de quebra de linha no Windows.
    // Daí para realizar a comparação de strings é necessário que o \r\n esteja presente.

    ocorrencias[0] += getOcorrencias(classes, "setosa");
    ocorrencias[1] += getOcorrencias(classes, "versicolor");
    ocorrencias[2] += getOcorrencias(classes, "virginica");
    // O item final não tem quebra de linha então é necessário fazer a comparação sem o \r\n.
    // Existem soluções mais eficientes mas eu diria que essa é a mais visível e fácil de entender.

    return ocorrencias;
}

int getOcorrencias(char *classes[DATA_LENGTH], char *classe)
{
    int ocorrencias = 0;
    for (int i = 0; i < DATA_LENGTH; i++)
    {
        if (strcmp(classes[i], classe) == 0)
        {
            ocorrencias++;
        }
    }
    return ocorrencias;
}

char *guessClass(double attributes[DATA_LENGTH][4], char *classes[DATA_LENGTH], struct IrisInformation input)
{
    char *class = malloc(256 * sizeof(char));
    class = classes[0];
    double distance = 100;
    for (int i = 0; i < DATA_LENGTH; i++)
    {
        double inputAttributes[4] = {input.sepal_length, input.sepal_width, input.petal_length, input.petal_width};
        double currentDistance = dist(inputAttributes, attributes[i]);
        if (currentDistance < distance)
        {
            distance = currentDistance;
            class = classes[i];
        }
    }
    return class;
}

double dist(double attr1[4], double attr2[4])
{
    double distance = 0;
    for (int i = 0; i < 4; i++)
    {
        distance += (attr1[i] - attr2[i]) * (attr1[i] - attr2[i]);
    }
    distance = sqrt(distance);
    return distance;
}

char *knn(double attributes[DATA_LENGTH][4], char *classes[DATA_LENGTH], struct IrisInformation input, int k)
{
    double *distancias;
    char *classesDistancias[k];

    distancias = malloc(k * sizeof(double));
    distancias = initializeArray(k);
    int maxValueIndex = getMaxValueIndex(distancias, k);

    for (int i = 0; i < k; i++)
    {
        classesDistancias[i] = malloc(256 * sizeof(char));
    }

    for (int i = 0; i < DATA_LENGTH; i++)
    {
        double inputAttributes[4] = {input.sepal_length, input.sepal_width, input.petal_length, input.petal_width};
        double currentDistance = dist(inputAttributes, attributes[i]);
        if (currentDistance < distancias[maxValueIndex])
        {
            distancias[maxValueIndex] = currentDistance;
            classesDistancias[maxValueIndex] = classes[i];
            // printf("classesDistancias[%d]: %s\n", maxValueIndex, classesDistancias[maxValueIndex]);
            maxValueIndex = getMaxValueIndex(distancias, k);
        }
    }

    char *majoritaryClass = searchMajoritaryClass(classesDistancias, k);
    free(distancias);
    for (int i = 0; i < k; i++)
    {
        free(classesDistancias[i]);
    }

    return majoritaryClass;
}

char *searchMajoritaryClass(char **classes, int size)
{
    int virginica = 0;
    int versicolor = 0;
    int setosa = 0;

    for (int i = 0; i < size; i++)
    {
        // printf("classes[%d]: %s\n", i, classes[i]);
        if (strcmp(classes[i], "virginica\r\n") == 0)
        {
            virginica++;
        }
        else if (strcmp(classes[i], "versicolor\r\n") == 0)
        {
            versicolor++;
        }
        else if (strcmp(classes[i], "setosa\r\n") == 0)
        {
            setosa++;
        }
    }
    printf("virginica: %d\n", virginica);
    printf("versicolor: %d\n", versicolor);
    printf("setosa: %d\n", setosa);

    if (virginica >= versicolor && virginica >= setosa)
    {
        return "virginica\r\n";
    }
    else if (versicolor >= virginica && versicolor >= setosa)
    {
        return "versicolor\r\n";
    }
    else if (setosa >= virginica && setosa >= versicolor)
    {
        return "setosa\r\n";
    }
}

double *initializeArray(int size)
{
    double *array = malloc(size * sizeof(double));
    for (int i = 0; i < size; i++)
    {
        array[i] = 100.0; // Initialize with appropriate default value
    }
    return array;
}

int getMaxValueIndex(double *array, int size)
{
    int max = array[0];
    int maxIndex = 0;
    for (int i = 0; i < size; i++)
    {
        if (array[i] > max)
        {
            max = array[i];
            maxIndex = i;
        }
    }
    return maxIndex;
}